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
namespace AVViewSharpen
{
    public partial class mainForm : Form
    {
        Thread m_video_play_thread;
        Thread m_audio_play_thread;
        protected int m_channel_count = 0;
        Boolean m_capturing = false;
        protected CMWCapture m_capture = null;
        protected IntPtr m_d3d_renderer = IntPtr.Zero;
        protected IntPtr m_dsound_render = IntPtr.Zero;
        protected IntPtr m_venc = IntPtr.Zero;
        int m_sharpen_value = 0;
        Byte[] m_sharpen_frame = null;
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
            Int32 sample_rate;
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
            m_sharpen_frame = null;
        }
        void sharpen_yuv420(ref Byte[] frame) {
            int capture_width = 0;
            int capture_height = 0;
            m_capture.get_resolution(out capture_width, out capture_height);
            int loop_width = capture_width - 1;
            int loop_height = capture_height - 1;
            for (int i = 1; i < loop_height; i++){
                int top_pos = (i - 1) * capture_width;
                int now_pos = i * capture_width;
                int bottom_pos = (i + 1) * capture_width;

                for (int j = 1; j < loop_width; j++){
                    short temp = (short)(frame[now_pos + j] * (10 + m_sharpen_value * 9) / 10 - 
                        (frame[top_pos + j - 1] + frame[top_pos + j] + frame[top_pos + j + 1] +
                        frame[now_pos + j - 1] + frame[now_pos + j + 1] +
                        frame[bottom_pos + j - 1] + frame[bottom_pos + j] + frame[bottom_pos + j + 1]) * m_sharpen_value / 10);
                    if (temp > 255){
                        m_sharpen_frame[now_pos + j] = 255;
                    }
                    else if (temp < 0){
                        m_sharpen_frame[now_pos + j] = 0;
                    }
                    else{
                        m_sharpen_frame[now_pos + j] = (byte)(temp);
                    }

                }
            }
            IntPtr p_uv = Marshal.UnsafeAddrOfPinnedArrayElement(m_sharpen_frame, capture_width * capture_height);
            Marshal.Copy(frame, capture_width * capture_height, p_uv, capture_width * capture_height / 2);
        }

        void sharpen_yuyv(ref Byte[] frame)
        {
            int capture_width = 0;
            int capture_height = 0;
            m_capture.get_resolution(out capture_width, out capture_height);
            int loop_height = capture_height - 1;
            int loop_width = (capture_width - 1)*2;
            for (int i = 1; i < loop_height; i++){
                int top_pos = (i - 1) * capture_width * 2;
                int now_pos = i * capture_width * 2;
                int bottom_pos = (i + 1) * capture_width * 2;
                for (int j = 2; j < loop_width; j += 2){
                    short temp = (short)(frame[now_pos + j] * (10 + m_sharpen_value * 9) / 10 - 
                        (frame[top_pos + j - 2] + frame[top_pos + j] + frame[top_pos + j + 2] +
                        frame[now_pos + j - 2] + frame[now_pos + j + 2] +
                        frame[bottom_pos + j - 2] + frame[bottom_pos + j] + frame[bottom_pos + j + 2]) * m_sharpen_value / 10);
                    if (temp > 255){
                        m_sharpen_frame[now_pos + j] = 255;
                    }
                    else if (temp < 0){
                        m_sharpen_frame[now_pos + j] = 0;
                    }
                    else{
                        m_sharpen_frame[now_pos + j] = (byte)(temp);
                    }
                    m_sharpen_frame[now_pos + j+1] = frame[now_pos + j + 1];
                }
            }
        }

        void sharpen_frame(ref Byte[] frame)
        {
            UInt32 mw_fourcc = 0;
            m_capture.get_mw_fourcc(out mw_fourcc);
            if (MWFOURCC.MWFOURCC_NV12 == mw_fourcc){
                sharpen_yuv420(ref frame);
            }
            else if (MWFOURCC.MWFOURCC_YUY2 == mw_fourcc) {
                sharpen_yuyv(ref frame);
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
                if (m_sharpen_value != 0){
                    if (null == m_sharpen_frame) {
                        m_sharpen_frame = new byte[frame.buffer_len];
                    }
                    sharpen_frame(ref frame.p_buffer);
                    LibMWMedia.MWD3DRendererPushFrame(m_d3d_renderer, m_sharpen_frame, 0);
                }
                else {
                    LibMWMedia.MWD3DRendererPushFrame(m_d3d_renderer, frame.p_buffer, 0);
                }
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

        private void OnUpdateStatusBar(object sender, EventArgs e)
        {
            int width = 0, height = 0;
            if (null != m_capture){
                m_capture.get_resolution(out width, out height);
            }
            //try {
            double fps = (m_capture == null) ? 0.0 : m_capture.get_video_capture_fps();
            String str = String.Format("{0:F}", fps);
            toolStripStatusLabel.Text = width + "x" + height + ",capture fps:" + str;
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

        private void mainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            stop_capture();
            CMWCapture.Exit();
        }

        private void trackBar_Scroll(object sender, EventArgs e)
        {
            m_sharpen_value = Convert.ToInt16(trackBar.Value);
            Console.WriteLine("sharpen value:" + m_sharpen_value);
        }
    }
}
