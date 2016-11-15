namespace UpdateFilePackingTool
{
    partial class Main
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnTest = new System.Windows.Forms.Button();
            this.btnStartPacking = new System.Windows.Forms.Button();
            this.NextVersionlistBox = new System.Windows.Forms.ListBox();
            this.btnAnalysisDiffFile = new System.Windows.Forms.Button();
            this.CurVersionlistBox = new System.Windows.Forms.ListBox();
            this.btnSelPackingFilesFolder = new System.Windows.Forms.Button();
            this.btnSelNextVerFolder = new System.Windows.Forms.Button();
            this.btnSelCurVerFolder = new System.Windows.Forms.Button();
            this.textBoxPackingFilesFolder = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxNextVerFolder = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxCurVerFolder = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.UpdateFileslistBox = new System.Windows.Forms.ListBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.checkBoxCurVerFiles = new System.Windows.Forms.CheckBox();
            this.checkBoxNextVerFiles = new System.Windows.Forms.CheckBox();
            this.LoglistBox = new System.Windows.Forms.ListBox();
            this.label7 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btnTest
            // 
            this.btnTest.Location = new System.Drawing.Point(756, 28);
            this.btnTest.Name = "btnTest";
            this.btnTest.Size = new System.Drawing.Size(98, 51);
            this.btnTest.TabIndex = 27;
            this.btnTest.Text = "Test";
            this.btnTest.UseVisualStyleBackColor = true;
            this.btnTest.Visible = false;
            this.btnTest.Click += new System.EventHandler(this.btnTest_Click);
            // 
            // btnStartPacking
            // 
            this.btnStartPacking.Enabled = false;
            this.btnStartPacking.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnStartPacking.Location = new System.Drawing.Point(291, 104);
            this.btnStartPacking.Name = "btnStartPacking";
            this.btnStartPacking.Size = new System.Drawing.Size(276, 34);
            this.btnStartPacking.TabIndex = 26;
            this.btnStartPacking.Text = "Start Packing";
            this.btnStartPacking.UseVisualStyleBackColor = true;
            this.btnStartPacking.Click += new System.EventHandler(this.btnStartPacking_Click);
            // 
            // NextVersionlistBox
            // 
            this.NextVersionlistBox.FormattingEnabled = true;
            this.NextVersionlistBox.HorizontalScrollbar = true;
            this.NextVersionlistBox.Location = new System.Drawing.Point(211, 170);
            this.NextVersionlistBox.Name = "NextVersionlistBox";
            this.NextVersionlistBox.Size = new System.Drawing.Size(196, 472);
            this.NextVersionlistBox.TabIndex = 25;
            // 
            // btnAnalysisDiffFile
            // 
            this.btnAnalysisDiffFile.Font = new System.Drawing.Font("Gulim", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnAnalysisDiffFile.Location = new System.Drawing.Point(3, 104);
            this.btnAnalysisDiffFile.Name = "btnAnalysisDiffFile";
            this.btnAnalysisDiffFile.Size = new System.Drawing.Size(280, 34);
            this.btnAnalysisDiffFile.TabIndex = 24;
            this.btnAnalysisDiffFile.Text = "Analyze";
            this.btnAnalysisDiffFile.UseVisualStyleBackColor = true;
            this.btnAnalysisDiffFile.Click += new System.EventHandler(this.btnAnalysisDiffFile_Click);
            // 
            // CurVersionlistBox
            // 
            this.CurVersionlistBox.FormattingEnabled = true;
            this.CurVersionlistBox.HorizontalScrollbar = true;
            this.CurVersionlistBox.Location = new System.Drawing.Point(10, 170);
            this.CurVersionlistBox.Name = "CurVersionlistBox";
            this.CurVersionlistBox.Size = new System.Drawing.Size(193, 472);
            this.CurVersionlistBox.TabIndex = 23;
            // 
            // btnSelPackingFilesFolder
            // 
            this.btnSelPackingFilesFolder.Location = new System.Drawing.Point(651, 72);
            this.btnSelPackingFilesFolder.Name = "btnSelPackingFilesFolder";
            this.btnSelPackingFilesFolder.Size = new System.Drawing.Size(62, 23);
            this.btnSelPackingFilesFolder.TabIndex = 22;
            this.btnSelPackingFilesFolder.Text = "Open";
            this.btnSelPackingFilesFolder.UseVisualStyleBackColor = true;
            this.btnSelPackingFilesFolder.Click += new System.EventHandler(this.btnSelFackingFilesFolder_Click);
            // 
            // btnSelNextVerFolder
            // 
            this.btnSelNextVerFolder.Location = new System.Drawing.Point(651, 42);
            this.btnSelNextVerFolder.Name = "btnSelNextVerFolder";
            this.btnSelNextVerFolder.Size = new System.Drawing.Size(62, 23);
            this.btnSelNextVerFolder.TabIndex = 21;
            this.btnSelNextVerFolder.Text = "Open";
            this.btnSelNextVerFolder.UseVisualStyleBackColor = true;
            this.btnSelNextVerFolder.Click += new System.EventHandler(this.btnSelNextVerFolder_Click);
            // 
            // btnSelCurVerFolder
            // 
            this.btnSelCurVerFolder.Location = new System.Drawing.Point(651, 12);
            this.btnSelCurVerFolder.Name = "btnSelCurVerFolder";
            this.btnSelCurVerFolder.Size = new System.Drawing.Size(62, 23);
            this.btnSelCurVerFolder.TabIndex = 20;
            this.btnSelCurVerFolder.Text = "Open";
            this.btnSelCurVerFolder.UseVisualStyleBackColor = true;
            this.btnSelCurVerFolder.Click += new System.EventHandler(this.btnSelCurVerFolder_Click);
            // 
            // textBoxPackingFilesFolder
            // 
            this.textBoxPackingFilesFolder.Location = new System.Drawing.Point(124, 72);
            this.textBoxPackingFilesFolder.Name = "textBoxPackingFilesFolder";
            this.textBoxPackingFilesFolder.Size = new System.Drawing.Size(522, 20);
            this.textBoxPackingFilesFolder.TabIndex = 19;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 76);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(95, 13);
            this.label3.TabIndex = 18;
            this.label3.Text = "Pack File Location";
            // 
            // textBoxNextVerFolder
            // 
            this.textBoxNextVerFolder.Location = new System.Drawing.Point(124, 42);
            this.textBoxNextVerFolder.Name = "textBoxNextVerFolder";
            this.textBoxNextVerFolder.Size = new System.Drawing.Size(522, 20);
            this.textBoxNextVerFolder.TabIndex = 17;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 47);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(99, 13);
            this.label2.TabIndex = 16;
            this.label2.Text = "New Version Folder";
            // 
            // textBoxCurVerFolder
            // 
            this.textBoxCurVerFolder.Location = new System.Drawing.Point(124, 13);
            this.textBoxCurVerFolder.Name = "textBoxCurVerFolder";
            this.textBoxCurVerFolder.Size = new System.Drawing.Size(522, 20);
            this.textBoxCurVerFolder.TabIndex = 15;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(111, 13);
            this.label1.TabIndex = 14;
            this.label1.Text = "Current Version Folder";
            // 
            // UpdateFileslistBox
            // 
            this.UpdateFileslistBox.FormattingEnabled = true;
            this.UpdateFileslistBox.HorizontalScrollbar = true;
            this.UpdateFileslistBox.Location = new System.Drawing.Point(415, 170);
            this.UpdateFileslistBox.Name = "UpdateFileslistBox";
            this.UpdateFileslistBox.Size = new System.Drawing.Size(217, 472);
            this.UpdateFileslistBox.TabIndex = 28;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 154);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(98, 13);
            this.label4.TabIndex = 29;
            this.label4.Text = "Current File Version";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(213, 154);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(86, 13);
            this.label5.TabIndex = 30;
            this.label5.Text = "New File Version";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(415, 154);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(97, 13);
            this.label6.TabIndex = 31;
            this.label6.Text = "Files to be updated";
            // 
            // checkBoxCurVerFiles
            // 
            this.checkBoxCurVerFiles.AutoSize = true;
            this.checkBoxCurVerFiles.Checked = true;
            this.checkBoxCurVerFiles.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxCurVerFiles.Location = new System.Drawing.Point(109, 152);
            this.checkBoxCurVerFiles.Name = "checkBoxCurVerFiles";
            this.checkBoxCurVerFiles.Size = new System.Drawing.Size(53, 17);
            this.checkBoxCurVerFiles.TabIndex = 32;
            this.checkBoxCurVerFiles.Text = "Show";
            this.checkBoxCurVerFiles.UseVisualStyleBackColor = true;
            // 
            // checkBoxNextVerFiles
            // 
            this.checkBoxNextVerFiles.AutoSize = true;
            this.checkBoxNextVerFiles.Checked = true;
            this.checkBoxNextVerFiles.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxNextVerFiles.Location = new System.Drawing.Point(311, 152);
            this.checkBoxNextVerFiles.Name = "checkBoxNextVerFiles";
            this.checkBoxNextVerFiles.Size = new System.Drawing.Size(53, 17);
            this.checkBoxNextVerFiles.TabIndex = 33;
            this.checkBoxNextVerFiles.Text = "Show";
            this.checkBoxNextVerFiles.UseVisualStyleBackColor = true;
            // 
            // LoglistBox
            // 
            this.LoglistBox.FormattingEnabled = true;
            this.LoglistBox.HorizontalScrollbar = true;
            this.LoglistBox.Location = new System.Drawing.Point(637, 170);
            this.LoglistBox.Name = "LoglistBox";
            this.LoglistBox.Size = new System.Drawing.Size(217, 472);
            this.LoglistBox.TabIndex = 34;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(635, 152);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(25, 13);
            this.label7.TabIndex = 35;
            this.label7.Text = "Log";
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(864, 653);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.LoglistBox);
            this.Controls.Add(this.checkBoxNextVerFiles);
            this.Controls.Add(this.checkBoxCurVerFiles);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.UpdateFileslistBox);
            this.Controls.Add(this.btnTest);
            this.Controls.Add(this.btnStartPacking);
            this.Controls.Add(this.NextVersionlistBox);
            this.Controls.Add(this.btnAnalysisDiffFile);
            this.Controls.Add(this.CurVersionlistBox);
            this.Controls.Add(this.btnSelPackingFilesFolder);
            this.Controls.Add(this.btnSelNextVerFolder);
            this.Controls.Add(this.btnSelCurVerFolder);
            this.Controls.Add(this.textBoxPackingFilesFolder);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBoxNextVerFolder);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBoxCurVerFolder);
            this.Controls.Add(this.label1);
            this.Name = "Main";
            this.Text = "RaiderZ Update Packing Tool";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnTest;
        private System.Windows.Forms.Button btnStartPacking;
        private System.Windows.Forms.ListBox NextVersionlistBox;
        private System.Windows.Forms.Button btnAnalysisDiffFile;
        private System.Windows.Forms.ListBox CurVersionlistBox;
        private System.Windows.Forms.Button btnSelPackingFilesFolder;
        private System.Windows.Forms.Button btnSelNextVerFolder;
        private System.Windows.Forms.Button btnSelCurVerFolder;
        private System.Windows.Forms.TextBox textBoxPackingFilesFolder;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxNextVerFolder;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxCurVerFolder;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.ListBox UpdateFileslistBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.CheckBox checkBoxCurVerFiles;
        private System.Windows.Forms.CheckBox checkBoxNextVerFiles;
        private System.Windows.Forms.ListBox LoglistBox;
        private System.Windows.Forms.Label label7;
    }
}

