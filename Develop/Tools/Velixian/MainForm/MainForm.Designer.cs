namespace Velixian
{
    partial class MainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            WeifenLuo.WinFormsUI.Docking.DockPanelSkin dockPanelSkin1 = new WeifenLuo.WinFormsUI.Docking.DockPanelSkin();
            WeifenLuo.WinFormsUI.Docking.AutoHideStripSkin autoHideStripSkin1 = new WeifenLuo.WinFormsUI.Docking.AutoHideStripSkin();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient1 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient1 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripSkin dockPaneStripSkin1 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripSkin();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripGradient dockPaneStripGradient1 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient2 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient2 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient3 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripToolWindowGradient dockPaneStripToolWindowGradient1 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripToolWindowGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient4 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient5 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient3 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient6 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient7 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            this.fsWatcher = new System.IO.FileSystemWatcher();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
            this.BottomToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.TopToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.RightToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.LeftToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.ContentPanel = new System.Windows.Forms.ToolStripContentPanel();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.mnuMain = new System.Windows.Forms.MenuStrip();
            this.mnuFile = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuFileSaveAll = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuItemDroperSave = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuItemQuestRewarder = new System.Windows.Forms.ToolStripMenuItem();
            this.menuNPCSpawnInfo = new System.Windows.Forms.ToolStripMenuItem();
            this.menuRecipeByNPC = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuReloadXml = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuSetPath = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuOpenServerSystemFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuOpenClientSystemFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuOpenBackupFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuFileExit = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEdit = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEditFind = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuNPCView = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewMonster = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewNPC = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewObject = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuViewHideNoninteract = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewHideNoLoot = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuTool = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRunServer = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRunClient = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuSVNUpdate = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.spellCheck = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuExportLootReport = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuExportNPCShopReport = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuOption = new System.Windows.Forms.ToolStripMenuItem();
            this.언어ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuko_KR = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuen_US = new System.Windows.Forms.ToolStripMenuItem();
            this.mnude_DE = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuWindow = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_NPC = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_Quest = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_Condition = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_ItemCondition = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_Item = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_Looting = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_Dialog = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuViewEditForm_String = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuClearDockConfig = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuLockDock = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuHelp = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuHelpAbout = new System.Windows.Forms.ToolStripMenuItem();
            this.tsMenu = new System.Windows.Forms.ToolStrip();
            this.tsbtnSaveAll = new System.Windows.Forms.ToolStripButton();
            this.tsFinder = new System.Windows.Forms.ToolStrip();
            this.cbFindType = new System.Windows.Forms.ToolStripComboBox();
            this.cbFindString = new System.Windows.Forms.ToolStripComboBox();
            this.cbFindIDOnly = new System.Windows.Forms.ToolStripComboBox();
            this.cbStrict = new System.Windows.Forms.ToolStripComboBox();
            this.ilMain = new System.Windows.Forms.ImageList(this.components);
            this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader7 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader8 = new System.Windows.Forms.ColumnHeader();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.sbMain = new System.Windows.Forms.StatusStrip();
            this.staListViewIndex = new System.Windows.Forms.ToolStripStatusLabel();
            this.staMain = new System.Windows.Forms.ToolStripStatusLabel();
            this.staPath = new System.Windows.Forms.ToolStripStatusLabel();
            this.dockPanel = new WeifenLuo.WinFormsUI.Docking.DockPanel();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.fsWatcher)).BeginInit();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.mnuMain.SuspendLayout();
            this.tsMenu.SuspendLayout();
            this.tsFinder.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.sbMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // fsWatcher
            // 
            this.fsWatcher.EnableRaisingEvents = true;
            this.fsWatcher.Filter = "*.xml";
            this.fsWatcher.IncludeSubdirectories = true;
            this.fsWatcher.NotifyFilter = System.IO.NotifyFilters.LastWrite;
            this.fsWatcher.SynchronizingObject = this;
            this.fsWatcher.Changed += new System.IO.FileSystemEventHandler(this.MonitoredFileChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "ID";
            this.columnHeader1.Width = 119;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "조건";
            this.columnHeader2.Width = 384;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "주석";
            // 
            // BottomToolStripPanel
            // 
            this.BottomToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.BottomToolStripPanel.Name = "BottomToolStripPanel";
            this.BottomToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.BottomToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.BottomToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // TopToolStripPanel
            // 
            this.TopToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.TopToolStripPanel.Name = "TopToolStripPanel";
            this.TopToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.TopToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.TopToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // RightToolStripPanel
            // 
            this.RightToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.RightToolStripPanel.Name = "RightToolStripPanel";
            this.RightToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.RightToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.RightToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // LeftToolStripPanel
            // 
            this.LeftToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.LeftToolStripPanel.Name = "LeftToolStripPanel";
            this.LeftToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.LeftToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.LeftToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // ContentPanel
            // 
            this.ContentPanel.AutoScroll = true;
            this.ContentPanel.Size = new System.Drawing.Size(873, 381);
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(743, 16);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Top;
            this.toolStripContainer1.LeftToolStripPanelVisible = false;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.RightToolStripPanelVisible = false;
            this.toolStripContainer1.Size = new System.Drawing.Size(743, 65);
            this.toolStripContainer1.TabIndex = 0;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.mnuMain);
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.tsMenu);
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.tsFinder);
            // 
            // mnuMain
            // 
            this.mnuMain.Dock = System.Windows.Forms.DockStyle.None;
            this.mnuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuFile,
            this.mnuEdit,
            this.mnuNPCView,
            this.mnuTool,
            this.언어ToolStripMenuItem,
            this.mnuWindow,
            this.mnuHelp});
            this.mnuMain.Location = new System.Drawing.Point(0, 0);
            this.mnuMain.Name = "mnuMain";
            this.mnuMain.Size = new System.Drawing.Size(743, 24);
            this.mnuMain.TabIndex = 0;
            this.mnuMain.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.mnuMain_ItemClicked);
            // 
            // mnuFile
            // 
            this.mnuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuFileSaveAll,
            this.toolStripSeparator5,
            this.mnuItemDroperSave,
            this.mnuItemQuestRewarder,
            this.menuNPCSpawnInfo,
            this.menuRecipeByNPC,
            this.toolStripMenuItem1,
            this.mnuReloadXml,
            this.mnuSetPath,
            this.toolStripMenuItem3,
            this.mnuOpenServerSystemFolder,
            this.mnuOpenClientSystemFolder,
            this.mnuOpenBackupFolder,
            this.toolStripSeparator1,
            this.mnuFileExit});
            this.mnuFile.Name = "mnuFile";
            this.mnuFile.Size = new System.Drawing.Size(54, 20);
            this.mnuFile.Text = "File (&F)";
            // 
            // mnuFileSaveAll
            // 
            this.mnuFileSaveAll.Name = "mnuFileSaveAll";
            this.mnuFileSaveAll.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.mnuFileSaveAll.Size = new System.Drawing.Size(227, 22);
            this.mnuFileSaveAll.Text = "Save All (&S)";
            this.mnuFileSaveAll.Click += new System.EventHandler(this.mnuFileSaveAll_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(224, 6);
            // 
            // mnuItemDroperSave
            // 
            this.mnuItemDroperSave.Name = "mnuItemDroperSave";
            this.mnuItemDroperSave.Size = new System.Drawing.Size(227, 22);
            this.mnuItemDroperSave.Text = "Save item_droper.xml";
            this.mnuItemDroperSave.Click += new System.EventHandler(this.mnuItemDroperSave_Click);
            // 
            // mnuItemQuestRewarder
            // 
            this.mnuItemQuestRewarder.Name = "mnuItemQuestRewarder";
            this.mnuItemQuestRewarder.Size = new System.Drawing.Size(227, 22);
            this.mnuItemQuestRewarder.Text = "Save item_questrewarder.xml";
            this.mnuItemQuestRewarder.Click += new System.EventHandler(this.mnuItemQuestRewarder_Click);
            // 
            // menuNPCSpawnInfo
            // 
            this.menuNPCSpawnInfo.Name = "menuNPCSpawnInfo";
            this.menuNPCSpawnInfo.Size = new System.Drawing.Size(227, 22);
            this.menuNPCSpawnInfo.Text = "Save NPC Spawn info";
            this.menuNPCSpawnInfo.Click += new System.EventHandler(this.menuNPCSpawnInfo_Click);
            // 
            // menuRecipeByNPC
            // 
            this.menuRecipeByNPC.Name = "menuRecipeByNPC";
            this.menuRecipeByNPC.Size = new System.Drawing.Size(227, 22);
            this.menuRecipeByNPC.Text = "Save NPC Recipe Info";
            this.menuRecipeByNPC.Click += new System.EventHandler(this.menuRecipeByNPC_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(224, 6);
            // 
            // mnuReloadXml
            // 
            this.mnuReloadXml.Name = "mnuReloadXml";
            this.mnuReloadXml.ShortcutKeys = System.Windows.Forms.Keys.F5;
            this.mnuReloadXml.Size = new System.Drawing.Size(227, 22);
            this.mnuReloadXml.Text = "Reload XML Files";
            this.mnuReloadXml.Click += new System.EventHandler(this.mnuReloadXml_Click);
            // 
            // mnuSetPath
            // 
            this.mnuSetPath.Name = "mnuSetPath";
            this.mnuSetPath.Size = new System.Drawing.Size(227, 22);
            this.mnuSetPath.Text = "Path Options(&O)";
            this.mnuSetPath.Click += new System.EventHandler(this.mnuSetPath_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(224, 6);
            // 
            // mnuOpenServerSystemFolder
            // 
            this.mnuOpenServerSystemFolder.Name = "mnuOpenServerSystemFolder";
            this.mnuOpenServerSystemFolder.Size = new System.Drawing.Size(227, 22);
            this.mnuOpenServerSystemFolder.Text = "Open server system folder";
            this.mnuOpenServerSystemFolder.Click += new System.EventHandler(this.mnuOpenServerSystemFolder_Click);
            // 
            // mnuOpenClientSystemFolder
            // 
            this.mnuOpenClientSystemFolder.Name = "mnuOpenClientSystemFolder";
            this.mnuOpenClientSystemFolder.Size = new System.Drawing.Size(227, 22);
            this.mnuOpenClientSystemFolder.Text = "Open client system folder";
            this.mnuOpenClientSystemFolder.Click += new System.EventHandler(this.mnuOpenClientSystemFolder_Click);
            // 
            // mnuOpenBackupFolder
            // 
            this.mnuOpenBackupFolder.Name = "mnuOpenBackupFolder";
            this.mnuOpenBackupFolder.Size = new System.Drawing.Size(227, 22);
            this.mnuOpenBackupFolder.Text = "Open backup folder";
            this.mnuOpenBackupFolder.Click += new System.EventHandler(this.mnuOpenBackupFolder_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(224, 6);
            // 
            // mnuFileExit
            // 
            this.mnuFileExit.Name = "mnuFileExit";
            this.mnuFileExit.Size = new System.Drawing.Size(227, 22);
            this.mnuFileExit.Text = "Exit (&X)";
            this.mnuFileExit.Click += new System.EventHandler(this.mnuFileExit_Click);
            // 
            // mnuEdit
            // 
            this.mnuEdit.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuEditFind});
            this.mnuEdit.Name = "mnuEdit";
            this.mnuEdit.Size = new System.Drawing.Size(56, 20);
            this.mnuEdit.Text = "Edit (&E)";
            // 
            // mnuEditFind
            // 
            this.mnuEditFind.Name = "mnuEditFind";
            this.mnuEditFind.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F)));
            this.mnuEditFind.Size = new System.Drawing.Size(154, 22);
            this.mnuEditFind.Text = "Find (&F)";
            this.mnuEditFind.Click += new System.EventHandler(this.mnuEditFind_Click);
            // 
            // mnuNPCView
            // 
            this.mnuNPCView.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuViewMonster,
            this.mnuViewNPC,
            this.mnuViewObject,
            this.toolStripSeparator2,
            this.mnuViewHideNoninteract,
            this.mnuViewHideNoLoot});
            this.mnuNPCView.Name = "mnuNPCView";
            this.mnuNPCView.Size = new System.Drawing.Size(62, 20);
            this.mnuNPCView.Text = "View (&V)";
            // 
            // mnuViewMonster
            // 
            this.mnuViewMonster.Checked = true;
            this.mnuViewMonster.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mnuViewMonster.Name = "mnuViewMonster";
            this.mnuViewMonster.Size = new System.Drawing.Size(224, 22);
            this.mnuViewMonster.Text = "Monsters";
            this.mnuViewMonster.Click += new System.EventHandler(this.mnuViewMonster_Click);
            // 
            // mnuViewNPC
            // 
            this.mnuViewNPC.Checked = true;
            this.mnuViewNPC.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mnuViewNPC.Name = "mnuViewNPC";
            this.mnuViewNPC.Size = new System.Drawing.Size(224, 22);
            this.mnuViewNPC.Text = "Noncombat NPCs (Critters?)";
            this.mnuViewNPC.Click += new System.EventHandler(this.mnuViewNPC_Click);
            // 
            // mnuViewObject
            // 
            this.mnuViewObject.Checked = true;
            this.mnuViewObject.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mnuViewObject.Name = "mnuViewObject";
            this.mnuViewObject.Size = new System.Drawing.Size(224, 22);
            this.mnuViewObject.Text = "Objects";
            this.mnuViewObject.Click += new System.EventHandler(this.mnuViewObject_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(221, 6);
            // 
            // mnuViewHideNoninteract
            // 
            this.mnuViewHideNoninteract.Name = "mnuViewHideNoninteract";
            this.mnuViewHideNoninteract.Size = new System.Drawing.Size(224, 22);
            this.mnuViewHideNoninteract.Text = "Hide noninteracting NPCs";
            this.mnuViewHideNoninteract.Click += new System.EventHandler(this.mnuViewHideNoninteract_Click);
            // 
            // mnuViewHideNoLoot
            // 
            this.mnuViewHideNoLoot.Name = "mnuViewHideNoLoot";
            this.mnuViewHideNoLoot.Size = new System.Drawing.Size(224, 22);
            this.mnuViewHideNoLoot.Text = "Hide nonrouted NPCs";
            this.mnuViewHideNoLoot.Click += new System.EventHandler(this.mnuViewHideNoLoot_Click);
            // 
            // mnuTool
            // 
            this.mnuTool.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuRunServer,
            this.mnuRunClient,
            this.toolStripSeparator4,
            this.mnuSVNUpdate,
            this.toolStripSeparator3,
            this.spellCheck,
            this.toolStripMenuItem4,
            this.mnuExportLootReport,
            this.mnuExportNPCShopReport,
            this.toolStripMenuItem2,
            this.mnuOption});
            this.mnuTool.Name = "mnuTool";
            this.mnuTool.Size = new System.Drawing.Size(66, 20);
            this.mnuTool.Text = "Tools (&T)";
            // 
            // mnuRunServer
            // 
            this.mnuRunServer.Name = "mnuRunServer";
            this.mnuRunServer.Size = new System.Drawing.Size(270, 22);
            this.mnuRunServer.Text = "Run Server (&S)";
            this.mnuRunServer.Click += new System.EventHandler(this.mnuRunServer_Click);
            // 
            // mnuRunClient
            // 
            this.mnuRunClient.Name = "mnuRunClient";
            this.mnuRunClient.Size = new System.Drawing.Size(270, 22);
            this.mnuRunClient.Text = "Run Client (&C)";
            this.mnuRunClient.Click += new System.EventHandler(this.mnuRunClient_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(267, 6);
            // 
            // mnuSVNUpdate
            // 
            this.mnuSVNUpdate.Name = "mnuSVNUpdate";
            this.mnuSVNUpdate.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.U)));
            this.mnuSVNUpdate.Size = new System.Drawing.Size(270, 22);
            this.mnuSVNUpdate.Text = "Update SVN (&U)";
            this.mnuSVNUpdate.Click += new System.EventHandler(this.mnuSVNUpdate_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(267, 6);
            // 
            // spellCheck
            // 
            this.spellCheck.Name = "spellCheck";
            this.spellCheck.ShortcutKeys = System.Windows.Forms.Keys.F9;
            this.spellCheck.Size = new System.Drawing.Size(270, 22);
            this.spellCheck.Text = "Korean Spelling/Grammar Site (&K)";
            this.spellCheck.Click += new System.EventHandler(this.spellCheck_Click);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(267, 6);
            // 
            // mnuExportLootReport
            // 
            this.mnuExportLootReport.Name = "mnuExportLootReport";
            this.mnuExportLootReport.Size = new System.Drawing.Size(270, 22);
            this.mnuExportLootReport.Text = "전리품 Report 익스포트";
            this.mnuExportLootReport.Click += new System.EventHandler(this.mnuExportLootReport_Click);
            // 
            // mnuExportNPCShopReport
            // 
            this.mnuExportNPCShopReport.Name = "mnuExportNPCShopReport";
            this.mnuExportNPCShopReport.Size = new System.Drawing.Size(270, 22);
            this.mnuExportNPCShopReport.Text = "상점 Report 익스포트";
            this.mnuExportNPCShopReport.Click += new System.EventHandler(this.mnuExportNPCShopReport_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(267, 6);
            // 
            // mnuOption
            // 
            this.mnuOption.Name = "mnuOption";
            this.mnuOption.Size = new System.Drawing.Size(270, 22);
            this.mnuOption.Text = "Options (&O)...";
            this.mnuOption.Click += new System.EventHandler(this.mnuOption_Click);
            // 
            // 언어ToolStripMenuItem
            // 
            this.언어ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuko_KR,
            this.mnuen_US,
            this.mnude_DE});
            this.언어ToolStripMenuItem.Name = "언어ToolStripMenuItem";
            this.언어ToolStripMenuItem.Size = new System.Drawing.Size(71, 20);
            this.언어ToolStripMenuItem.Text = "Language";
            // 
            // mnuko_KR
            // 
            this.mnuko_KR.Checked = true;
            this.mnuko_KR.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mnuko_KR.Name = "mnuko_KR";
            this.mnuko_KR.Size = new System.Drawing.Size(152, 22);
            this.mnuko_KR.Text = "ko_KR";
            this.mnuko_KR.Click += new System.EventHandler(this.mnuko_KR_Click);
            // 
            // mnuen_US
            // 
            this.mnuen_US.Name = "mnuen_US";
            this.mnuen_US.Size = new System.Drawing.Size(152, 22);
            this.mnuen_US.Text = "en_US";
            this.mnuen_US.Click += new System.EventHandler(this.mnuen_US_Click);
            // 
            // mnude_DE
            // 
            this.mnude_DE.Enabled = false;
            this.mnude_DE.Name = "mnude_DE";
            this.mnude_DE.Size = new System.Drawing.Size(152, 22);
            this.mnude_DE.Text = "de_DE";
            this.mnude_DE.Click += new System.EventHandler(this.mnude_DE_Click);
            // 
            // mnuWindow
            // 
            this.mnuWindow.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuViewEditForm_NPC,
            this.mnuViewEditForm_Quest,
            this.mnuViewEditForm_Condition,
            this.mnuViewEditForm_ItemCondition,
            this.mnuViewEditForm_Item,
            this.mnuViewEditForm_Looting,
            this.mnuViewEditForm_Dialog,
            this.mnuViewEditForm_String,
            this.toolStripSeparator6,
            this.mnuClearDockConfig,
            this.mnuLockDock});
            this.mnuWindow.Name = "mnuWindow";
            this.mnuWindow.Size = new System.Drawing.Size(88, 20);
            this.mnuWindow.Text = "Windows (&H)";
            // 
            // mnuViewEditForm_NPC
            // 
            this.mnuViewEditForm_NPC.Name = "mnuViewEditForm_NPC";
            this.mnuViewEditForm_NPC.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D1)));
            this.mnuViewEditForm_NPC.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_NPC.Text = "NPCs";
            this.mnuViewEditForm_NPC.Click += new System.EventHandler(this.mnuViewEditForm_NPC_Click);
            // 
            // mnuViewEditForm_Quest
            // 
            this.mnuViewEditForm_Quest.Name = "mnuViewEditForm_Quest";
            this.mnuViewEditForm_Quest.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D2)));
            this.mnuViewEditForm_Quest.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_Quest.Text = "Quests";
            this.mnuViewEditForm_Quest.Click += new System.EventHandler(this.mnuViewEditForm_Quest_Click);
            // 
            // mnuViewEditForm_Condition
            // 
            this.mnuViewEditForm_Condition.Name = "mnuViewEditForm_Condition";
            this.mnuViewEditForm_Condition.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D3)));
            this.mnuViewEditForm_Condition.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_Condition.Text = "Conditions";
            this.mnuViewEditForm_Condition.Click += new System.EventHandler(this.mnuViewEditForm_Condition_Click);
            // 
            // mnuViewEditForm_ItemCondition
            // 
            this.mnuViewEditForm_ItemCondition.Name = "mnuViewEditForm_ItemCondition";
            this.mnuViewEditForm_ItemCondition.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D4)));
            this.mnuViewEditForm_ItemCondition.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_ItemCondition.Text = "Conditions (Quest)";
            this.mnuViewEditForm_ItemCondition.Click += new System.EventHandler(this.mnuViewEditForm_ItemCondition_Click);
            // 
            // mnuViewEditForm_Item
            // 
            this.mnuViewEditForm_Item.Name = "mnuViewEditForm_Item";
            this.mnuViewEditForm_Item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D5)));
            this.mnuViewEditForm_Item.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_Item.Text = "Items";
            this.mnuViewEditForm_Item.Click += new System.EventHandler(this.mnuViewEditForm_Item_Click);
            // 
            // mnuViewEditForm_Looting
            // 
            this.mnuViewEditForm_Looting.Name = "mnuViewEditForm_Looting";
            this.mnuViewEditForm_Looting.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D6)));
            this.mnuViewEditForm_Looting.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_Looting.Text = "Routing(?)";
            this.mnuViewEditForm_Looting.Click += new System.EventHandler(this.mnuViewEditForm_Looting_Click);
            // 
            // mnuViewEditForm_Dialog
            // 
            this.mnuViewEditForm_Dialog.Name = "mnuViewEditForm_Dialog";
            this.mnuViewEditForm_Dialog.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D7)));
            this.mnuViewEditForm_Dialog.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_Dialog.Text = "Dialogs";
            this.mnuViewEditForm_Dialog.Click += new System.EventHandler(this.mnuViewEditForm_Dialog_Click);
            // 
            // mnuViewEditForm_String
            // 
            this.mnuViewEditForm_String.Name = "mnuViewEditForm_String";
            this.mnuViewEditForm_String.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D8)));
            this.mnuViewEditForm_String.Size = new System.Drawing.Size(214, 22);
            this.mnuViewEditForm_String.Text = "Strings";
            this.mnuViewEditForm_String.Click += new System.EventHandler(this.mnuViewEditForm_String_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(211, 6);
            // 
            // mnuClearDockConfig
            // 
            this.mnuClearDockConfig.Name = "mnuClearDockConfig";
            this.mnuClearDockConfig.Size = new System.Drawing.Size(214, 22);
            this.mnuClearDockConfig.Text = "Init windows (?)";
            this.mnuClearDockConfig.Click += new System.EventHandler(this.mnuClearDockConfig_Click);
            // 
            // mnuLockDock
            // 
            this.mnuLockDock.Name = "mnuLockDock";
            this.mnuLockDock.Size = new System.Drawing.Size(214, 22);
            this.mnuLockDock.Text = "Fix windows (?)";
            this.mnuLockDock.Click += new System.EventHandler(this.mnuLockDock_Click);
            // 
            // mnuHelp
            // 
            this.mnuHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuHelpAbout});
            this.mnuHelp.Name = "mnuHelp";
            this.mnuHelp.Size = new System.Drawing.Size(64, 20);
            this.mnuHelp.Text = "Help (&H)";
            this.mnuHelp.Click += new System.EventHandler(this.mnuHelp_Click);
            // 
            // mnuHelpAbout
            // 
            this.mnuHelpAbout.Name = "mnuHelpAbout";
            this.mnuHelpAbout.Size = new System.Drawing.Size(152, 22);
            this.mnuHelpAbout.Text = "About (&A)";
            this.mnuHelpAbout.Click += new System.EventHandler(this.mnuHelpAbout_Click);
            // 
            // tsMenu
            // 
            this.tsMenu.Dock = System.Windows.Forms.DockStyle.None;
            this.tsMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsbtnSaveAll});
            this.tsMenu.Location = new System.Drawing.Point(3, 24);
            this.tsMenu.Name = "tsMenu";
            this.tsMenu.Size = new System.Drawing.Size(35, 25);
            this.tsMenu.TabIndex = 1;
            // 
            // tsbtnSaveAll
            // 
            this.tsbtnSaveAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.tsbtnSaveAll.Image = ((System.Drawing.Image)(resources.GetObject("tsbtnSaveAll.Image")));
            this.tsbtnSaveAll.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbtnSaveAll.Name = "tsbtnSaveAll";
            this.tsbtnSaveAll.Size = new System.Drawing.Size(23, 22);
            this.tsbtnSaveAll.Text = "toolStripButton1";
            this.tsbtnSaveAll.Click += new System.EventHandler(this.tsbtnSaveAll_Click);
            // 
            // tsFinder
            // 
            this.tsFinder.Dock = System.Windows.Forms.DockStyle.None;
            this.tsFinder.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cbFindType,
            this.cbFindString,
            this.cbFindIDOnly,
            this.cbStrict});
            this.tsFinder.Location = new System.Drawing.Point(38, 24);
            this.tsFinder.Name = "tsFinder";
            this.tsFinder.Size = new System.Drawing.Size(435, 25);
            this.tsFinder.TabIndex = 2;
            // 
            // cbFindType
            // 
            this.cbFindType.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.cbFindType.DropDownHeight = 160;
            this.cbFindType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbFindType.IntegralHeight = false;
            this.cbFindType.Name = "cbFindType";
            this.cbFindType.Size = new System.Drawing.Size(80, 25);
            // 
            // cbFindString
            // 
            this.cbFindString.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
            this.cbFindString.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.cbFindString.Name = "cbFindString";
            this.cbFindString.Size = new System.Drawing.Size(140, 25);
            this.cbFindString.ToolTipText = "문자열 검색시 첫 시작 문자열만 가능합니다.";
            this.cbFindString.KeyDown += new System.Windows.Forms.KeyEventHandler(this.cbFindString_KeyDown);
            // 
            // cbFindIDOnly
            // 
            this.cbFindIDOnly.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbFindIDOnly.Items.AddRange(new object[] {
            "아이디만 검색",
            "시작 문자열 검색",
            "모든 문자열 검색"});
            this.cbFindIDOnly.Name = "cbFindIDOnly";
            this.cbFindIDOnly.Size = new System.Drawing.Size(120, 25);
            this.cbFindIDOnly.SelectedIndexChanged += new System.EventHandler(this.cbFindIDOnly_SelectedIndexChanged);
            // 
            // cbStrict
            // 
            this.cbStrict.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbStrict.Items.AddRange(new object[] {
            "부분일치",
            "완전일치"});
            this.cbStrict.Name = "cbStrict";
            this.cbStrict.Size = new System.Drawing.Size(75, 25);
            // 
            // ilMain
            // 
            this.ilMain.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilMain.ImageStream")));
            this.ilMain.TransparentColor = System.Drawing.Color.Transparent;
            this.ilMain.Images.SetKeyName(0, "female.ico");
            this.ilMain.Images.SetKeyName(1, "male.ico");
            this.ilMain.Images.SetKeyName(2, "monster1.ico");
            this.ilMain.Images.SetKeyName(3, "monster2.ico");
            this.ilMain.Images.SetKeyName(4, "monster3.ico");
            this.ilMain.Images.SetKeyName(5, "npc.ico");
            this.ilMain.Images.SetKeyName(6, "ok.ico");
            this.ilMain.Images.SetKeyName(7, "notok.ico");
            this.ilMain.Images.SetKeyName(8, "object.ico");
            this.ilMain.Images.SetKeyName(9, "silver.png");
            this.ilMain.Images.SetKeyName(10, "items.png");
            this.ilMain.Images.SetKeyName(11, "soul.png");
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "ID";
            this.columnHeader4.Width = 119;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "이름";
            this.columnHeader5.Width = 214;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "설명";
            this.columnHeader6.Width = 286;
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "사용조건";
            this.columnHeader7.Width = 88;
            // 
            // columnHeader8
            // 
            this.columnHeader8.Text = "성별";
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.propertyGrid1);
            this.splitContainer2.Size = new System.Drawing.Size(331, 303);
            this.splitContainer2.SplitterDistance = 133;
            this.splitContainer2.TabIndex = 1;
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(331, 166);
            this.propertyGrid1.TabIndex = 2;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // sbMain
            // 
            this.sbMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.staListViewIndex,
            this.staMain,
            this.staPath});
            this.sbMain.Location = new System.Drawing.Point(0, 558);
            this.sbMain.Name = "sbMain";
            this.sbMain.Padding = new System.Windows.Forms.Padding(1, 0, 12, 0);
            this.sbMain.Size = new System.Drawing.Size(743, 22);
            this.sbMain.TabIndex = 7;
            // 
            // staListViewIndex
            // 
            this.staListViewIndex.Name = "staListViewIndex";
            this.staListViewIndex.Size = new System.Drawing.Size(38, 17);
            this.staListViewIndex.Text = "(0 / 0)";
            // 
            // staMain
            // 
            this.staMain.Name = "staMain";
            this.staMain.Size = new System.Drawing.Size(31, 17);
            this.staMain.Text = "완료";
            // 
            // staPath
            // 
            this.staPath.ActiveLinkColor = System.Drawing.Color.YellowGreen;
            this.staPath.IsLink = true;
            this.staPath.LinkBehavior = System.Windows.Forms.LinkBehavior.AlwaysUnderline;
            this.staPath.LinkColor = System.Drawing.Color.Green;
            this.staPath.Name = "staPath";
            this.staPath.Size = new System.Drawing.Size(661, 17);
            this.staPath.Spring = true;
            this.staPath.Text = "C:\\";
            this.staPath.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.staPath.Click += new System.EventHandler(this.mnuSetPath_Click);
            // 
            // dockPanel
            // 
            this.dockPanel.ActiveAutoHideContent = null;
            this.dockPanel.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.dockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dockPanel.DockBackColor = System.Drawing.SystemColors.AppWorkspace;
            this.dockPanel.Location = new System.Drawing.Point(0, 0);
            this.dockPanel.Name = "dockPanel";
            this.dockPanel.Size = new System.Drawing.Size(743, 580);
            dockPanelGradient1.EndColor = System.Drawing.SystemColors.ControlLight;
            dockPanelGradient1.StartColor = System.Drawing.SystemColors.ControlLight;
            autoHideStripSkin1.DockStripGradient = dockPanelGradient1;
            tabGradient1.EndColor = System.Drawing.SystemColors.Control;
            tabGradient1.StartColor = System.Drawing.SystemColors.Control;
            tabGradient1.TextColor = System.Drawing.SystemColors.ControlDarkDark;
            autoHideStripSkin1.TabGradient = tabGradient1;
            autoHideStripSkin1.TextFont = new System.Drawing.Font("Malgun Gothic", 9F);
            dockPanelSkin1.AutoHideStripSkin = autoHideStripSkin1;
            tabGradient2.EndColor = System.Drawing.SystemColors.ControlLightLight;
            tabGradient2.StartColor = System.Drawing.SystemColors.ControlLightLight;
            tabGradient2.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripGradient1.ActiveTabGradient = tabGradient2;
            dockPanelGradient2.EndColor = System.Drawing.SystemColors.Control;
            dockPanelGradient2.StartColor = System.Drawing.SystemColors.Control;
            dockPaneStripGradient1.DockStripGradient = dockPanelGradient2;
            tabGradient3.EndColor = System.Drawing.SystemColors.ControlLight;
            tabGradient3.StartColor = System.Drawing.SystemColors.ControlLight;
            tabGradient3.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripGradient1.InactiveTabGradient = tabGradient3;
            dockPaneStripSkin1.DocumentGradient = dockPaneStripGradient1;
            dockPaneStripSkin1.TextFont = new System.Drawing.Font("Malgun Gothic", 9F);
            tabGradient4.EndColor = System.Drawing.SystemColors.ActiveCaption;
            tabGradient4.LinearGradientMode = System.Drawing.Drawing2D.LinearGradientMode.Vertical;
            tabGradient4.StartColor = System.Drawing.SystemColors.GradientActiveCaption;
            tabGradient4.TextColor = System.Drawing.SystemColors.ActiveCaptionText;
            dockPaneStripToolWindowGradient1.ActiveCaptionGradient = tabGradient4;
            tabGradient5.EndColor = System.Drawing.SystemColors.Control;
            tabGradient5.StartColor = System.Drawing.SystemColors.Control;
            tabGradient5.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripToolWindowGradient1.ActiveTabGradient = tabGradient5;
            dockPanelGradient3.EndColor = System.Drawing.SystemColors.ControlLight;
            dockPanelGradient3.StartColor = System.Drawing.SystemColors.ControlLight;
            dockPaneStripToolWindowGradient1.DockStripGradient = dockPanelGradient3;
            tabGradient6.EndColor = System.Drawing.SystemColors.InactiveCaption;
            tabGradient6.LinearGradientMode = System.Drawing.Drawing2D.LinearGradientMode.Vertical;
            tabGradient6.StartColor = System.Drawing.SystemColors.GradientInactiveCaption;
            tabGradient6.TextColor = System.Drawing.SystemColors.InactiveCaptionText;
            dockPaneStripToolWindowGradient1.InactiveCaptionGradient = tabGradient6;
            tabGradient7.EndColor = System.Drawing.Color.Transparent;
            tabGradient7.StartColor = System.Drawing.Color.Transparent;
            tabGradient7.TextColor = System.Drawing.SystemColors.ControlDarkDark;
            dockPaneStripToolWindowGradient1.InactiveTabGradient = tabGradient7;
            dockPaneStripSkin1.ToolWindowGradient = dockPaneStripToolWindowGradient1;
            dockPanelSkin1.DockPaneStripSkin = dockPaneStripSkin1;
            this.dockPanel.Skin = dockPanelSkin1;
            this.dockPanel.TabIndex = 8;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(743, 580);
            this.Controls.Add(this.sbMain);
            this.Controls.Add(this.toolStripContainer1);
            this.Controls.Add(this.dockPanel);
            this.DoubleBuffered = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.mnuMain;
            this.Name = "MainForm";
            this.Text = "Velixian";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.fsWatcher)).EndInit();
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.mnuMain.ResumeLayout(false);
            this.mnuMain.PerformLayout();
            this.tsMenu.ResumeLayout(false);
            this.tsMenu.PerformLayout();
            this.tsFinder.ResumeLayout(false);
            this.tsFinder.PerformLayout();
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.sbMain.ResumeLayout(false);
            this.sbMain.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        public System.IO.FileSystemWatcher fsWatcher;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private System.Windows.Forms.MenuStrip mnuMain;
        private System.Windows.Forms.ToolStripMenuItem mnuFile;
        private System.Windows.Forms.ToolStripMenuItem mnuSetPath;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem mnuFileExit;
        private System.Windows.Forms.ToolStripMenuItem mnuEdit;
        private System.Windows.Forms.ToolStripMenuItem mnuEditFind;
        private System.Windows.Forms.ToolStripMenuItem mnuHelp;
        private System.Windows.Forms.ToolStripMenuItem mnuHelpAbout;
        private System.Windows.Forms.ToolStrip tsFinder;
        private System.Windows.Forms.ToolStripComboBox cbFindType;
        private System.Windows.Forms.ToolStripComboBox cbFindString;
        private System.Windows.Forms.ToolStripComboBox cbFindIDOnly;
        private System.Windows.Forms.ToolStripPanel BottomToolStripPanel;
        private System.Windows.Forms.ToolStripPanel TopToolStripPanel;
        private System.Windows.Forms.ToolStripPanel RightToolStripPanel;
        private System.Windows.Forms.ToolStripPanel LeftToolStripPanel;
        private System.Windows.Forms.ToolStripContentPanel ContentPanel;
        private System.Windows.Forms.ToolStripMenuItem mnuFileSaveAll;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem mnuTool;
        private System.Windows.Forms.ToolStripMenuItem spellCheck;
        private System.Windows.Forms.ImageList ilMain;
        private System.Windows.Forms.ToolStripMenuItem mnuNPCView;
        public System.Windows.Forms.ToolStripMenuItem mnuViewHideNoninteract;
        public System.Windows.Forms.ToolStripMenuItem mnuViewHideNoLoot;
        public System.Windows.Forms.ToolStripMenuItem mnuViewMonster;
        public System.Windows.Forms.ToolStripMenuItem mnuViewNPC;
        public System.Windows.Forms.ToolStripMenuItem mnuViewObject;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem mnuRunServer;
        private System.Windows.Forms.ToolStripMenuItem mnuRunClient;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem mnuSVNUpdate;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripComboBox cbStrict;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader8;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem mnuItemDroperSave;
        private System.Windows.Forms.ToolStripMenuItem mnuItemQuestRewarder;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem mnuExportLootReport;
        private System.Windows.Forms.ToolStripMenuItem menuNPCSpawnInfo;
        private System.Windows.Forms.ToolStripMenuItem menuRecipeByNPC;
        private System.Windows.Forms.ToolStripMenuItem mnuExportNPCShopReport;
        private System.Windows.Forms.StatusStrip sbMain;
        private System.Windows.Forms.ToolStripStatusLabel staMain;
        private System.Windows.Forms.ToolStripStatusLabel staPath;
        private WeifenLuo.WinFormsUI.Docking.DockPanel dockPanel;
        private System.Windows.Forms.ToolStripMenuItem mnuWindow;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_NPC;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_Quest;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_Condition;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_ItemCondition;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_Item;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_Looting;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_Dialog;
        private System.Windows.Forms.ToolStripMenuItem mnuViewEditForm_String;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripMenuItem mnuClearDockConfig;
        private System.Windows.Forms.ToolStripMenuItem mnuLockDock;
        private System.Windows.Forms.ToolStrip tsMenu;
        private System.Windows.Forms.ToolStripButton tsbtnSaveAll;
        private System.Windows.Forms.ToolStripMenuItem 언어ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuko_KR;
        private System.Windows.Forms.ToolStripMenuItem mnuen_US;
        private System.Windows.Forms.ToolStripMenuItem mnude_DE;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem mnuOption;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.ToolStripStatusLabel staListViewIndex;
        private System.Windows.Forms.ToolStripMenuItem mnuReloadXml;
        private System.Windows.Forms.ToolStripMenuItem mnuOpenBackupFolder;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem mnuOpenServerSystemFolder;
        private System.Windows.Forms.ToolStripMenuItem mnuOpenClientSystemFolder;
    }
}

