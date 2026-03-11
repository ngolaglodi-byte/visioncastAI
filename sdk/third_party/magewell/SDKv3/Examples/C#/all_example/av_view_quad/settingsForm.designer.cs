namespace AVViewQuad
{
    partial class settingsForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.okButton = new System.Windows.Forms.Button();
            this.layoutComboBox = new System.Windows.Forms.ComboBox();
            this.resolutionComboBox = new System.Windows.Forms.ComboBox();
            this.framerateComboBox = new System.Windows.Forms.ComboBox();
            this.cancelButton = new System.Windows.Forms.Button();
            this.layoutLabel = new System.Windows.Forms.Label();
            this.resolutionLabel = new System.Windows.Forms.Label();
            this.framerateLabel = new System.Windows.Forms.Label();
            this.deviceCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.syncCheckBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // okButton
            // 
            this.okButton.Location = new System.Drawing.Point(354, 210);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 3;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.OkButton_Click);
            // 
            // layoutComboBox
            // 
            this.layoutComboBox.Items.AddRange(new object[] {
            "2x2",
            "2x1",
            "1x1"});
            this.layoutComboBox.Location = new System.Drawing.Point(57, 0);
            this.layoutComboBox.Name = "layoutComboBox";
            this.layoutComboBox.Size = new System.Drawing.Size(83, 20);
            this.layoutComboBox.TabIndex = 4;
            this.layoutComboBox.SelectedIndexChanged += new System.EventHandler(this.layoutComboBox_SelectedIndexChanged);
            // 
            // resolutionComboBox
            // 
            this.resolutionComboBox.Items.AddRange(new object[] {
            "720 x 576",
            "1280 x 720",
            "1920 x 1080",
            "2048 x 1080"});
            this.resolutionComboBox.Location = new System.Drawing.Point(227, 0);
            this.resolutionComboBox.Name = "resolutionComboBox";
            this.resolutionComboBox.Size = new System.Drawing.Size(100, 20);
            this.resolutionComboBox.TabIndex = 0;
            this.resolutionComboBox.SelectedIndexChanged += new System.EventHandler(this.resolutionComboBox_SelectedIndexChanged);
            // 
            // framerateComboBox
            // 
            this.framerateComboBox.Items.AddRange(new object[] {
            "25.00",
            "29.97",
            "30.00",
            "59.94",
            "60.00"});
            this.framerateComboBox.Location = new System.Drawing.Point(416, 0);
            this.framerateComboBox.Name = "framerateComboBox";
            this.framerateComboBox.Size = new System.Drawing.Size(92, 20);
            this.framerateComboBox.TabIndex = 0;
            this.framerateComboBox.SelectedIndexChanged += new System.EventHandler(this.framerateComboBox_SelectedIndexChanged);
            // 
            // cancelButton
            // 
            this.cancelButton.Location = new System.Drawing.Point(435, 210);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 5;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // layoutLabel
            // 
            this.layoutLabel.Location = new System.Drawing.Point(12, 3);
            this.layoutLabel.Name = "layoutLabel";
            this.layoutLabel.Size = new System.Drawing.Size(48, 23);
            this.layoutLabel.TabIndex = 6;
            this.layoutLabel.Text = "layout:";
            // 
            // resolutionLabel
            // 
            this.resolutionLabel.Location = new System.Drawing.Point(158, 3);
            this.resolutionLabel.Name = "resolutionLabel";
            this.resolutionLabel.Size = new System.Drawing.Size(74, 23);
            this.resolutionLabel.TabIndex = 7;
            this.resolutionLabel.Text = "resolution:";
            // 
            // framerateLabel
            // 
            this.framerateLabel.Location = new System.Drawing.Point(352, 3);
            this.framerateLabel.Name = "framerateLabel";
            this.framerateLabel.Size = new System.Drawing.Size(82, 23);
            this.framerateLabel.TabIndex = 8;
            this.framerateLabel.Text = "framerate:";
            // 
            // deviceCheckedListBox
            // 
            this.deviceCheckedListBox.CheckOnClick = true;
            this.deviceCheckedListBox.Location = new System.Drawing.Point(0, 26);
            this.deviceCheckedListBox.Name = "deviceCheckedListBox";
            this.deviceCheckedListBox.Size = new System.Drawing.Size(510, 180);
            this.deviceCheckedListBox.TabIndex = 10;
            this.deviceCheckedListBox.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.deviceCheckedListBox_ItemCheck);
            // 
            // syncCheckBox
            // 
            this.syncCheckBox.Enabled = false;
            this.syncCheckBox.Location = new System.Drawing.Point(36, 209);
            this.syncCheckBox.Name = "syncCheckBox";
            this.syncCheckBox.Size = new System.Drawing.Size(138, 24);
            this.syncCheckBox.TabIndex = 11;
            this.syncCheckBox.Text = "Synchronized Mode";
            this.syncCheckBox.CheckedChanged += new System.EventHandler(this.syncCheckBox_CheckedChanged);
            // 
            // settingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(517, 245);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.layoutComboBox);
            this.Controls.Add(this.resolutionComboBox);
            this.Controls.Add(this.framerateComboBox);
            this.Controls.Add(this.layoutLabel);
            this.Controls.Add(this.resolutionLabel);
            this.Controls.Add(this.framerateLabel);
            this.Controls.Add(this.deviceCheckedListBox);
            this.Controls.Add(this.syncCheckBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "settingsForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "settings";
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;

        private System.Windows.Forms.ComboBox layoutComboBox;
        private System.Windows.Forms.ComboBox resolutionComboBox;
        private System.Windows.Forms.ComboBox framerateComboBox;

        private System.Windows.Forms.Label layoutLabel;
        private System.Windows.Forms.Label resolutionLabel;
        private System.Windows.Forms.Label framerateLabel;
        private System.Windows.Forms.CheckedListBox deviceCheckedListBox;
        private System.Windows.Forms.CheckBox syncCheckBox;
    }
}