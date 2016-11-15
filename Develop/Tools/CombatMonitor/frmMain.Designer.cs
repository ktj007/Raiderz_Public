namespace CombatMonitor
{
    partial class frmMain
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
            this.components = new System.ComponentModel.Container();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.listViewAnalysis = new System.Windows.Forms.ListView();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.listViewBasic = new System.Windows.Forms.ListView();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.comboStartType = new System.Windows.Forms.ComboBox();
            this.txtStartParam = new System.Windows.Forms.TextBox();
            this.lblElapsed = new System.Windows.Forms.Label();
            this.checkLimitTime = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtLimitTime = new System.Windows.Forms.TextBox();
            this.BtnStart = new System.Windows.Forms.Button();
            this.timerTick = new System.Windows.Forms.Timer(this.components);
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.groupBox3);
            this.splitContainer1.Panel2.Controls.Add(this.groupBox2);
            this.splitContainer1.Size = new System.Drawing.Size(1046, 693);
            this.splitContainer1.SplitterDistance = 254;
            this.splitContainer1.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.treeView1);
            this.groupBox1.Location = new System.Drawing.Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(248, 690);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "액터 목록";
            // 
            // treeView1
            // 
            this.treeView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.treeView1.Location = new System.Drawing.Point(12, 20);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(236, 658);
            this.treeView1.TabIndex = 4;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.tabControl1);
            this.groupBox3.Location = new System.Drawing.Point(4, 100);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(781, 590);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "정보";
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(6, 20);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(769, 564);
            this.tabControl1.TabIndex = 2;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.listViewAnalysis);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(761, 538);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "분석";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // listViewAnalysis
            // 
            this.listViewAnalysis.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewAnalysis.FullRowSelect = true;
            this.listViewAnalysis.GridLines = true;
            this.listViewAnalysis.Location = new System.Drawing.Point(-4, -1);
            this.listViewAnalysis.Name = "listViewAnalysis";
            this.listViewAnalysis.Size = new System.Drawing.Size(766, 539);
            this.listViewAnalysis.TabIndex = 2;
            this.listViewAnalysis.UseCompatibleStateImageBehavior = false;
            this.listViewAnalysis.View = System.Windows.Forms.View.Details;
            this.listViewAnalysis.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.listViewAnalysis_ColumnClick);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.listViewBasic);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(761, 538);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "세부";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // listViewBasic
            // 
            this.listViewBasic.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewBasic.FullRowSelect = true;
            this.listViewBasic.GridLines = true;
            this.listViewBasic.Location = new System.Drawing.Point(-3, 0);
            this.listViewBasic.Name = "listViewBasic";
            this.listViewBasic.Size = new System.Drawing.Size(766, 539);
            this.listViewBasic.TabIndex = 3;
            this.listViewBasic.UseCompatibleStateImageBehavior = false;
            this.listViewBasic.View = System.Windows.Forms.View.Details;
            this.listViewBasic.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.listViewBasic_ColumnClick);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.comboStartType);
            this.groupBox2.Controls.Add(this.txtStartParam);
            this.groupBox2.Controls.Add(this.lblElapsed);
            this.groupBox2.Controls.Add(this.checkLimitTime);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.txtLimitTime);
            this.groupBox2.Controls.Add(this.BtnStart);
            this.groupBox2.Location = new System.Drawing.Point(4, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(781, 91);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "설정";
            // 
            // comboStartType
            // 
            this.comboStartType.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.comboStartType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboStartType.FormattingEnabled = true;
            this.comboStartType.Location = new System.Drawing.Point(525, 64);
            this.comboStartType.Name = "comboStartType";
            this.comboStartType.Size = new System.Drawing.Size(78, 20);
            this.comboStartType.TabIndex = 4;
            // 
            // txtStartParam
            // 
            this.txtStartParam.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.txtStartParam.Location = new System.Drawing.Point(609, 64);
            this.txtStartParam.Name = "txtStartParam";
            this.txtStartParam.Size = new System.Drawing.Size(82, 21);
            this.txtStartParam.TabIndex = 6;
            // 
            // lblElapsed
            // 
            this.lblElapsed.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblElapsed.Font = new System.Drawing.Font("굴림", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lblElapsed.ForeColor = System.Drawing.SystemColors.Highlight;
            this.lblElapsed.Location = new System.Drawing.Point(606, 44);
            this.lblElapsed.Name = "lblElapsed";
            this.lblElapsed.Size = new System.Drawing.Size(169, 17);
            this.lblElapsed.TabIndex = 5;
            this.lblElapsed.Text = "진행된 시간: 30 초";
            this.lblElapsed.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.lblElapsed.Visible = false;
            // 
            // checkLimitTime
            // 
            this.checkLimitTime.AutoSize = true;
            this.checkLimitTime.Location = new System.Drawing.Point(6, 20);
            this.checkLimitTime.Name = "checkLimitTime";
            this.checkLimitTime.Size = new System.Drawing.Size(76, 16);
            this.checkLimitTime.TabIndex = 3;
            this.checkLimitTime.Text = "자동 중지";
            this.checkLimitTime.UseVisualStyleBackColor = true;
            this.checkLimitTime.Visible = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(92, 44);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(73, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "초 후에 중지";
            this.label2.Visible = false;
            // 
            // txtLimitTime
            // 
            this.txtLimitTime.Location = new System.Drawing.Point(6, 39);
            this.txtLimitTime.Name = "txtLimitTime";
            this.txtLimitTime.Size = new System.Drawing.Size(82, 21);
            this.txtLimitTime.TabIndex = 1;
            this.txtLimitTime.Visible = false;
            // 
            // BtnStart
            // 
            this.BtnStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.BtnStart.Location = new System.Drawing.Point(700, 64);
            this.BtnStart.Name = "BtnStart";
            this.BtnStart.Size = new System.Drawing.Size(75, 23);
            this.BtnStart.TabIndex = 0;
            this.BtnStart.Text = "시작(&S)";
            this.BtnStart.UseVisualStyleBackColor = true;
            this.BtnStart.Click += new System.EventHandler(this.BtnStart_Click);
            // 
            // timerTick
            // 
            this.timerTick.Interval = 1000;
            this.timerTick.Tick += new System.EventHandler(this.timerTick_Tick);
            // 
            // frmMain
            // 
            this.AcceptButton = this.BtnStart;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1046, 693);
            this.Controls.Add(this.splitContainer1);
            this.Name = "frmMain";
            this.Text = "Combat Monitor";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox checkLimitTime;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtLimitTime;
        private System.Windows.Forms.Button BtnStart;
        private System.Windows.Forms.Label lblElapsed;
        private System.Windows.Forms.ComboBox comboStartType;
        private System.Windows.Forms.TextBox txtStartParam;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.Timer timerTick;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.ListView listViewAnalysis;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.ListView listViewBasic;
    }
}

