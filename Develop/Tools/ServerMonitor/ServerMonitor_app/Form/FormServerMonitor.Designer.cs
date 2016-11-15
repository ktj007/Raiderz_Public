namespace ServerMonitor
{
    partial class FormServerMonitor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormServerMonitor));
            this.panelMainView = new System.Windows.Forms.Panel();
            this.splitServerListnInfo = new System.Windows.Forms.SplitContainer();
            this.lviewServerList = new System.Windows.Forms.ListView();
            this.colRevive = new System.Windows.Forms.ColumnHeader();
            this.colProcessState = new System.Windows.Forms.ColumnHeader();
            this.colServable = new System.Windows.Forms.ColumnHeader();
            this.colServerName = new System.Windows.Forms.ColumnHeader();
            this.colServerType = new System.Windows.Forms.ColumnHeader();
            this.colWorldID = new System.Windows.Forms.ColumnHeader();
            this.colServerID = new System.Windows.Forms.ColumnHeader();
            this.colPlayerNum = new System.Windows.Forms.ColumnHeader();
            this.colLatestStartTime = new System.Windows.Forms.ColumnHeader();
            this.listBoxMonitoringLog = new System.Windows.Forms.ListBox();
            this.panelControlButtons = new System.Windows.Forms.Panel();
            this.btnShowDetail = new System.Windows.Forms.Button();
            this.btnTerminate = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.menuServerMonitor = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSettings = new System.Windows.Forms.ToolStripMenuItem();
            this.reloadAllServersINIFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuClearLog = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.notifyIconServerMonitor = new System.Windows.Forms.NotifyIcon(this.components);
            this.ctxtmenuTray = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.ctxtmenuShowServerMonitor = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxtmenuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.statusServerMonitor = new System.Windows.Forms.StatusStrip();
            this.labelConnCount = new System.Windows.Forms.ToolStripStatusLabel();
            this.timerUpdate = new System.Windows.Forms.Timer(this.components);
            this.panelMainView.SuspendLayout();
            this.splitServerListnInfo.Panel1.SuspendLayout();
            this.splitServerListnInfo.Panel2.SuspendLayout();
            this.splitServerListnInfo.SuspendLayout();
            this.panelControlButtons.SuspendLayout();
            this.menuServerMonitor.SuspendLayout();
            this.ctxtmenuTray.SuspendLayout();
            this.statusServerMonitor.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelMainView
            // 
            this.panelMainView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelMainView.Controls.Add(this.splitServerListnInfo);
            this.panelMainView.Location = new System.Drawing.Point(0, 21);
            this.panelMainView.Name = "panelMainView";
            this.panelMainView.Size = new System.Drawing.Size(854, 238);
            this.panelMainView.TabIndex = 0;
            // 
            // splitServerListnInfo
            // 
            this.splitServerListnInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitServerListnInfo.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitServerListnInfo.Location = new System.Drawing.Point(3, 3);
            this.splitServerListnInfo.Name = "splitServerListnInfo";
            this.splitServerListnInfo.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitServerListnInfo.Panel1
            // 
            this.splitServerListnInfo.Panel1.Controls.Add(this.lviewServerList);
            // 
            // splitServerListnInfo.Panel2
            // 
            this.splitServerListnInfo.Panel2.Controls.Add(this.listBoxMonitoringLog);
            this.splitServerListnInfo.Size = new System.Drawing.Size(848, 232);
            this.splitServerListnInfo.SplitterDistance = 110;
            this.splitServerListnInfo.TabIndex = 0;
            // 
            // lviewServerList
            // 
            this.lviewServerList.CheckBoxes = true;
            this.lviewServerList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colRevive,
            this.colProcessState,
            this.colServable,
            this.colServerName,
            this.colServerType,
            this.colWorldID,
            this.colServerID,
            this.colPlayerNum,
            this.colLatestStartTime});
            this.lviewServerList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lviewServerList.FullRowSelect = true;
            this.lviewServerList.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.lviewServerList.HideSelection = false;
            this.lviewServerList.Location = new System.Drawing.Point(0, 0);
            this.lviewServerList.Name = "lviewServerList";
            this.lviewServerList.Size = new System.Drawing.Size(844, 106);
            this.lviewServerList.TabIndex = 0;
            this.lviewServerList.UseCompatibleStateImageBehavior = false;
            this.lviewServerList.View = System.Windows.Forms.View.Details;
            this.lviewServerList.SelectedIndexChanged += new System.EventHandler(this.lviewServerList_SelectedIndexChanged);
            this.lviewServerList.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.lviewServerList_ItemCheck);
            // 
            // colRevive
            // 
            this.colRevive.Text = "Revive";
            this.colRevive.Width = 53;
            // 
            // colProcessState
            // 
            this.colProcessState.Text = "ProcessState";
            this.colProcessState.Width = 99;
            // 
            // colServable
            // 
            this.colServable.Text = "Servable";
            this.colServable.Width = 92;
            // 
            // colServerName
            // 
            this.colServerName.Tag = "";
            this.colServerName.Text = "ServerName";
            this.colServerName.Width = 141;
            // 
            // colServerType
            // 
            this.colServerType.Text = "ServerType";
            this.colServerType.Width = 125;
            // 
            // colWorldID
            // 
            this.colWorldID.Text = "WorldID";
            this.colWorldID.Width = 57;
            // 
            // colServerID
            // 
            this.colServerID.Text = "ServerID";
            this.colServerID.Width = 62;
            // 
            // colPlayerNum
            // 
            this.colPlayerNum.Text = "PlayerNum";
            this.colPlayerNum.Width = 81;
            // 
            // colLatestStartTime
            // 
            this.colLatestStartTime.Text = "LatestStartTime";
            this.colLatestStartTime.Width = 129;
            // 
            // listBoxMonitoringLog
            // 
            this.listBoxMonitoringLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBoxMonitoringLog.FormattingEnabled = true;
            this.listBoxMonitoringLog.HorizontalScrollbar = true;
            this.listBoxMonitoringLog.ItemHeight = 12;
            this.listBoxMonitoringLog.Location = new System.Drawing.Point(0, 0);
            this.listBoxMonitoringLog.Name = "listBoxMonitoringLog";
            this.listBoxMonitoringLog.Size = new System.Drawing.Size(844, 112);
            this.listBoxMonitoringLog.TabIndex = 0;
            this.listBoxMonitoringLog.TabStop = false;
            // 
            // panelControlButtons
            // 
            this.panelControlButtons.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelControlButtons.Controls.Add(this.btnShowDetail);
            this.panelControlButtons.Controls.Add(this.btnTerminate);
            this.panelControlButtons.Controls.Add(this.btnStart);
            this.panelControlButtons.Location = new System.Drawing.Point(0, 257);
            this.panelControlButtons.Name = "panelControlButtons";
            this.panelControlButtons.Size = new System.Drawing.Size(854, 47);
            this.panelControlButtons.TabIndex = 1;
            // 
            // btnShowDetail
            // 
            this.btnShowDetail.Enabled = false;
            this.btnShowDetail.Location = new System.Drawing.Point(237, 8);
            this.btnShowDetail.Name = "btnShowDetail";
            this.btnShowDetail.Size = new System.Drawing.Size(110, 29);
            this.btnShowDetail.TabIndex = 2;
            this.btnShowDetail.Text = "Detail Info";
            this.btnShowDetail.UseVisualStyleBackColor = true;
            this.btnShowDetail.Click += new System.EventHandler(this.btnShowDetail_Click);
            // 
            // btnTerminate
            // 
            this.btnTerminate.Location = new System.Drawing.Point(121, 8);
            this.btnTerminate.Name = "btnTerminate";
            this.btnTerminate.Size = new System.Drawing.Size(110, 29);
            this.btnTerminate.TabIndex = 1;
            this.btnTerminate.Text = "Terminate";
            this.btnTerminate.UseVisualStyleBackColor = true;
            this.btnTerminate.Click += new System.EventHandler(this.btnTerminate_Click);
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(5, 8);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(110, 29);
            this.btnStart.TabIndex = 0;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // menuServerMonitor
            // 
            this.menuServerMonitor.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuServerMonitor.Location = new System.Drawing.Point(0, 0);
            this.menuServerMonitor.Name = "menuServerMonitor";
            this.menuServerMonitor.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.menuServerMonitor.Size = new System.Drawing.Size(854, 24);
            this.menuServerMonitor.TabIndex = 2;
            this.menuServerMonitor.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuSettings,
            this.reloadAllServersINIFileToolStripMenuItem,
            this.menuClearLog,
            this.toolStripSeparator1,
            this.menuExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
            this.fileToolStripMenuItem.Text = "동작";
            // 
            // menuSettings
            // 
            this.menuSettings.Name = "menuSettings";
            this.menuSettings.Size = new System.Drawing.Size(223, 22);
            this.menuSettings.Text = "Settings";
            this.menuSettings.Click += new System.EventHandler(this.menuSettings_Click);
            // 
            // reloadAllServersINIFileToolStripMenuItem
            // 
            this.reloadAllServersINIFileToolStripMenuItem.Name = "reloadAllServersINIFileToolStripMenuItem";
            this.reloadAllServersINIFileToolStripMenuItem.Size = new System.Drawing.Size(223, 22);
            this.reloadAllServersINIFileToolStripMenuItem.Text = "Reload All Servers INI File";
            this.reloadAllServersINIFileToolStripMenuItem.Click += new System.EventHandler(this.reloadAllServersINIFileToolStripMenuItem_Click);
            // 
            // menuClearLog
            // 
            this.menuClearLog.Name = "menuClearLog";
            this.menuClearLog.Size = new System.Drawing.Size(223, 22);
            this.menuClearLog.Text = "Clear Log";
            this.menuClearLog.Click += new System.EventHandler(this.menuClearLog_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(220, 6);
            // 
            // menuExit
            // 
            this.menuExit.Name = "menuExit";
            this.menuExit.Size = new System.Drawing.Size(223, 22);
            this.menuExit.Text = "Exit";
            this.menuExit.Click += new System.EventHandler(this.menuExit_Click);
            // 
            // notifyIconServerMonitor
            // 
            this.notifyIconServerMonitor.ContextMenuStrip = this.ctxtmenuTray;
            this.notifyIconServerMonitor.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconServerMonitor.Icon")));
            this.notifyIconServerMonitor.Text = "ServerMonitor";
            this.notifyIconServerMonitor.Visible = true;
            this.notifyIconServerMonitor.DoubleClick += new System.EventHandler(this.notifyIconServerMonitor_DoubleClick);
            // 
            // ctxtmenuTray
            // 
            this.ctxtmenuTray.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxtmenuShowServerMonitor,
            this.toolStripSeparator2,
            this.ctxtmenuExit});
            this.ctxtmenuTray.Name = "ctxtmenuTray";
            this.ctxtmenuTray.Size = new System.Drawing.Size(209, 54);
            // 
            // ctxtmenuShowServerMonitor
            // 
            this.ctxtmenuShowServerMonitor.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold);
            this.ctxtmenuShowServerMonitor.Name = "ctxtmenuShowServerMonitor";
            this.ctxtmenuShowServerMonitor.Size = new System.Drawing.Size(208, 22);
            this.ctxtmenuShowServerMonitor.Text = "Show ServerMonitor";
            this.ctxtmenuShowServerMonitor.Click += new System.EventHandler(this.ctxtmenuShowServerMonitor_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(205, 6);
            // 
            // ctxtmenuExit
            // 
            this.ctxtmenuExit.Name = "ctxtmenuExit";
            this.ctxtmenuExit.Size = new System.Drawing.Size(208, 22);
            this.ctxtmenuExit.Text = "Exit";
            this.ctxtmenuExit.Click += new System.EventHandler(this.ctxtmenuExit_Click);
            // 
            // statusServerMonitor
            // 
            this.statusServerMonitor.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.labelConnCount});
            this.statusServerMonitor.Location = new System.Drawing.Point(0, 301);
            this.statusServerMonitor.Name = "statusServerMonitor";
            this.statusServerMonitor.Size = new System.Drawing.Size(854, 22);
            this.statusServerMonitor.TabIndex = 3;
            this.statusServerMonitor.Text = "statusServerMonitor";
            // 
            // labelConnCount
            // 
            this.labelConnCount.Name = "labelConnCount";
            this.labelConnCount.Size = new System.Drawing.Size(95, 17);
            this.labelConnCount.Text = "labelConnCount";
            // 
            // timerUpdate
            // 
            this.timerUpdate.Enabled = true;
            this.timerUpdate.Tick += new System.EventHandler(this.timerUpdate_Tick);
            // 
            // FormServerMonitor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(854, 323);
            this.Controls.Add(this.statusServerMonitor);
            this.Controls.Add(this.panelMainView);
            this.Controls.Add(this.menuServerMonitor);
            this.Controls.Add(this.panelControlButtons);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormServerMonitor";
            this.Text = "ServerMonitor";
            this.Deactivate += new System.EventHandler(this.MainForm_Deactivate);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.panelMainView.ResumeLayout(false);
            this.splitServerListnInfo.Panel1.ResumeLayout(false);
            this.splitServerListnInfo.Panel2.ResumeLayout(false);
            this.splitServerListnInfo.ResumeLayout(false);
            this.panelControlButtons.ResumeLayout(false);
            this.menuServerMonitor.ResumeLayout(false);
            this.menuServerMonitor.PerformLayout();
            this.ctxtmenuTray.ResumeLayout(false);
            this.statusServerMonitor.ResumeLayout(false);
            this.statusServerMonitor.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panelMainView;
        private System.Windows.Forms.SplitContainer splitServerListnInfo;
        private System.Windows.Forms.Panel panelControlButtons;
        private System.Windows.Forms.MenuStrip menuServerMonitor;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuSettings;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem menuExit;
        private System.Windows.Forms.Button btnTerminate;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.NotifyIcon notifyIconServerMonitor;
        private System.Windows.Forms.ContextMenuStrip ctxtmenuTray;
        private System.Windows.Forms.ToolStripMenuItem ctxtmenuShowServerMonitor;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem ctxtmenuExit;
        private System.Windows.Forms.ListView lviewServerList;
        private System.Windows.Forms.ColumnHeader colServerName;
        private System.Windows.Forms.ColumnHeader colProcessState;
        private System.Windows.Forms.StatusStrip statusServerMonitor;
        private System.Windows.Forms.ListBox listBoxMonitoringLog;
        private System.Windows.Forms.Timer timerUpdate;
        private System.Windows.Forms.ToolStripStatusLabel labelConnCount;
        private System.Windows.Forms.ColumnHeader colWorldID;
        private System.Windows.Forms.ColumnHeader colServerID;
        private System.Windows.Forms.ColumnHeader colLatestStartTime;
        private System.Windows.Forms.Button btnShowDetail;
        private System.Windows.Forms.ColumnHeader colServerType;
        private System.Windows.Forms.ToolStripMenuItem reloadAllServersINIFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuClearLog;
        private System.Windows.Forms.ColumnHeader colPlayerNum;
        private System.Windows.Forms.ColumnHeader colRevive;
        private System.Windows.Forms.ColumnHeader colServable;
    }
}

