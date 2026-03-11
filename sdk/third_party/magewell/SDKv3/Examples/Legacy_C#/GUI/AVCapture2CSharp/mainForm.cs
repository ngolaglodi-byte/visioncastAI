using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AVCapture2CSharp
{
    public partial class mainForm : Form
    {
        const int m_nDefaultChannelIndex = 0;

        int m_cx = 720;
        int m_cy = 480;
        UInt32 m_nFrameDuration = 400000;
        UInt32 m_dwFourcc = MWCap_FOURCC.MWCAP_FOURCC_YUY2;


        protected int m_nVideoChannelCount = 0;

        protected MWCapture m_MWCapture;

        public mainForm()
        {
            InitializeComponent();

            MWCapture.RefreshDevices();
            m_nVideoChannelCount = MWCapture.GetChannelCount();

            if (m_nVideoChannelCount == 0) {
                MessageBox.Show(this, "Can't find capture devices!", "AVCapture2CSharp", MessageBoxButtons.OK);
                return;
            }
            for (int i = 0; i < m_nVideoChannelCount; i++) {
                LibMWCapture.MWCAP_CHANNEL_INFO channelInfo = new LibMWCapture.MWCAP_CHANNEL_INFO();
                MWCapture.GetChannelInfobyIndex(i, ref channelInfo);

                System.Windows.Forms.ToolStripMenuItem channelToolStripMenuItem = new ToolStripMenuItem();
                channelToolStripMenuItem.Name = "channelToolStripMenuItem" + "1";
                channelToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
                channelToolStripMenuItem.Text = channelInfo.byBoardIndex + ":" + channelInfo.byChannelIndex + " " + channelInfo.szProductName;
                channelToolStripMenuItem.Tag = i;
                channelToolStripMenuItem.Click += new System.EventHandler(this.OnChannelItem);
                deviceToolStripMenuItem.DropDownItems.Add(channelToolStripMenuItem);
            }

            m_MWCapture = new MWCapture();
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == 0x0014) return;
            base.WndProc(ref m);
        }

        protected override void OnCreateControl()
        {
            base.OnCreateControl();

            IntPtr hWnd = Handle;
            if (m_nVideoChannelCount > 0) {
                m_MWCapture.OpenVideoChannel(m_nDefaultChannelIndex, m_dwFourcc, m_cx, m_cy, m_nFrameDuration, hWnd, this.ClientRectangle);
                ToolStripMenuItem anItem = (ToolStripMenuItem)deviceToolStripMenuItem.DropDownItems[m_nDefaultChannelIndex];
                anItem.Checked = true;
                timer1.Enabled = true;
            }
        }

        private void mainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if(m_MWCapture!=null)
                m_MWCapture.Destory();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            aboutForm about = new aboutForm();
            about.ShowDialog();
        }

        private void OnChannelItem(object sender, EventArgs e)
        {
            ToolStripMenuItem item = sender as ToolStripMenuItem;

            int nIndex = Convert.ToInt32(item.Tag);

            timer1.Enabled = false;
            m_MWCapture.Destory();
            m_MWCapture.OpenVideoChannel(nIndex, m_dwFourcc, m_cx, m_cy, m_nFrameDuration, Handle, this.ClientRectangle);

            for (int i = 0; i < deviceToolStripMenuItem.DropDownItems.Count; i++) {
                ToolStripMenuItem anIntem = (ToolStripMenuItem)deviceToolStripMenuItem.DropDownItems[i];
                anIntem.Checked = false;
            }

            item.Checked = true;
            timer1.Enabled = true;
        }

        private void OnUpdateStatusBar(object sender, EventArgs e)
        {
            try {
                double dFps = m_MWCapture.GetFps();
                String str = String.Format("{0:F}", dFps);
                toolStripStatusLabel1.Text = m_cx + "x" + m_cy + ",FPS:" + str;
            }
            catch (Exception ex){

            }
            System.GC.Collect();
        }
    }
}
