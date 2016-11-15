namespace ColtViewer
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
            this.checkPauseNPCUpdate = new System.Windows.Forms.CheckBox();
            this.lblUpdateTime = new System.Windows.Forms.Label();
            this.propertyGrid = new System.Windows.Forms.PropertyGrid();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.groupBoxB = new System.Windows.Forms.GroupBox();
            this.listBJobQueue = new AIMonitor.View.Controls.DoubleBufferedListView();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.btnClearLog = new System.Windows.Forms.Button();
            this.checkPauseLogUpdate = new System.Windows.Forms.CheckBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.listLog = new AIMonitor.View.Controls.DoubleBufferedListView();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.checkEvent = new System.Windows.Forms.CheckBox();
            this.checkShowGLog = new System.Windows.Forms.CheckBox();
            this.checkCallback = new System.Windows.Forms.CheckBox();
            this.CheckShowJobLog = new System.Windows.Forms.CheckBox();
            this.splitContainer4 = new System.Windows.Forms.SplitContainer();
            this.groupBoxM = new System.Windows.Forms.GroupBox();
            this.listMJobQueue = new AIMonitor.View.Controls.DoubleBufferedListView();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.listSJobQueue = new AIMonitor.View.Controls.DoubleBufferedListView();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.groupBoxB.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.splitContainer4.Panel1.SuspendLayout();
            this.splitContainer4.Panel2.SuspendLayout();
            this.splitContainer4.SuspendLayout();
            this.groupBoxM.SuspendLayout();
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
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer3);
            this.splitContainer1.Size = new System.Drawing.Size(1215, 690);
            this.splitContainer1.SplitterDistance = 295;
            this.splitContainer1.TabIndex = 3;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.checkPauseNPCUpdate);
            this.groupBox1.Controls.Add(this.lblUpdateTime);
            this.groupBox1.Controls.Add(this.propertyGrid);
            this.groupBox1.Location = new System.Drawing.Point(0, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(292, 691);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "NPC Property";
            // 
            // checkPauseNPCUpdate
            // 
            this.checkPauseNPCUpdate.Location = new System.Drawing.Point(8, 17);
            this.checkPauseNPCUpdate.Name = "checkPauseNPCUpdate";
            this.checkPauseNPCUpdate.Size = new System.Drawing.Size(106, 19);
            this.checkPauseNPCUpdate.TabIndex = 4;
            this.checkPauseNPCUpdate.Text = "Pause";
            this.checkPauseNPCUpdate.UseVisualStyleBackColor = true;
            // 
            // lblUpdateTime
            // 
            this.lblUpdateTime.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lblUpdateTime.Location = new System.Drawing.Point(6, 34);
            this.lblUpdateTime.Name = "lblUpdateTime";
            this.lblUpdateTime.Size = new System.Drawing.Size(274, 16);
            this.lblUpdateTime.TabIndex = 3;
            this.lblUpdateTime.Text = "never updated";
            this.lblUpdateTime.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // propertyGrid
            // 
            this.propertyGrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.propertyGrid.Location = new System.Drawing.Point(0, 50);
            this.propertyGrid.Name = "propertyGrid";
            this.propertyGrid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.propertyGrid.Size = new System.Drawing.Size(280, 634);
            this.propertyGrid.TabIndex = 2;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 0);
            this.splitContainer3.Name = "splitContainer3";
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.groupBox4);
            this.splitContainer3.Size = new System.Drawing.Size(916, 690);
            this.splitContainer3.SplitterDistance = 308;
            this.splitContainer3.TabIndex = 3;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer2.Location = new System.Drawing.Point(-2, 3);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.groupBoxB);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer4);
            this.splitContainer2.Size = new System.Drawing.Size(311, 684);
            this.splitContainer2.SplitterDistance = 137;
            this.splitContainer2.TabIndex = 3;
            // 
            // groupBoxB
            // 
            this.groupBoxB.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxB.Controls.Add(this.listBJobQueue);
            this.groupBoxB.Location = new System.Drawing.Point(3, 3);
            this.groupBoxB.Name = "groupBoxB";
            this.groupBoxB.Size = new System.Drawing.Size(306, 137);
            this.groupBoxB.TabIndex = 2;
            this.groupBoxB.TabStop = false;
            this.groupBoxB.Text = "BehaviorJob\'s Queue";
            // 
            // listBJobQueue
            // 
            this.listBJobQueue.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listBJobQueue.FullRowSelect = true;
            this.listBJobQueue.GridLines = true;
            this.listBJobQueue.Location = new System.Drawing.Point(6, 13);
            this.listBJobQueue.Name = "listBJobQueue";
            this.listBJobQueue.Size = new System.Drawing.Size(294, 118);
            this.listBJobQueue.TabIndex = 0;
            this.listBJobQueue.UseCompatibleStateImageBehavior = false;
            this.listBJobQueue.View = System.Windows.Forms.View.Details;
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.Controls.Add(this.btnClearLog);
            this.groupBox4.Controls.Add(this.checkPauseLogUpdate);
            this.groupBox4.Controls.Add(this.tabControl1);
            this.groupBox4.Location = new System.Drawing.Point(6, 5);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(595, 679);
            this.groupBox4.TabIndex = 0;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Log";
            // 
            // btnClearLog
            // 
            this.btnClearLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClearLog.Location = new System.Drawing.Point(511, 14);
            this.btnClearLog.Name = "btnClearLog";
            this.btnClearLog.Size = new System.Drawing.Size(73, 21);
            this.btnClearLog.TabIndex = 6;
            this.btnClearLog.Text = "Clear";
            this.btnClearLog.UseVisualStyleBackColor = true;
            this.btnClearLog.Click += new System.EventHandler(this.btnClearLog_Click);
            // 
            // checkPauseLogUpdate
            // 
            this.checkPauseLogUpdate.Location = new System.Drawing.Point(10, 17);
            this.checkPauseLogUpdate.Name = "checkPauseLogUpdate";
            this.checkPauseLogUpdate.Size = new System.Drawing.Size(106, 19);
            this.checkPauseLogUpdate.TabIndex = 5;
            this.checkPauseLogUpdate.Text = "Pause";
            this.checkPauseLogUpdate.UseVisualStyleBackColor = true;
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(6, 39);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(589, 634);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.listLog);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(581, 608);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "List";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // listLog
            // 
            this.listLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listLog.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.listLog.FullRowSelect = true;
            this.listLog.GridLines = true;
            this.listLog.Location = new System.Drawing.Point(6, 6);
            this.listLog.Name = "listLog";
            this.listLog.Size = new System.Drawing.Size(569, 596);
            this.listLog.SmallImageList = this.imageList1;
            this.listLog.TabIndex = 1;
            this.listLog.UseCompatibleStateImageBehavior = false;
            this.listLog.View = System.Windows.Forms.View.Details;
            // 
            // imageList1
            // 
            this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imageList1.ImageSize = new System.Drawing.Size(1, 24);
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.checkEvent);
            this.tabPage2.Controls.Add(this.checkShowGLog);
            this.tabPage2.Controls.Add(this.checkCallback);
            this.tabPage2.Controls.Add(this.CheckShowJobLog);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(581, 608);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Filter";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // checkEvent
            // 
            this.checkEvent.Checked = true;
            this.checkEvent.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkEvent.Location = new System.Drawing.Point(14, 64);
            this.checkEvent.Name = "checkEvent";
            this.checkEvent.Size = new System.Drawing.Size(156, 19);
            this.checkEvent.TabIndex = 3;
            this.checkEvent.Text = "Show Event";
            this.checkEvent.UseVisualStyleBackColor = true;
            // 
            // checkShowGLog
            // 
            this.checkShowGLog.Checked = true;
            this.checkShowGLog.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkShowGLog.Location = new System.Drawing.Point(14, 89);
            this.checkShowGLog.Name = "checkShowGLog";
            this.checkShowGLog.Size = new System.Drawing.Size(156, 19);
            this.checkShowGLog.TabIndex = 2;
            this.checkShowGLog.Text = "Show GLog";
            this.checkShowGLog.UseVisualStyleBackColor = true;
            // 
            // checkCallback
            // 
            this.checkCallback.Checked = true;
            this.checkCallback.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkCallback.Location = new System.Drawing.Point(14, 39);
            this.checkCallback.Name = "checkCallback";
            this.checkCallback.Size = new System.Drawing.Size(156, 19);
            this.checkCallback.TabIndex = 1;
            this.checkCallback.Text = "Show Callback";
            this.checkCallback.UseVisualStyleBackColor = true;
            // 
            // CheckShowJobLog
            // 
            this.CheckShowJobLog.Checked = true;
            this.CheckShowJobLog.CheckState = System.Windows.Forms.CheckState.Checked;
            this.CheckShowJobLog.Location = new System.Drawing.Point(14, 14);
            this.CheckShowJobLog.Name = "CheckShowJobLog";
            this.CheckShowJobLog.Size = new System.Drawing.Size(156, 19);
            this.CheckShowJobLog.TabIndex = 0;
            this.CheckShowJobLog.Text = "Show Job";
            this.CheckShowJobLog.UseVisualStyleBackColor = true;
            // 
            // splitContainer4
            // 
            this.splitContainer4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer4.Location = new System.Drawing.Point(2, 3);
            this.splitContainer4.Name = "splitContainer4";
            this.splitContainer4.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer4.Panel1
            // 
            this.splitContainer4.Panel1.Controls.Add(this.groupBox2);
            // 
            // splitContainer4.Panel2
            // 
            this.splitContainer4.Panel2.Controls.Add(this.groupBoxM);
            this.splitContainer4.Size = new System.Drawing.Size(312, 540);
            this.splitContainer4.SplitterDistance = 171;
            this.splitContainer4.TabIndex = 0;
            // 
            // groupBoxM
            // 
            this.groupBoxM.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxM.Controls.Add(this.listMJobQueue);
            this.groupBoxM.Location = new System.Drawing.Point(6, 3);
            this.groupBoxM.Name = "groupBoxM";
            this.groupBoxM.Size = new System.Drawing.Size(306, 359);
            this.groupBoxM.TabIndex = 5;
            this.groupBoxM.TabStop = false;
            this.groupBoxM.Text = "MissionJob\'s Queue";
            // 
            // listMJobQueue
            // 
            this.listMJobQueue.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listMJobQueue.FullRowSelect = true;
            this.listMJobQueue.GridLines = true;
            this.listMJobQueue.Location = new System.Drawing.Point(5, 13);
            this.listMJobQueue.Name = "listMJobQueue";
            this.listMJobQueue.Size = new System.Drawing.Size(294, 340);
            this.listMJobQueue.TabIndex = 0;
            this.listMJobQueue.UseCompatibleStateImageBehavior = false;
            this.listMJobQueue.View = System.Windows.Forms.View.Details;
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.listSJobQueue);
            this.groupBox2.Location = new System.Drawing.Point(7, 8);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(306, 160);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "SessionJob\'s Queue";
            // 
            // listSJobQueue
            // 
            this.listSJobQueue.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listSJobQueue.FullRowSelect = true;
            this.listSJobQueue.GridLines = true;
            this.listSJobQueue.Location = new System.Drawing.Point(5, 13);
            this.listSJobQueue.Name = "listSJobQueue";
            this.listSJobQueue.Size = new System.Drawing.Size(294, 141);
            this.listSJobQueue.TabIndex = 0;
            this.listSJobQueue.UseCompatibleStateImageBehavior = false;
            this.listSJobQueue.View = System.Windows.Forms.View.Details;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1215, 690);
            this.Controls.Add(this.splitContainer1);
            this.Name = "frmMain";
            this.Text = "AI Monitor";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            this.splitContainer3.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.groupBoxB.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.splitContainer4.Panel1.ResumeLayout(false);
            this.splitContainer4.Panel2.ResumeLayout(false);
            this.splitContainer4.ResumeLayout(false);
            this.groupBoxM.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.PropertyGrid propertyGrid;
        private System.Windows.Forms.Label lblUpdateTime;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.GroupBox groupBoxB;
        private AIMonitor.View.Controls.DoubleBufferedListView listBJobQueue;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.CheckBox checkPauseNPCUpdate;
        private System.Windows.Forms.CheckBox checkPauseLogUpdate;
        private System.Windows.Forms.Button btnClearLog;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private AIMonitor.View.Controls.DoubleBufferedListView listLog;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.CheckBox CheckShowJobLog;
        private System.Windows.Forms.CheckBox checkShowGLog;
        private System.Windows.Forms.CheckBox checkCallback;
        private System.Windows.Forms.CheckBox checkEvent;
        private System.Windows.Forms.SplitContainer splitContainer4;
        private System.Windows.Forms.GroupBox groupBox2;
        private AIMonitor.View.Controls.DoubleBufferedListView listSJobQueue;
        private System.Windows.Forms.GroupBox groupBoxM;
        private AIMonitor.View.Controls.DoubleBufferedListView listMJobQueue;
    }
}

