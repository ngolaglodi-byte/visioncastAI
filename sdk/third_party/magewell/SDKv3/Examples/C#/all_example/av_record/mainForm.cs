using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using MWModle;
using System.IO;
using System.Runtime.InteropServices;
namespace AVRecord
{
    public partial class mainForm : Form
    {
        Thread m_video_play_thread;
        Thread m_audio_play_thread;
        Thread m_av_record_thread;
        protected int m_channel_count = 0;
        Boolean m_capturing = false;
        Boolean m_record = false;
        protected CMWCapture m_capture = null;
        protected IntPtr m_d3d_renderer = IntPtr.Zero;
        protected IntPtr m_dsound_render = IntPtr.Zero;

        UInt32 m_encode_frames = 0;
        UInt32 m_prev_encode_frames = 0;
        CMp4Record m_mp4_record = null;
        protected CMWVenc.MW_ENCODER_CALLBACK m_encoder_callback = null;
        public mainForm()
        {
            InitializeComponent();
            CMWCapture.Init();
            CMWCapture.RefreshDevices();
             m_channel_count = CMWCapture.GetChannelCount();
            if (m_channel_count == 0) {
                MessageBox.Show(this, "Can't find capture devices!", "GpuEncodeGui", MessageBoxButtons.OK);
                return;
            }

            for (int i = 0; i < m_channel_count; i++) {
                LibMWCapture.MWCAP_CHANNEL_INFO channelInfo = new LibMWCapture.MWCAP_CHANNEL_INFO();
                CMWCapture.GetChannelInfobyIndex(i, ref channelInfo);

                System.Windows.Forms.ToolStripMenuItem channelToolStripMenuItem = new ToolStripMenuItem();
                channelToolStripMenuItem.Name = "channelToolStripMenuItem" + i;
                channelToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
                channelToolStripMenuItem.Text = channelInfo.byBoardIndex + ":" + channelInfo.byChannelIndex + " " + channelInfo.szProductName;
                channelToolStripMenuItem.Tag = i;
                channelToolStripMenuItem.Click += new System.EventHandler(this.OnChannelItem);
                deviceToolStripMenuItem.DropDownItems.Add(channelToolStripMenuItem);
            }
            CMWVenc.mw_venc_init();
            Int32 gpu_num = CMWVenc.mw_venc_get_gpu_num();
            if (0 != gpu_num) {
                startRecordToolStripMenuItem.Enabled = true;
            }
            System.Windows.Forms.ToolStripMenuItem cpuStripMenuItem = new ToolStripMenuItem();
            cpuStripMenuItem.Name = "channelToolStripMenuItem" + "cpu";
            cpuStripMenuItem.Size = new System.Drawing.Size(152, 22);
            cpuStripMenuItem.Text = "Ffmpeg(libx264) on cpu";
            cpuStripMenuItem.Tag = -1;
            cpuStripMenuItem.Click += new System.EventHandler(this.OnGpuItem);
            startRecordToolStripMenuItem.DropDownItems.Add(cpuStripMenuItem);

            for (int i = 0; i < gpu_num; i++){
                CMWVenc.mw_venc_gpu_info_t info = new CMWVenc.mw_venc_gpu_info_t();
                CMWVenc.mw_venc_get_gpu_info_by_index(i, ref info);
                System.Windows.Forms.ToolStripMenuItem gpuStripMenuItem = new ToolStripMenuItem();
                gpuStripMenuItem.Name = "channelToolStripMenuItem" + i;
                gpuStripMenuItem.Size = new System.Drawing.Size(152, 22);
                gpuStripMenuItem.Text = info.gpu_name;
                gpuStripMenuItem.Tag = i;
                gpuStripMenuItem.Click += new System.EventHandler(this.OnGpuItem);
                startRecordToolStripMenuItem.DropDownItems.Add(gpuStripMenuItem);
            }
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == 0x0014) return;
            base.WndProc(ref m);
        }
        protected override void OnCreateControl()
        {
            base.OnCreateControl();
            if (m_channel_count == 0){
                return;
            }
            if (!start_capture(0)){
                return;
            }
            ToolStripMenuItem anItem = (ToolStripMenuItem)deviceToolStripMenuItem.DropDownItems[0];
            anItem.Checked = true;
            timer.Enabled = true;
        }
        private Boolean start_capture(Int32 index) {
            UInt32 mw_fourcc = 0;
            if (m_capturing) {
                return true;
            }
            m_capture = CMWCapture.mw_capture_factory(index);// new CMWCapturePro();
            m_capture.get_mw_fourcc(out mw_fourcc);
            if (null == m_capture){
                return false;
            }
            /*if (!m_capture.set_device(index)){
                return false;
            }*/
            if (!m_capture.start_capture(true, true)){
                return false;
            }
            m_capture.get_mw_fourcc(out mw_fourcc);
            m_capturing = true;
            bool reverse = false;
            bool mirror = false;
            Int32 width = 0;
            Int32 height = 0;
            m_capture.get_mw_fourcc(out mw_fourcc);

            m_capture.get_mirror_and_reverse(out mirror, out reverse);
            m_capture.get_resolution(out width, out height);
            m_d3d_renderer = LibMWMedia.MWCreateD3DRenderer(width, height, mw_fourcc, reverse, mirror, Handle);

            if (m_d3d_renderer == IntPtr.Zero){
                return false;
            }
            m_video_play_thread = new Thread(new ThreadStart(video_play));
            if (m_video_play_thread != null){
                m_video_play_thread.Start();
            }
            int sample_rate = 0;
            m_capture.get_audio_sample_rate(out sample_rate);
            m_dsound_render = LibMWMedia.MWCreateDSoundRenderer(sample_rate, 2, (Int32)m_capture.m_audio_buffer.m_buffer_size / 2 / 2, 10);
            if (m_dsound_render == IntPtr.Zero){
                return true;
            }
            m_audio_play_thread = new Thread(new ThreadStart(audio_play));
            if (m_audio_play_thread != null){
                m_audio_play_thread.Start();
            }
            return true;
        }
        private void stop_capture()
        {
            stop_record();
            if (!m_capturing) {
                return;
            }
            m_capturing = false;
            if (m_video_play_thread != null){
                m_video_play_thread.Join();
                m_video_play_thread = null;
            }
            if (m_d3d_renderer != IntPtr.Zero){
                LibMWMedia.MWDestroyD3DRenderer(m_d3d_renderer);
                m_d3d_renderer = IntPtr.Zero;
            }

            if (m_audio_play_thread != null){
                m_audio_play_thread.Join();
                m_audio_play_thread = null;
            }
            if (m_dsound_render != IntPtr.Zero){
                LibMWMedia.MWDestroyDSoundRenderer(m_dsound_render);
                m_dsound_render = IntPtr.Zero;
            }
            if (m_capture != null){
                m_capture.Dispose();
                m_capture = null;
            }
        }

        void video_play()
        {
            Console.WriteLine("video_play in");
            while (m_capturing){
                CRingBuffer.st_frame_t frame = m_capture.m_video_buffer.get_frame_to_render();
                if (frame.buffer_len == 0){
                    Thread.Sleep(5);
                    continue;
                }
                LibMWMedia.MWD3DRendererPushFrame(m_d3d_renderer, frame.p_buffer, 0);
            }
            CRingBuffer.st_frame_t out_frame = m_capture.m_video_buffer.get_buffer_by_index(0);
            Array.Clear(out_frame.p_buffer, 0, out_frame.p_buffer.Length);
            LibMWMedia.MWD3DRendererPushFrame(m_d3d_renderer, out_frame.p_buffer, 0);
            m_capture.m_video_buffer.stop_render();
            Console.WriteLine("video_play out");
        }
        void audio_play()
        {
            Console.WriteLine("audio_play in");
            while (m_capturing){
                CRingBuffer.st_frame_t frame = m_capture.m_audio_buffer.get_frame_to_render();
                if (frame.buffer_len == 0){
                    Thread.Sleep(5);
                    continue;
                }
                LibMWMedia.MWDSoundRendererPushFrame(m_dsound_render, frame.p_buffer, frame.frame_len);
            }
            m_capture.m_audio_buffer.stop_render();
            Console.WriteLine("audio_play out");
        }

        private CMWVenc.mw_venc_fourcc_t fourcc(UInt32 mw_fourcc)
        {
            if (MWFOURCC.MWFOURCC_NV12 == mw_fourcc){
                return CMWVenc.mw_venc_fourcc_t.MW_VENC_FOURCC_NV12;
            }
            else if (MWFOURCC.MWFOURCC_YUY2 == mw_fourcc){
                return CMWVenc.mw_venc_fourcc_t.MW_VENC_FOURCC_YUY2;
            }

            return CMWVenc.mw_venc_fourcc_t.MW_VENC_FOURCC_NV12;
        }
        private Boolean start_record(int index) {
            if (null == m_capture) {
                Console.WriteLine("please start capture first");
                return false;
            }
            m_mp4_record = new CMp4Record("test.mp4"); //new CMp4Record("中文.mp4");
            if (null == m_mp4_record) {
                return false;
            }
            int width = 0, height = 0;
            UInt32 mw_fourcc;
            Int64 frame_duration;
            if (index >= 0) {
                m_mp4_record.set_video_encoder_on_gpu(index);
            }
            Int32 sample_rate = 0;
            Int32 channels = 0;
            Int32 bit_per_sample = 0;
            if ((false == m_capture.get_resolution(out width, out height)) ||
                (false == m_capture.get_mw_fourcc(out mw_fourcc)) ||
                (false == m_capture.get_frame_duration(out frame_duration)) ||
                (false == m_mp4_record.set_video(width, height, mw_fourcc, frame_duration)) ||
                (false == m_capture.get_audio_sample_rate(out sample_rate)) ||
                (false == m_capture.get_audio_channels(out channels)) ||
                (false == m_capture.get_audio_bit_per_sample(out bit_per_sample)) ||
                (false == m_mp4_record.set_audio(sample_rate, channels, bit_per_sample))) {
                m_mp4_record.Dispose();
                m_mp4_record = null;
                return false;
            }
            m_record = true;
            m_av_record_thread = new Thread(new ThreadStart(av_record));
            if (null == m_av_record_thread){
                m_mp4_record.Dispose();
                m_mp4_record = null;
                return false;
            }
            m_av_record_thread.Start();
            return true;
        }
        private void stop_record()
        {
            m_record = false;
            if (null != m_av_record_thread){
                m_av_record_thread.Join();
                m_av_record_thread = null;
            }
            if (null != m_mp4_record){
                m_mp4_record.Dispose();
                m_mp4_record = null;
            }
            startRecordToolStripMenuItem.Enabled = true;
            stopRecordToolStripMenuItem.Enabled = false;
        }
        void av_record_sync()
        {
            CRingBuffer.st_frame_t v_frame = m_capture.m_video_buffer.get_frame_to_encode();
            Thread.Sleep(115);
            CRingBuffer.st_frame_t a_frame = m_capture.m_audio_buffer.get_frame_to_encode();
            while (m_capturing && m_record){
                if (v_frame.buffer_len == 0){
                    v_frame = m_capture.m_video_buffer.get_frame_to_encode();
                }
                
                if (a_frame.buffer_len == 0){
                    a_frame = m_capture.m_audio_buffer.get_frame_to_encode();
                }
                if (v_frame.buffer_len == 0 || a_frame.buffer_len == 0){
                    Thread.Sleep(5);
                    continue;
                }
                if ((v_frame.ts > a_frame.ts) && ((v_frame.ts - a_frame.ts) > 80)){
                    a_frame = m_capture.m_audio_buffer.get_frame_to_encode();
                }
                else if ((v_frame.ts < a_frame.ts) && ((a_frame.ts - v_frame.ts) > 80)){
                    v_frame = m_capture.m_video_buffer.get_frame_to_encode();
                }
                else {
                    m_mp4_record.write_video(v_frame.p_buffer, v_frame.ts);
                    m_encode_frames++;
                    m_mp4_record.write_audio(a_frame.p_buffer, (int)a_frame.frame_len, a_frame.ts);
                    break;
                }
            }
        }
        void av_record()
        {
            Console.WriteLine("gpu_encoder in");
            //av_record_sync();
            while (m_capturing && m_record){
                bool sleep = true;
                CRingBuffer.st_frame_t frame = m_capture.m_video_buffer.get_frame_to_encode();
                if (frame.buffer_len != 0){
                    m_mp4_record.write_video(frame.p_buffer, frame.ts);
                    m_encode_frames++;
                    sleep = false;
                }
                frame = m_capture.m_audio_buffer.get_frame_to_encode();
                if (frame.buffer_len != 0){
                    m_mp4_record.write_audio(frame.p_buffer, (int)frame.frame_len, frame.ts);
                    sleep = false;
                }
                if (sleep) {
                    Thread.Sleep(5);
                }
            }
            m_capture.m_audio_buffer.stop_encode();
            m_capture.m_video_buffer.stop_encode();
            Console.WriteLine("gpu_encoder out");
        }
        private void OnUpdateStatusBar(object sender, EventArgs e)
        {
            int width = 0, height = 0;
            if (null != m_capture){
                m_capture.get_resolution(out width, out height);
            }
            //            try {
            double fps = (m_capture == null) ? 0.0:m_capture.get_video_capture_fps();
            String str = String.Format("{0:F}", fps);
            toolStripStatusLabel.Text = width + "x" + height + ",capture fps:" + str+",encode fps:"+ (m_encode_frames - m_prev_encode_frames);
            m_prev_encode_frames = m_encode_frames;
//             }
//             catch (Exception ex){
// 
//             }
//             System.GC.Collect();
        }

        private void OnChannelItem(object sender, EventArgs e)
        {
            ToolStripMenuItem item = sender as ToolStripMenuItem;

            int index = Convert.ToInt32(item.Tag);
            stop_capture();
            timer.Enabled = false;
            toolStripStatusLabel.Text = "stop";
            if (item.Checked){
                item.Checked = false;
                return;
            }

            if (!start_capture(index)){
                return;
            }
            timer.Enabled = true;
            for (int i = 0; i < deviceToolStripMenuItem.DropDownItems.Count; i++){
                ToolStripMenuItem anIntem = (ToolStripMenuItem)deviceToolStripMenuItem.DropDownItems[i];
                anIntem.Checked = false;
            }

            item.Checked = true;
        }


        private void OnGpuItem(object sender, EventArgs e)
        {
            ToolStripMenuItem item = sender as ToolStripMenuItem;
            int index = Convert.ToInt32(item.Tag);
            if (!start_record(index)){
                return;
            }
            Console.WriteLine("encode on gpu:"+ item.Text);
            startRecordToolStripMenuItem.Enabled = false;
            stopRecordToolStripMenuItem.Enabled = true;
        }
        private void stopRecordToolStripMenuItem_Click(object sender, EventArgs e)
        {
            stop_record();

        }

        private void mainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            stop_capture();
            CMWCapture.Exit();
        }

        private void snapshotToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.Filter = "bmp(*.bmp)|*.bmp|png(*.png)|*.png";
            dialog.DefaultExt = "bmp(*.bmp)";
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string strFilePath = dialog.FileName;
                int nFilter = dialog.FilterIndex;
                //m_capture.set_snapshot_flag(strFilePath, nFilter);
            }
       }
    }
}
