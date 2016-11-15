namespace ServerMonitor
{
    partial class FormSettingsAddServer
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
            this.btnAdd = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.textboxServerName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textboxExePath = new System.Windows.Forms.TextBox();
            this.opendlgExeFilePath = new System.Windows.Forms.OpenFileDialog();
            this.btnFindExeFile = new System.Windows.Forms.Button();
            this.textboxProcessName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.textboxWorldID = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textboxServerID = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.comboboxServerType = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // btnAdd
            // 
            this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnAdd.Location = new System.Drawing.Point(12, 232);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(130, 23);
            this.btnAdd.TabIndex = 3;
            this.btnAdd.Text = "Add";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(151, 232);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(130, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // textboxServerName
            // 
            this.textboxServerName.Location = new System.Drawing.Point(128, 150);
            this.textboxServerName.Name = "textboxServerName";
            this.textboxServerName.ReadOnly = true;
            this.textboxServerName.Size = new System.Drawing.Size(154, 21);
            this.textboxServerName.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 153);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(79, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "Server Name";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(113, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "ExecutableFilePath";
            // 
            // textboxExePath
            // 
            this.textboxExePath.Location = new System.Drawing.Point(12, 58);
            this.textboxExePath.Name = "textboxExePath";
            this.textboxExePath.ReadOnly = true;
            this.textboxExePath.Size = new System.Drawing.Size(235, 21);
            this.textboxExePath.TabIndex = 1;
            // 
            // btnFindExeFile
            // 
            this.btnFindExeFile.Location = new System.Drawing.Point(253, 57);
            this.btnFindExeFile.Name = "btnFindExeFile";
            this.btnFindExeFile.Size = new System.Drawing.Size(28, 23);
            this.btnFindExeFile.TabIndex = 2;
            this.btnFindExeFile.Text = "...";
            this.btnFindExeFile.UseVisualStyleBackColor = true;
            this.btnFindExeFile.Click += new System.EventHandler(this.btnFindExeFile_Click);
            // 
            // textboxProcessName
            // 
            this.textboxProcessName.Location = new System.Drawing.Point(12, 109);
            this.textboxProcessName.Name = "textboxProcessName";
            this.textboxProcessName.ReadOnly = true;
            this.textboxProcessName.Size = new System.Drawing.Size(269, 21);
            this.textboxProcessName.TabIndex = 5;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 94);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(147, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "Expected Process Name";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 193);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(51, 12);
            this.label4.TabIndex = 4;
            this.label4.Text = "World ID";
            // 
            // textboxWorldID
            // 
            this.textboxWorldID.Location = new System.Drawing.Point(69, 190);
            this.textboxWorldID.Name = "textboxWorldID";
            this.textboxWorldID.ReadOnly = true;
            this.textboxWorldID.Size = new System.Drawing.Size(73, 21);
            this.textboxWorldID.TabIndex = 5;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(149, 193);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(56, 12);
            this.label5.TabIndex = 4;
            this.label5.Text = "Server ID";
            // 
            // textboxServerID
            // 
            this.textboxServerID.Location = new System.Drawing.Point(208, 190);
            this.textboxServerID.Name = "textboxServerID";
            this.textboxServerID.ReadOnly = true;
            this.textboxServerID.Size = new System.Drawing.Size(73, 21);
            this.textboxServerID.TabIndex = 5;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 15);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(74, 12);
            this.label6.TabIndex = 3;
            this.label6.Text = "Server Type";
            // 
            // comboboxServerType
            // 
            this.comboboxServerType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboboxServerType.FormattingEnabled = true;
            this.comboboxServerType.Location = new System.Drawing.Point(127, 12);
            this.comboboxServerType.Name = "comboboxServerType";
            this.comboboxServerType.Size = new System.Drawing.Size(155, 20);
            this.comboboxServerType.TabIndex = 6;
            // 
            // FormSettingsAddServer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(293, 267);
            this.ControlBox = false;
            this.Controls.Add(this.comboboxServerType);
            this.Controls.Add(this.textboxServerID);
            this.Controls.Add(this.textboxWorldID);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.textboxProcessName);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.btnFindExeFile);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textboxExePath);
            this.Controls.Add(this.textboxServerName);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnAdd);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "FormSettingsAddServer";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add New Server";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.TextBox textboxServerName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textboxExePath;
        private System.Windows.Forms.OpenFileDialog opendlgExeFilePath;
        private System.Windows.Forms.Button btnFindExeFile;
        private System.Windows.Forms.TextBox textboxProcessName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textboxWorldID;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textboxServerID;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox comboboxServerType;
    }
}