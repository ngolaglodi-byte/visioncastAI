using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MWModle;
namespace AVViewQuad
{
    public partial class settingsForm : Form
    {
        static int[,] resolution = { { 720, 576 }, { 1280, 720 }, { 1920, 1080 }, { 2048, 1080} };
        static Double[] fps = {25.00, 29.97, 30.00, 59.94, 60.00 };
        static Int32[] layout_num = { 4, 2, 1 };

        Int32 m_device_num = 0;
        Int32[] m_index_map = new Int32[64];


        int m_select_fps_index = 2;
        public double m_select_fps = 30.0;

        int m_select_resolution_index = 2;
        public Int32 m_select_width = 1920;
        public Int32 m_select_height = 1080;

        public Int32 m_select_layout_index = 0;
        Int32 m_can_select_num = 4;

        public Boolean m_sync_mode = false;

        Int32 m_temp_select_num = 0;
        public Int32 m_select_num = 0;
        public Int32[] m_select_index = new Int32[4];

        DialogResult m_ret = DialogResult.None;
        public settingsForm()
        {
            CMWCaptureQuad.RefreshDevices();
            InitializeComponent();
            int device_count = CMWCaptureQuad.GetChannelCount();
            for (int i = 0; i < device_count; i++) {
                LibMWCapture.MWCAP_CHANNEL_INFO channel_info = new LibMWCapture.MWCAP_CHANNEL_INFO();
                CMWCaptureQuad.GetChannelInfobyIndex(i, ref channel_info);
                if (CMWCaptureQuad.GetChannelInfobyIndex(i, ref channel_info) && ((UInt16)LibMWCapture.MW_FAMILY_ID.MW_FAMILY_ID_PRO_CAPTURE == channel_info.wFamilyID)) {
                    deviceCheckedListBox.Items.Add(channel_info.byBoardIndex + ":" + channel_info.byChannelIndex + " " + channel_info.szProductName);
                    m_index_map[m_device_num] = i;
                    m_device_num++;
                }
            }
        }
        public new DialogResult ShowDialog()
        {
            m_ret = DialogResult.Cancel;

            layoutComboBox.SelectedIndex = m_select_layout_index;
            resolutionComboBox.SelectedIndex = m_select_resolution_index;
            framerateComboBox.SelectedIndex = m_select_fps_index;

            m_can_select_num = layout_num[layoutComboBox.SelectedIndex];
            for (int i = 0, j=0; i < m_device_num; i++) {
                if ((j < m_select_num) && (m_select_index[j] == i))
                {
                    if (!deviceCheckedListBox.GetItemChecked(i))
                    {
                        deviceCheckedListBox.SetItemChecked(i, true);
                    }
                    j++;
                }
                else if (deviceCheckedListBox.GetItemChecked(i)) {
                    deviceCheckedListBox.SetItemChecked(i, false);
                }
            }

            m_temp_select_num = m_select_num;
            if (m_select_num >= 2)
            {
                syncCheckBox.CheckState = m_sync_mode ? CheckState.Checked:CheckState.Unchecked;
                syncCheckBox.Enabled = true;
            }
            else {
                syncCheckBox.CheckState = CheckState.Unchecked;
                syncCheckBox.Enabled = false;
            }
            base.ShowDialog();
            return m_ret;
        }
        private void OkButton_Click(object sender, EventArgs e) {
            bool change = false;
            if (m_select_layout_index != layoutComboBox.SelectedIndex) {
                change = true;
            }
            m_select_layout_index = layoutComboBox.SelectedIndex;

            if (m_select_resolution_index != resolutionComboBox.SelectedIndex)
            {
                change = true;
            }
            m_select_resolution_index = resolutionComboBox.SelectedIndex;
            m_select_width = resolution[resolutionComboBox.SelectedIndex,0];
            m_select_height = resolution[resolutionComboBox.SelectedIndex, 1];

            if (m_select_fps_index != framerateComboBox.SelectedIndex) {
                change = true;
            }
            m_select_fps_index = framerateComboBox.SelectedIndex;
            m_select_fps = fps[framerateComboBox.SelectedIndex];
            if (m_select_num != m_temp_select_num) {
                change = true;
            }
            m_select_num = m_temp_select_num;

            m_temp_select_num = 0;
            for (int i = 0; i < m_device_num; i++)
            {
                if (deviceCheckedListBox.GetItemChecked(i)) {
                    if (m_select_index[m_temp_select_num] != m_index_map[i]) {
                        change = true;
                    }
                    m_select_index[m_temp_select_num] = m_index_map[i];
                    m_temp_select_num++;
                    if (m_temp_select_num >= m_select_num) {
                        break;
                    }
                }
            }

            if (CheckState.Checked == syncCheckBox.CheckState)
            {
                if (!m_sync_mode) {
                    change = true;
                }
                m_sync_mode = true;
            }
            else {
                if (m_sync_mode)
                {
                    change = true;
                }
                m_sync_mode = false;
            }
            Hide();
            if (change)
            {
                m_ret = DialogResult.OK;
            }
            else {
                m_ret = DialogResult.None;
            }
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Hide();
            m_ret = DialogResult.Cancel;
        }

        private void deviceCheckedListBox_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (e.NewValue == CheckState.Checked)
            {
                if (m_temp_select_num >= m_can_select_num)
                {
                    e.NewValue = CheckState.Unchecked;
                    Console.WriteLine(deviceCheckedListBox.Items[e.Index]+" add fail");
                    return;
                }
                Console.WriteLine("add: "+ deviceCheckedListBox.Items[e.Index]);
                m_temp_select_num++;
                if (m_temp_select_num >= 2) {
                    syncCheckBox.Enabled = true;
                }
            }
            else {
                Console.WriteLine("delete: "+ deviceCheckedListBox.Items[e.Index]);
                m_temp_select_num--;
                if (m_temp_select_num < 2)
                {
                    syncCheckBox.CheckState = CheckState.Unchecked;
                    syncCheckBox.Enabled = false;
                }
            }
        }

        private void framerateComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //m_select_fps = fps[framerateComboBox.SelectedIndex];
            Console.WriteLine("select fps:"+ fps[framerateComboBox.SelectedIndex]);
        }

        private void resolutionComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
//             m_select_width = resolution[resolutionComboBox.SelectedIndex,0];
//             m_select_height = resolution[resolutionComboBox.SelectedIndex, 1];
            Console.WriteLine("select resolution:" + resolution[resolutionComboBox.SelectedIndex, 0] + "x"+ resolution[resolutionComboBox.SelectedIndex, 1]);
        }

        private void layoutComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_can_select_num = layout_num[layoutComboBox.SelectedIndex];
            if (m_can_select_num < m_temp_select_num) {
                int temp_num = m_can_select_num;
                for (int i = 0; i < m_device_num; i++)
                {
                    if (deviceCheckedListBox.GetItemChecked(i))
                    {
                        temp_num--;
                        if (temp_num < 0) {
                            deviceCheckedListBox.SetItemChecked(i, false);
                        }
                    }
                }
                m_temp_select_num = m_can_select_num;
                if (m_temp_select_num < 2)
                {
                    syncCheckBox.CheckState = CheckState.Unchecked;
                    syncCheckBox.Enabled = false;
                }
            }
            
//             m_layout_index = layoutComboBox.SelectedIndex;
            Console.WriteLine("select layout:" + m_can_select_num);
        }

        private void syncCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (CheckState.Checked == syncCheckBox.CheckState)
            {
                Console.WriteLine("set sync mode");
            }
            else {
                Console.WriteLine("not set sync mode");
            }
        }
    }
}
