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
namespace GpuEncodeGui
{
    public partial class mainForm : Form
    {
        Thread m_video_play_thread;
        Thread m_audio_play_thread;
        Thread m_gpu_encode_thread;
        protected int m_channel_count = 0;
        Boolean m_capturing = false;
        Boolean m_encode = false;
        protected CMWCapture m_capture = null;
        protected IntPtr m_d3d_renderer = IntPtr.Zero;
        protected IntPtr m_dsound_render = IntPtr.Zero;
        protected IntPtr m_venc = IntPtr.Zero;
        FileStream m_file = null;
        UInt32 m_encode_frames = 0;
        UInt32 m_prev_encode_frames = 0;
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
                startEncodeToolStripMenuItem.Enabled = true;
            }
            for (int i = 0; i < gpu_num; i++){
                CMWVenc.mw_venc_gpu_info_t info = new CMWVenc.mw_venc_gpu_info_t();
                CMWVenc.mw_venc_get_gpu_info_by_index(i, ref info);
                System.Windows.Forms.ToolStripMenuItem gpuStripMenuItem = new ToolStripMenuItem();
                gpuStripMenuItem.Name = "channelToolStripMenuItem" + i;
                gpuStripMenuItem.Size = new System.Drawing.Size(152, 22);
                gpuStripMenuItem.Text = info.gpu_name;
                gpuStripMenuItem.Tag = i;
                gpuStripMenuItem.Click += new System.EventHandler(this.OnGpuItem);
                startEncodeToolStripMenuItem.DropDownItems.Add(gpuStripMenuItem);
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
            if (m_capturing) {
                return true;
            }
            m_capture = CMWCapture.mw_capture_factory(index);// new CMWCapturePro();
            if (null == m_capture){
                return false;
            }
            /*if (!m_capture.set_device(index)){
                return false;
            }*/
            
            if (!m_capture.start_capture(true, true)){
                return false;
            }
            m_capturing = true;
            bool reverse = false;
            bool mirror = false;
            UInt32 mw_fourcc = 0;
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
            stop_encoder();
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
        private void encoder_frame_callback(IntPtr user_ptr, IntPtr p_frame, UInt32 frame_len, IntPtr p_frame_info)
        {
            if (null != user_ptr) {
                int written;
                Libkernel32.WriteFile(user_ptr, p_frame, (Int32)frame_len, out written, IntPtr.Zero);
            }
        }
        private Boolean start_encoder(int index) {
            if (null == m_capture) {
                Console.WriteLine("please start capture first");
                return false;
            }
            int width = 0, height = 0;
            UInt32 mw_fourcc;
            Int64 frame_duration;
            m_capture.get_resolution(out width, out height);
            m_capture.get_mw_fourcc(out mw_fourcc);
            m_capture.get_frame_duration(out frame_duration);
            CMWVenc.mw_venc_param_t enc_param = new CMWVenc.mw_venc_param_t();
            CMWVenc.mw_venc_get_default_param(ref enc_param);
            enc_param.width = width;
            enc_param.height = height;
            enc_param.fourcc = fourcc(mw_fourcc);
            enc_param.code_type = CMWVenc.mw_venc_code_type_t.MW_VENC_CODE_TYPE_H264;
            enc_param.rate_control.bitrate.target_bitrate = 1024;
            enc_param.rate_control.mode = CMWVenc.mw_venc_rate_control_mode_t.MW_VENC_RATECONTROL_CBR;
            enc_param.fps.den = (int)frame_duration;
            enc_param.fps.num = 1000000;
            enc_param.amd_mem_reserved = CMWVenc.mw_venc_amd_mem_type_t.MW_VENC_AMD_MEM_CPU;
            m_encoder_callback = new CMWVenc.MW_ENCODER_CALLBACK(encoder_frame_callback);

            m_file = new FileStream("test.h264",FileMode.Create, FileAccess.Write);

            m_venc = CMWVenc.mw_venc_create_by_index(index, ref enc_param, m_encoder_callback, (m_file == null)?IntPtr.Zero: m_file.Handle);
            if (IntPtr.Zero == m_venc) {
                return false;
            }
            m_encode = true;
            m_gpu_encode_thread = new Thread(new ThreadStart(gpu_encoder));
            if (null == m_gpu_encode_thread){
                CMWVenc.mw_venc_destory(m_venc);
                return false;
            }
            m_gpu_encode_thread.Start();
            return true;
        }
        private void stop_encoder()
        {
            m_encode = false;
            if (null != m_gpu_encode_thread){
                m_gpu_encode_thread.Join();
                m_gpu_encode_thread = null;
            }
            if (IntPtr.Zero != m_venc){
                CMWVenc.mw_venc_destory(m_venc);
                m_venc = IntPtr.Zero;
            }
            if (null != m_file){
                m_file.Close();
                m_file = null;
            }
        }
        void gpu_encoder()
        {
            Console.WriteLine("gpu_encoder in");
            while (m_capturing && m_encode){
                CRingBuffer.st_frame_t frame = m_capture.m_video_buffer.get_frame_to_encode();
                if (frame.buffer_len == 0){
                    Thread.Sleep(5);
                    continue;
                }
                CMWVenc.mw_venc_put_frame(m_venc, frame.p_buffer);
                m_encode_frames++;
            }
            m_capture.m_video_buffer.stop_encode();
            Console.WriteLine("gpu_encoder out");
        }
        private void OnUpdateStatusBar(object sender, EventArgs e)
        {
            int width = 0, height = 0;
            if (null != m_capture) {
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
            if (!start_encoder(index)){
                return;
            }
            Console.WriteLine("encode on gpu:"+ item.Text);
            startEncodeToolStripMenuItem.Enabled = false;
            stopEncodeToolStripMenuItem.Enabled = true;
        }
        private void stopEncodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            stop_encoder();
            startEncodeToolStripMenuItem.Enabled = true;
            stopEncodeToolStripMenuItem.Enabled = false;
        }

        private void mainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            stop_capture();
            CMWCapture.Exit();
        }
    }
}
