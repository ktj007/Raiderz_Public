namespace ServerMonitorClient
{
    partial class FormServerMonitorClient
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormServerMonitorClient));
            this.btnStart = new System.Windows.Forms.Button();
            this.btnTerminate = new System.Windows.Forms.Button();
            this.panelControlButtons = new System.Windows.Forms.Panel();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.btnToggleRevivable = new System.Windows.Forms.Button();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuReload = new System.Windows.Forms.ToolStripMenuItem();
            this.menuClearLog = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.menuServerMonitorClient = new System.Windows.Forms.MenuStrip();
            this.ctxtmenuShowServerMonitorClient = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxtmenuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxtmenuTray = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.notifyIconServerMonitorClient = new System.Windows.Forms.NotifyIcon(this.components);
            this.splitServerTreeInfo = new System.Windows.Forms.SplitContainer();
            this.tviewServerList = new System.Windows.Forms.TreeView();
            this.imagelistTreeIcon = new System.Windows.Forms.ImageList(this.components);
            this.listboxMonitoringLog = new System.Windows.Forms.ListBox();
            this.textboxSelectedNodeInfo = new System.Windows.Forms.TextBox();
            this.timerMainUpdate = new System.Windows.Forms.Timer(this.components);
            this.notifyIconFine = new System.Windows.Forms.NotifyIcon(this.components);
            this.notifyIconBad = new System.Windows.Forms.NotifyIcon(this.components);
            this.notifyIconChanging = new System.Windows.Forms.NotifyIcon(this.components);
            this.menuShowBalloonToolTip = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxtmenuShowBalloonToolTip = new System.Windows.Forms.ToolStripMenuItem();
            this.panelControlButtons.SuspendLayout();
            this.menuServerMonitorClient.SuspendLayout();
            this.ctxtmenuTray.SuspendLayout();
            this.splitServerTreeInfo.Panel1.SuspendLayout();
            this.splitServerTreeInfo.Panel2.SuspendLayout();
            this.splitServerTreeInfo.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnStart
            // 
            this.btnStart.Enabled = false;
            this.btnStart.Location = new System.Drawing.Point(12, 9);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(110, 29);
            this.btnStart.TabIndex = 0;
            this.btnStart.Text = "Start (&S)";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnTerminate
            // 
            this.btnTerminate.Enabled = false;
            this.btnTerminate.Location = new System.Drawing.Point(128, 9);
            this.btnTerminate.Name = "btnTerminate";
            this.btnTerminate.Size = new System.Drawing.Size(110, 29);
            this.btnTerminate.TabIndex = 2;
            this.btnTerminate.Text = "Terminate (&D)";
            this.btnTerminate.UseVisualStyleBackColor = true;
            this.btnTerminate.Click += new System.EventHandler(this.btnTerminate_Click);
            // 
            // panelControlButtons
            // 
            this.panelControlButtons.Controls.Add(this.btnRefresh);
            this.panelControlButtons.Controls.Add(this.btnTerminate);
            this.panelControlButtons.Controls.Add(this.btnToggleRevivable);
            this.panelControlButtons.Controls.Add(this.btnStart);
            this.panelControlButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panelControlButtons.Location = new System.Drawing.Point(0, 269);
            this.panelControlButtons.Name = "panelControlButtons";
            this.panelControlButtons.Size = new System.Drawing.Size(892, 47);
            this.panelControlButtons.TabIndex = 4;
            // 
            // btnRefresh
            // 
            this.btnRefresh.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRefresh.Location = new System.Drawing.Point(770, 9);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(110, 29);
            this.btnRefresh.TabIndex = 3;
            this.btnRefresh.Text = "Refresh (&R)";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // btnToggleRevivable
            // 
            this.btnToggleRevivable.Enabled = false;
            this.btnToggleRevivable.Location = new System.Drawing.Point(244, 9);
            this.btnToggleRevivable.Name = "btnToggleRevivable";
            this.btnToggleRevivable.Size = new System.Drawing.Size(135, 29);
            this.btnToggleRevivable.TabIndex = 1;
            this.btnToggleRevivable.Text = "Toggle Revivable (&T)";
            this.btnToggleRevivable.UseVisualStyleBackColor = true;
            this.btnToggleRevivable.Click += new System.EventHandler(this.btnToggleRevivable_Click);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuReload,
            this.menuClearLog,
            this.menuShowBalloonToolTip,
            this.toolStripSeparator1,
            this.menuExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.ShortcutKeyDisplayString = "C";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(60, 20);
            this.fileToolStripMenuItem.Text = "동작(&C)";
            // 
            // menuReload
            // 
            this.menuReload.Name = "menuReload";
            this.menuReload.ShortcutKeyDisplayString = "";
            this.menuReload.Size = new System.Drawing.Size(223, 22);
            this.menuReload.Text = "Reconnect And Reload(&R)";
            this.menuReload.Click += new System.EventHandler(this.menuReloadReconnect_Click);
            // 
            // menuClearLog
            // 
            this.menuClearLog.Name = "menuClearLog";
            this.menuClearLog.ShortcutKeyDisplayString = "";
            this.menuClearLog.Size = new System.Drawing.Size(223, 22);
            this.menuClearLog.Text = "Clear Log(&C)";
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
            this.menuExit.Text = "Exit(&X)";
            this.menuExit.Click += new System.EventHandler(this.menuExit_Click);
            // 
            // menuServerMonitorClient
            // 
            this.menuServerMonitorClient.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuServerMonitorClient.Location = new System.Drawing.Point(0, 0);
            this.menuServerMonitorClient.Name = "menuServerMonitorClient";
            this.menuServerMonitorClient.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.menuServerMonitorClient.Size = new System.Drawing.Size(892, 24);
            this.menuServerMonitorClient.TabIndex = 5;
            this.menuServerMonitorClient.Text = "menuServerKeepserClient";
            // 
            // ctxtmenuShowServerMonitorClient
            // 
            this.ctxtmenuShowServerMonitorClient.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold);
            this.ctxtmenuShowServerMonitorClient.Name = "ctxtmenuShowServerMonitorClient";
            this.ctxtmenuShowServerMonitorClient.Size = new System.Drawing.Size(246, 22);
            this.ctxtmenuShowServerMonitorClient.Text = "Show ServerMonitorClient";
            this.ctxtmenuShowServerMonitorClient.Click += new System.EventHandler(this.ctxtmenuShowServerMonitorClient_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(243, 6);
            // 
            // ctxtmenuExit
            // 
            this.ctxtmenuExit.Name = "ctxtmenuExit";
            this.ctxtmenuExit.Size = new System.Drawing.Size(246, 22);
            this.ctxtmenuExit.Text = "Exit";
            this.ctxtmenuExit.Click += new System.EventHandler(this.ctxtmenuExit_Click);
            // 
            // ctxtmenuTray
            // 
            this.ctxtmenuTray.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxtmenuShowServerMonitorClient,
            this.ctxtmenuShowBalloonToolTip,
            this.toolStripSeparator2,
            this.ctxtmenuExit});
            this.ctxtmenuTray.Name = "ctxtmenuTray";
            this.ctxtmenuTray.Size = new System.Drawing.Size(247, 76);
            // 
            // notifyIconServerMonitorClient
            // 
            this.notifyIconServerMonitorClient.ContextMenuStrip = this.ctxtmenuTray;
            this.notifyIconServerMonitorClient.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconServerMonitorClient.Icon")));
            this.notifyIconServerMonitorClient.Text = "ServerMonitorClient";
            this.notifyIconServerMonitorClient.Visible = true;
            this.notifyIconServerMonitorClient.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIconServerMonitorClient_MouseDoubleClick);
            // 
            // splitServerTreeInfo
            // 
            this.splitServerTreeInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitServerTreeInfo.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitServerTreeInfo.Location = new System.Drawing.Point(0, 24);
            this.splitServerTreeInfo.Name = "splitServerTreeInfo";
            // 
            // splitServerTreeInfo.Panel1
            // 
            this.splitServerTreeInfo.Panel1.Controls.Add(this.tviewServerList);
            // 
            // splitServerTreeInfo.Panel2
            // 
            this.splitServerTreeInfo.Panel2.Controls.Add(this.listboxMonitoringLog);
            this.splitServerTreeInfo.Panel2.Controls.Add(this.textboxSelectedNodeInfo);
            this.splitServerTreeInfo.Size = new System.Drawing.Size(892, 245);
            this.splitServerTreeInfo.SplitterDistance = 248;
            this.splitServerTreeInfo.TabIndex = 6;
            // 
            // tviewServerList
            // 
            this.tviewServerList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tviewServerList.HideSelection = false;
            this.tviewServerList.ImageIndex = 0;
            this.tviewServerList.ImageList = this.imagelistTreeIcon;
            this.tviewServerList.Location = new System.Drawing.Point(0, 0);
            this.tviewServerList.Name = "tviewServerList";
            this.tviewServerList.SelectedImageIndex = 0;
            this.tviewServerList.Size = new System.Drawing.Size(248, 245);
            this.tviewServerList.TabIndex = 0;
            this.tviewServerList.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tviewServerList_AfterSelect);
            // 
            // imagelistTreeIcon
            // 
            this.imagelistTreeIcon.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imagelistTreeIcon.ImageStream")));
            this.imagelistTreeIcon.TransparentColor = System.Drawing.Color.Transparent;
            this.imagelistTreeIcon.Images.SetKeyName(0, "Machine.ico");
            this.imagelistTreeIcon.Images.SetKeyName(1, "Machine_Disconn.ico");
            this.imagelistTreeIcon.Images.SetKeyName(2, "Server_NotDetected.ico");
            this.imagelistTreeIcon.Images.SetKeyName(3, "Server_ProcessChange.ico");
            this.imagelistTreeIcon.Images.SetKeyName(4, "Server_Bad.ico");
            this.imagelistTreeIcon.Images.SetKeyName(5, "Server_Fine.ico");
            this.imagelistTreeIcon.Images.SetKeyName(6, "Server_Fine_NotRevive.ico");
            this.imagelistTreeIcon.Images.SetKeyName(7, "Server_Inactive.ico");
            this.imagelistTreeIcon.Images.SetKeyName(8, "Server_Error.ico");
            // 
            // listboxMonitoringLog
            // 
            this.listboxMonitoringLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listboxMonitoringLog.FormattingEnabled = true;
            this.listboxMonitoringLog.HorizontalScrollbar = true;
            this.listboxMonitoringLog.ItemHeight = 12;
            this.listboxMonitoringLog.Location = new System.Drawing.Point(0, 108);
            this.listboxMonitoringLog.Name = "listboxMonitoringLog";
            this.listboxMonitoringLog.Size = new System.Drawing.Size(640, 136);
            this.listboxMonitoringLog.TabIndex = 1;
            // 
            // textboxSelectedNodeInfo
            // 
            this.textboxSelectedNodeInfo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textboxSelectedNodeInfo.Location = new System.Drawing.Point(0, 0);
            this.textboxSelectedNodeInfo.Multiline = true;
            this.textboxSelectedNodeInfo.Name = "textboxSelectedNodeInfo";
            this.textboxSelectedNodeInfo.ReadOnly = true;
            this.textboxSelectedNodeInfo.Size = new System.Drawing.Size(640, 102);
            this.textboxSelectedNodeInfo.TabIndex = 0;
            // 
            // timerMainUpdate
            // 
            this.timerMainUpdate.Enabled = true;
            this.timerMainUpdate.Interval = 50;
            this.timerMainUpdate.Tick += new System.EventHandler(this.timerMainUpdate_Tick);
            // 
            // notifyIconFine
            // 
            this.notifyIconFine.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconFine.Icon")));
            this.notifyIconFine.Text = "ServerMonitor : Fine";
            // 
            // notifyIconBad
            // 
            this.notifyIconBad.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconBad.Icon")));
            this.notifyIconBad.Text = "ServerMonitor : Bad";
            // 
            // notifyIconChanging
            // 
            this.notifyIconChanging.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconChanging.Icon")));
            this.notifyIconChanging.Text = "ServerMonitor : Changing";
            // 
            // menuShowBalloonToolTip
            // 
            this.menuShowBalloonToolTip.Name = "menuShowBalloonToolTip";
            this.menuShowBalloonToolTip.Size = new System.Drawing.Size(223, 22);
            this.menuShowBalloonToolTip.Text = "Show BalloonToolTip";
            this.menuShowBalloonToolTip.Click += new System.EventHandler(this.menuShowBalloonToolTip_Click);
            // 
            // ctxtmenuShowBalloonToolTip
            // 
            this.ctxtmenuShowBalloonToolTip.Name = "ctxtmenuShowBalloonToolTip";
            this.ctxtmenuShowBalloonToolTip.Size = new System.Drawing.Size(246, 22);
            this.ctxtmenuShowBalloonToolTip.Text = "ShowBalloonToolTip";
            this.ctxtmenuShowBalloonToolTip.Click += new System.EventHandler(this.ctxtmenuShowBalloonToolTip_Click);
            // 
            // FormServerMonitorClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(892, 316);
            this.Controls.Add(this.splitServerTreeInfo);
            this.Controls.Add(this.menuServerMonitorClient);
            this.Controls.Add(this.panelControlButtons);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormServerMonitorClient";
            this.Text = "ServerMonitor Client";
            this.Deactivate += new System.EventHandler(this.MainForm_Deactivate);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.panelControlButtons.ResumeLayout(false);
            this.menuServerMonitorClient.ResumeLayout(false);
            this.menuServerMonitorClient.PerformLayout();
            this.ctxtmenuTray.ResumeLayout(false);
            this.splitServerTreeInfo.Panel1.ResumeLayout(false);
            this.splitServerTreeInfo.Panel2.ResumeLayout(false);
            this.splitServerTreeInfo.Panel2.PerformLayout();
            this.splitServerTreeInfo.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnTerminate;
        private System.Windows.Forms.Panel panelControlButtons;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuReload;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem menuExit;
        private System.Windows.Forms.MenuStrip menuServerMonitorClient;
        private System.Windows.Forms.ToolStripMenuItem ctxtmenuShowServerMonitorClient;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem ctxtmenuExit;
        private System.Windows.Forms.ContextMenuStrip ctxtmenuTray;
        private System.Windows.Forms.NotifyIcon notifyIconServerMonitorClient;
        private System.Windows.Forms.SplitContainer splitServerTreeInfo;
        private System.Windows.Forms.TreeView tviewServerList;
        private System.Windows.Forms.TextBox textboxSelectedNodeInfo;
        private System.Windows.Forms.Button btnToggleRevivable;
        private System.Windows.Forms.Timer timerMainUpdate;
        private System.Windows.Forms.ImageList imagelistTreeIcon;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.ListBox listboxMonitoringLog;
        private System.Windows.Forms.ToolStripMenuItem menuClearLog;
        private System.Windows.Forms.NotifyIcon notifyIconFine;
        private System.Windows.Forms.NotifyIcon notifyIconBad;
        private System.Windows.Forms.NotifyIcon notifyIconChanging;
        private System.Windows.Forms.ToolStripMenuItem menuShowBalloonToolTip;
        private System.Windows.Forms.ToolStripMenuItem ctxtmenuShowBalloonToolTip;
    }
}

