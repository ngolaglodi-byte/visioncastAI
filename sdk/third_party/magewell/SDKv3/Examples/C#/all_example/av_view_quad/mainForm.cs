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
namespace AVViewQuad
{
    public partial class mainForm : Form
    {
        Thread m_video_play_thread;
        protected int m_channel_count = 0;
        CMWCaptureQuad m_capture_quad = null;
        Boolean m_capturing = false;
        protected IntPtr m_d3d_renderer = IntPtr.Zero;
        protected IntPtr m_venc = IntPtr.Zero;
        settingsForm setting = null;
        public mainForm()
        {
            setting = new settingsForm();
            DialogResult ret = setting.ShowDialog();
            if (DialogResult.Cancel == ret) {
                Environment.Exit(0);
                return;
            }
            if (setting.m_select_num > 0) {
                start_capture();
            }
            InitializeComponent();
        }
        private void start_capture()
        {
            bool reverse = false;
            m_capture_quad = new CMWCaptureQuad();
            m_capture_quad.start_capture(setting.m_select_index, setting.m_select_num, setting.m_select_layout_index, 
                setting.m_select_width, setting.m_select_height, setting.m_select_fps, setting.m_sync_mode);
            m_capturing = true;
//             if (m_capture_quad.m_mw_fourcc == MWFOURCC.MWFOURCC_BGR24 || m_capture_quad.m_mw_fourcc == MWFOURCC.MWFOURCC_BGRA){
//                 reverse = true;
//             }
            m_d3d_renderer = LibMWMedia.MWCreateD3DRenderer(m_capture_quad.m_frame_width, m_capture_quad.m_frame_height, 
                m_capture_quad.m_mw_fourcc, reverse, false, Handle);

            if (m_d3d_renderer == IntPtr.Zero){
                return;
            }
            m_video_play_thread = new Thread(new ThreadStart(video_play));
            if (m_video_play_thread != null){
                m_video_play_thread.Start();
            }
        }
        void video_play()
        {
            Console.WriteLine("video_play in");
            while (m_capturing){
                CRingBuffer.st_frame_t frame = m_capture_quad.m_video_buffer.get_frame_to_render();
                if (frame.buffer_len == 0){
                    Thread.Sleep(5);
                    continue;
                }
                LibMWMedia.MWD3DRendererPushFrame(m_d3d_renderer, frame.p_buffer, 0);
            }
            CRingBuffer.st_frame_t out_frame = m_capture_quad.m_video_buffer.get_buffer_by_index(0);
            Array.Clear(out_frame.p_buffer, 0, out_frame.p_buffer.Length);
            LibMWMedia.MWD3DRendererPushFrame(m_d3d_renderer, out_frame.p_buffer, 0);
            m_capture_quad.m_video_buffer.stop_render();
            Console.WriteLine("video_play out");
        }
        private void stop_capture()
        {
            if (!m_capturing){
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
            if (null != m_capture_quad) {
                m_capture_quad.Dispose();
                m_capture_quad = null;
            }
        }
        private void OnUpdateStatusBar(object sender, EventArgs e)
        {
            //            try{
            if (m_capture_quad == null) {
                toolStripStatusLabel.Text = "stop";
                return;
            }
            String str = String.Format("{0:F}", m_capture_quad.get_video_capture_fps());
            toolStripStatusLabel.Text = "capture fps:" + str;
            //             }
            //             catch (Exception ex){
            // 
            //             }
            //             System.GC.Collect();
        }
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == 0x0014) return;
            base.WndProc(ref m);
        }
        protected override void OnCreateControl()
        {
            base.OnCreateControl();
            timer.Enabled = true;
        }

        private void settingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult ret = setting.ShowDialog();
            if (DialogResult.OK != ret) {
                return;
            }
            if ((setting.m_select_num > 0) && (null == m_capture_quad)){
                start_capture();
            }
            else if ((0 == setting.m_select_num) && (null != m_capture_quad)) {
                stop_capture();
            }
            else if ((setting.m_select_num > 0) && (null != m_capture_quad)) {
                stop_capture();
                start_capture();
            }
        }
        private void mainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            stop_capture();
        }
    }
}
