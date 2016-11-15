namespace SoundManager
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
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.treeSoundList = new System.Windows.Forms.TreeView();
            this.imageSoundType = new System.Windows.Forms.ImageList(this.components);
            this.buttonAddSound = new System.Windows.Forms.Button();
            this.imageButtons = new System.Windows.Forms.ImageList(this.components);
            this.buttonAddGroup = new System.Windows.Forms.Button();
            this.buttonRemove = new System.Windows.Forms.Button();
            this.tabSoundEnvironment = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.propertySoundInfo = new System.Windows.Forms.PropertyGrid();
            this.listSounds = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.buttonAddSubSound = new System.Windows.Forms.Button();
            this.buttonRemoveSubSound = new System.Windows.Forms.Button();
            this.splitContainer4 = new System.Windows.Forms.SplitContainer();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.buttonPositionScaleUp = new System.Windows.Forms.Button();
            this.buttonPositionScaleDown = new System.Windows.Forms.Button();
            this.comboReverb = new System.Windows.Forms.ComboBox();
            this.propertyReverb = new System.Windows.Forms.PropertyGrid();
            this.pictureSoundPositionInfo = new System.Windows.Forms.PictureBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.labelSoundControl = new System.Windows.Forms.Label();
            this.radioPanlevel = new System.Windows.Forms.RadioButton();
            this.radioLowpass = new System.Windows.Forms.RadioButton();
            this.radioReverb = new System.Windows.Forms.RadioButton();
            this.radioDirect = new System.Windows.Forms.RadioButton();
            this.radioRolloff = new System.Windows.Forms.RadioButton();
            this.pictureSoundControlInfo = new System.Windows.Forms.PictureBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.panelComponent = new System.Windows.Forms.Panel();
            this.buttonPauseComponent = new System.Windows.Forms.Button();
            this.buttonPlayComponent = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.buttonPlay = new System.Windows.Forms.Button();
            this.labelPlayTime = new System.Windows.Forms.Label();
            this.trackTimeLine = new System.Windows.Forms.TrackBar();
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.reloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.setworkdirToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.validationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.timerUpdate = new System.Windows.Forms.Timer(this.components);
            this.statusStrip.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.tabSoundEnvironment.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.splitContainer4.Panel1.SuspendLayout();
            this.splitContainer4.Panel2.SuspendLayout();
            this.splitContainer4.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureSoundPositionInfo)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureSoundControlInfo)).BeginInit();
            this.tabPage2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackTimeLine)).BeginInit();
            this.menuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel});
            this.statusStrip.Location = new System.Drawing.Point(0, 544);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(892, 22);
            this.statusStrip.TabIndex = 2;
            this.statusStrip.Text = "statusStrip1";
            // 
            // toolStripStatusLabel
            // 
            this.toolStripStatusLabel.Name = "toolStripStatusLabel";
            this.toolStripStatusLabel.Size = new System.Drawing.Size(11, 17);
            this.toolStripStatusLabel.Text = "-";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 24);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treeSoundList);
            this.splitContainer1.Panel1.Controls.Add(this.buttonAddSound);
            this.splitContainer1.Panel1.Controls.Add(this.buttonAddGroup);
            this.splitContainer1.Panel1.Controls.Add(this.buttonRemove);
            this.splitContainer1.Panel1MinSize = 250;
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tabSoundEnvironment);
            this.splitContainer1.Panel2.Controls.Add(this.buttonPlay);
            this.splitContainer1.Panel2.Controls.Add(this.labelPlayTime);
            this.splitContainer1.Panel2.Controls.Add(this.trackTimeLine);
            this.splitContainer1.Panel2MinSize = 0;
            this.splitContainer1.Size = new System.Drawing.Size(892, 520);
            this.splitContainer1.SplitterDistance = 250;
            this.splitContainer1.TabIndex = 3;
            // 
            // treeSoundList
            // 
            this.treeSoundList.AllowDrop = true;
            this.treeSoundList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.treeSoundList.FullRowSelect = true;
            this.treeSoundList.HideSelection = false;
            this.treeSoundList.Indent = 19;
            this.treeSoundList.ItemHeight = 16;
            this.treeSoundList.Location = new System.Drawing.Point(3, 3);
            this.treeSoundList.Name = "treeSoundList";
            this.treeSoundList.ShowRootLines = false;
            this.treeSoundList.Size = new System.Drawing.Size(245, 499);
            this.treeSoundList.StateImageList = this.imageSoundType;
            this.treeSoundList.TabIndex = 0;
            this.treeSoundList.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeSoundList_DragDrop);
            this.treeSoundList.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeSoundList_AfterSelect);
            this.treeSoundList.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeSoundList_DragEnter);
            this.treeSoundList.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeSoundList_ItemDrag);
            // 
            // imageSoundType
            // 
            this.imageSoundType.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageSoundType.ImageStream")));
            this.imageSoundType.Tag = "";
            this.imageSoundType.TransparentColor = System.Drawing.Color.Transparent;
            this.imageSoundType.Images.SetKeyName(0, "soundtype1.bmp");
            this.imageSoundType.Images.SetKeyName(1, "soundtype2.bmp");
            this.imageSoundType.Images.SetKeyName(2, "soundtype3.bmp");
            this.imageSoundType.Images.SetKeyName(3, "soundtype4.bmp");
            // 
            // buttonAddSound
            // 
            this.buttonAddSound.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonAddSound.FlatAppearance.BorderSize = 0;
            this.buttonAddSound.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.buttonAddSound.ImageIndex = 0;
            this.buttonAddSound.ImageList = this.imageButtons;
            this.buttonAddSound.Location = new System.Drawing.Point(192, 503);
            this.buttonAddSound.Name = "buttonAddSound";
            this.buttonAddSound.Size = new System.Drawing.Size(15, 15);
            this.buttonAddSound.TabIndex = 1;
            this.buttonAddSound.UseVisualStyleBackColor = true;
            this.buttonAddSound.Click += new System.EventHandler(this.buttonAddSound_Click);
            // 
            // imageButtons
            // 
            this.imageButtons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageButtons.ImageStream")));
            this.imageButtons.Tag = "";
            this.imageButtons.TransparentColor = System.Drawing.Color.Transparent;
            this.imageButtons.Images.SetKeyName(0, "addsound.bmp");
            this.imageButtons.Images.SetKeyName(1, "addgroup.bmp");
            this.imageButtons.Images.SetKeyName(2, "remove.bmp");
            // 
            // buttonAddGroup
            // 
            this.buttonAddGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonAddGroup.FlatAppearance.BorderSize = 0;
            this.buttonAddGroup.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.buttonAddGroup.ImageIndex = 1;
            this.buttonAddGroup.ImageList = this.imageButtons;
            this.buttonAddGroup.Location = new System.Drawing.Point(213, 503);
            this.buttonAddGroup.Name = "buttonAddGroup";
            this.buttonAddGroup.Size = new System.Drawing.Size(14, 14);
            this.buttonAddGroup.TabIndex = 3;
            this.buttonAddGroup.UseVisualStyleBackColor = true;
            this.buttonAddGroup.Click += new System.EventHandler(this.buttonAddGroup_Click);
            // 
            // buttonRemove
            // 
            this.buttonRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonRemove.FlatAppearance.BorderSize = 0;
            this.buttonRemove.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.buttonRemove.ImageIndex = 2;
            this.buttonRemove.ImageList = this.imageButtons;
            this.buttonRemove.Location = new System.Drawing.Point(233, 503);
            this.buttonRemove.Name = "buttonRemove";
            this.buttonRemove.Size = new System.Drawing.Size(14, 14);
            this.buttonRemove.TabIndex = 2;
            this.buttonRemove.Text = "-";
            this.buttonRemove.UseVisualStyleBackColor = true;
            this.buttonRemove.Click += new System.EventHandler(this.buttonRemove_Click);
            // 
            // tabSoundEnvironment
            // 
            this.tabSoundEnvironment.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabSoundEnvironment.Controls.Add(this.tabPage1);
            this.tabSoundEnvironment.Controls.Add(this.tabPage2);
            this.tabSoundEnvironment.Location = new System.Drawing.Point(3, 3);
            this.tabSoundEnvironment.Name = "tabSoundEnvironment";
            this.tabSoundEnvironment.SelectedIndex = 0;
            this.tabSoundEnvironment.Size = new System.Drawing.Size(632, 485);
            this.tabSoundEnvironment.TabIndex = 0;
            this.tabSoundEnvironment.SizeChanged += new System.EventHandler(this.tabSoundEnvironment_SizeChanged);
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.splitContainer2);
            this.tabPage1.Location = new System.Drawing.Point(4, 21);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(624, 460);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Sound Settings";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer2.Location = new System.Drawing.Point(3, 3);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.groupBox3);
            this.splitContainer2.Panel1MinSize = 250;
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer4);
            this.splitContainer2.Panel2MinSize = 0;
            this.splitContainer2.Size = new System.Drawing.Size(618, 454);
            this.splitContainer2.SplitterDistance = 250;
            this.splitContainer2.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.splitContainer3);
            this.groupBox3.Location = new System.Drawing.Point(3, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(244, 446);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Sound settings";
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(3, 17);
            this.splitContainer3.Name = "splitContainer3";
            this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.propertySoundInfo);
            this.splitContainer3.Panel1MinSize = 0;
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.listSounds);
            this.splitContainer3.Panel2.Controls.Add(this.buttonAddSubSound);
            this.splitContainer3.Panel2.Controls.Add(this.buttonRemoveSubSound);
            this.splitContainer3.Panel2MinSize = 0;
            this.splitContainer3.Size = new System.Drawing.Size(238, 426);
            this.splitContainer3.SplitterDistance = 213;
            this.splitContainer3.TabIndex = 0;
            // 
            // propertySoundInfo
            // 
            this.propertySoundInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.propertySoundInfo.Location = new System.Drawing.Point(3, 3);
            this.propertySoundInfo.Name = "propertySoundInfo";
            this.propertySoundInfo.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.propertySoundInfo.Size = new System.Drawing.Size(232, 207);
            this.propertySoundInfo.TabIndex = 0;
            this.propertySoundInfo.ToolbarVisible = false;
            this.propertySoundInfo.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertySoundInfo_PropertyValueChanged);
            // 
            // listSounds
            // 
            this.listSounds.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listSounds.AutoArrange = false;
            this.listSounds.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.listSounds.FullRowSelect = true;
            this.listSounds.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listSounds.HideSelection = false;
            this.listSounds.Location = new System.Drawing.Point(3, 3);
            this.listSounds.MultiSelect = false;
            this.listSounds.Name = "listSounds";
            this.listSounds.Size = new System.Drawing.Size(232, 188);
            this.listSounds.SmallImageList = this.imageSoundType;
            this.listSounds.TabIndex = 0;
            this.listSounds.UseCompatibleStateImageBehavior = false;
            this.listSounds.View = System.Windows.Forms.View.Details;
            this.listSounds.Resize += new System.EventHandler(this.listSounds_Resize);
            this.listSounds.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.listSounds_ItemSelectionChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Param";
            this.columnHeader1.Width = 100;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Sound";
            this.columnHeader2.Width = 125;
            // 
            // buttonAddSubSound
            // 
            this.buttonAddSubSound.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonAddSubSound.FlatAppearance.BorderSize = 0;
            this.buttonAddSubSound.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.buttonAddSubSound.ImageIndex = 0;
            this.buttonAddSubSound.ImageList = this.imageButtons;
            this.buttonAddSubSound.Location = new System.Drawing.Point(200, 192);
            this.buttonAddSubSound.Name = "buttonAddSubSound";
            this.buttonAddSubSound.Size = new System.Drawing.Size(15, 15);
            this.buttonAddSubSound.TabIndex = 4;
            this.buttonAddSubSound.UseVisualStyleBackColor = true;
            // 
            // buttonRemoveSubSound
            // 
            this.buttonRemoveSubSound.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonRemoveSubSound.FlatAppearance.BorderSize = 0;
            this.buttonRemoveSubSound.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.buttonRemoveSubSound.ImageIndex = 2;
            this.buttonRemoveSubSound.ImageList = this.imageButtons;
            this.buttonRemoveSubSound.Location = new System.Drawing.Point(221, 192);
            this.buttonRemoveSubSound.Name = "buttonRemoveSubSound";
            this.buttonRemoveSubSound.Size = new System.Drawing.Size(14, 14);
            this.buttonRemoveSubSound.TabIndex = 4;
            this.buttonRemoveSubSound.Text = "-";
            this.buttonRemoveSubSound.UseVisualStyleBackColor = true;
            // 
            // splitContainer4
            // 
            this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer4.Location = new System.Drawing.Point(0, 0);
            this.splitContainer4.Name = "splitContainer4";
            this.splitContainer4.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer4.Panel1
            // 
            this.splitContainer4.Panel1.Controls.Add(this.groupBox1);
            this.splitContainer4.Panel1MinSize = 0;
            // 
            // splitContainer4.Panel2
            // 
            this.splitContainer4.Panel2.Controls.Add(this.groupBox2);
            this.splitContainer4.Panel2MinSize = 0;
            this.splitContainer4.Size = new System.Drawing.Size(364, 454);
            this.splitContainer4.SplitterDistance = 249;
            this.splitContainer4.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.buttonPositionScaleUp);
            this.groupBox1.Controls.Add(this.buttonPositionScaleDown);
            this.groupBox1.Controls.Add(this.comboReverb);
            this.groupBox1.Controls.Add(this.propertyReverb);
            this.groupBox1.Controls.Add(this.pictureSoundPositionInfo);
            this.groupBox1.Location = new System.Drawing.Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(358, 241);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "3D Environment";
            // 
            // buttonPositionScaleUp
            // 
            this.buttonPositionScaleUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonPositionScaleUp.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.buttonPositionScaleUp.Location = new System.Drawing.Point(313, 215);
            this.buttonPositionScaleUp.Margin = new System.Windows.Forms.Padding(0);
            this.buttonPositionScaleUp.Name = "buttonPositionScaleUp";
            this.buttonPositionScaleUp.Size = new System.Drawing.Size(16, 16);
            this.buttonPositionScaleUp.TabIndex = 7;
            this.buttonPositionScaleUp.Text = "+";
            this.buttonPositionScaleUp.UseVisualStyleBackColor = true;
            this.buttonPositionScaleUp.Click += new System.EventHandler(this.buttonPositionScaleUp_Click);
            // 
            // buttonPositionScaleDown
            // 
            this.buttonPositionScaleDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonPositionScaleDown.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.buttonPositionScaleDown.Location = new System.Drawing.Point(332, 215);
            this.buttonPositionScaleDown.Margin = new System.Windows.Forms.Padding(0);
            this.buttonPositionScaleDown.Name = "buttonPositionScaleDown";
            this.buttonPositionScaleDown.Size = new System.Drawing.Size(16, 16);
            this.buttonPositionScaleDown.TabIndex = 6;
            this.buttonPositionScaleDown.Text = "-";
            this.buttonPositionScaleDown.UseVisualStyleBackColor = true;
            this.buttonPositionScaleDown.Click += new System.EventHandler(this.buttonPositionScaleDown_Click);
            // 
            // comboReverb
            // 
            this.comboReverb.DropDownHeight = 300;
            this.comboReverb.FormattingEnabled = true;
            this.comboReverb.IntegralHeight = false;
            this.comboReverb.Items.AddRange(new object[] {
            "Off",
            "Generic",
            "Padded cell",
            "Room",
            "Bathroom",
            "Living room",
            "Stone room",
            "Auditorium",
            "Concert hall",
            "Cave",
            "Arena",
            "Hangar",
            "Carpetted hallway",
            "Hallway",
            "Stone Corridor",
            "Alley",
            "Forest",
            "City",
            "Mountains",
            "Quarry",
            "Plain",
            "Parkinglot",
            "Sewerpipe",
            "Under water"});
            this.comboReverb.Location = new System.Drawing.Point(6, 20);
            this.comboReverb.Name = "comboReverb";
            this.comboReverb.Size = new System.Drawing.Size(196, 20);
            this.comboReverb.TabIndex = 0;
            this.comboReverb.SelectedIndexChanged += new System.EventHandler(this.comboReverb_SelectedIndexChanged);
            // 
            // propertyReverb
            // 
            this.propertyReverb.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.propertyReverb.Location = new System.Drawing.Point(6, 46);
            this.propertyReverb.Name = "propertyReverb";
            this.propertyReverb.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.propertyReverb.Size = new System.Drawing.Size(196, 189);
            this.propertyReverb.TabIndex = 2;
            this.propertyReverb.ToolbarVisible = false;
            this.propertyReverb.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyReverb_PropertyValueChanged);
            // 
            // pictureSoundPositionInfo
            // 
            this.pictureSoundPositionInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureSoundPositionInfo.Location = new System.Drawing.Point(208, 20);
            this.pictureSoundPositionInfo.Name = "pictureSoundPositionInfo";
            this.pictureSoundPositionInfo.Size = new System.Drawing.Size(144, 215);
            this.pictureSoundPositionInfo.TabIndex = 5;
            this.pictureSoundPositionInfo.TabStop = false;
            this.pictureSoundPositionInfo.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureSoundPositionInfo_MouseMove);
            this.pictureSoundPositionInfo.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureSoundPositionInfo_MouseDown);
            this.pictureSoundPositionInfo.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureSoundPositionInfo_Paint);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.labelSoundControl);
            this.groupBox2.Controls.Add(this.radioPanlevel);
            this.groupBox2.Controls.Add(this.radioLowpass);
            this.groupBox2.Controls.Add(this.radioReverb);
            this.groupBox2.Controls.Add(this.radioDirect);
            this.groupBox2.Controls.Add(this.radioRolloff);
            this.groupBox2.Controls.Add(this.pictureSoundControlInfo);
            this.groupBox2.Location = new System.Drawing.Point(3, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(358, 193);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Sound Controls";
            // 
            // labelSoundControl
            // 
            this.labelSoundControl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.labelSoundControl.Location = new System.Drawing.Point(4, 150);
            this.labelSoundControl.Name = "labelSoundControl";
            this.labelSoundControl.Size = new System.Drawing.Size(198, 37);
            this.labelSoundControl.TabIndex = 10;
            // 
            // radioPanlevel
            // 
            this.radioPanlevel.AutoSize = true;
            this.radioPanlevel.Location = new System.Drawing.Point(6, 108);
            this.radioPanlevel.Name = "radioPanlevel";
            this.radioPanlevel.Size = new System.Drawing.Size(75, 16);
            this.radioPanlevel.TabIndex = 9;
            this.radioPanlevel.Text = "Pan level";
            this.radioPanlevel.UseVisualStyleBackColor = true;
            this.radioPanlevel.CheckedChanged += new System.EventHandler(this.radioPanlevel_CheckedChanged);
            // 
            // radioLowpass
            // 
            this.radioLowpass.AutoSize = true;
            this.radioLowpass.Location = new System.Drawing.Point(6, 64);
            this.radioLowpass.Name = "radioLowpass";
            this.radioLowpass.Size = new System.Drawing.Size(81, 16);
            this.radioLowpass.TabIndex = 8;
            this.radioLowpass.Text = "Low-pass";
            this.radioLowpass.UseVisualStyleBackColor = true;
            this.radioLowpass.CheckedChanged += new System.EventHandler(this.radioLowpass_CheckedChanged);
            // 
            // radioReverb
            // 
            this.radioReverb.AutoSize = true;
            this.radioReverb.Location = new System.Drawing.Point(6, 42);
            this.radioReverb.Name = "radioReverb";
            this.radioReverb.Size = new System.Drawing.Size(62, 16);
            this.radioReverb.TabIndex = 7;
            this.radioReverb.Text = "Reverb";
            this.radioReverb.UseVisualStyleBackColor = true;
            this.radioReverb.CheckedChanged += new System.EventHandler(this.radioReverb_CheckedChanged);
            // 
            // radioDirect
            // 
            this.radioDirect.AutoSize = true;
            this.radioDirect.Checked = true;
            this.radioDirect.Location = new System.Drawing.Point(6, 20);
            this.radioDirect.Name = "radioDirect";
            this.radioDirect.Size = new System.Drawing.Size(55, 16);
            this.radioDirect.TabIndex = 6;
            this.radioDirect.TabStop = true;
            this.radioDirect.Text = "Direct";
            this.radioDirect.UseVisualStyleBackColor = true;
            this.radioDirect.CheckedChanged += new System.EventHandler(this.radioDirect_CheckedChanged);
            // 
            // radioRolloff
            // 
            this.radioRolloff.AutoSize = true;
            this.radioRolloff.Location = new System.Drawing.Point(6, 86);
            this.radioRolloff.Name = "radioRolloff";
            this.radioRolloff.Size = new System.Drawing.Size(63, 16);
            this.radioRolloff.TabIndex = 5;
            this.radioRolloff.Text = "Roll-off";
            this.radioRolloff.UseVisualStyleBackColor = true;
            this.radioRolloff.CheckedChanged += new System.EventHandler(this.radioRolloff_CheckedChanged);
            // 
            // pictureSoundControlInfo
            // 
            this.pictureSoundControlInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureSoundControlInfo.Location = new System.Drawing.Point(208, 20);
            this.pictureSoundControlInfo.Name = "pictureSoundControlInfo";
            this.pictureSoundControlInfo.Size = new System.Drawing.Size(144, 167);
            this.pictureSoundControlInfo.TabIndex = 4;
            this.pictureSoundControlInfo.TabStop = false;
            this.pictureSoundControlInfo.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureSoundControlInfo_MouseMove);
            this.pictureSoundControlInfo.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureSoundControlInfo_MouseDown);
            this.pictureSoundControlInfo.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureSoundControlInfo_Paint);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.panelComponent);
            this.tabPage2.Controls.Add(this.buttonPauseComponent);
            this.tabPage2.Controls.Add(this.buttonPlayComponent);
            this.tabPage2.Controls.Add(this.button1);
            this.tabPage2.Location = new System.Drawing.Point(4, 21);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(624, 460);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Component map";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // panelComponent
            // 
            this.panelComponent.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelComponent.Location = new System.Drawing.Point(3, 3);
            this.panelComponent.Name = "panelComponent";
            this.panelComponent.Size = new System.Drawing.Size(618, 454);
            this.panelComponent.TabIndex = 6;
            this.panelComponent.Paint += new System.Windows.Forms.PaintEventHandler(this.panelComponent_Paint);
            this.panelComponent.Scroll += new System.Windows.Forms.ScrollEventHandler(this.panelComponent_Scroll);
            this.panelComponent.SizeChanged += new System.EventHandler(this.panelComponent_SizeChanged);
            // 
            // buttonPauseComponent
            // 
            this.buttonPauseComponent.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonPauseComponent.Location = new System.Drawing.Point(87, 466);
            this.buttonPauseComponent.Name = "buttonPauseComponent";
            this.buttonPauseComponent.Size = new System.Drawing.Size(75, 23);
            this.buttonPauseComponent.TabIndex = 3;
            this.buttonPauseComponent.Text = "Pause";
            this.buttonPauseComponent.UseVisualStyleBackColor = true;
            // 
            // buttonPlayComponent
            // 
            this.buttonPlayComponent.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonPlayComponent.Location = new System.Drawing.Point(6, 466);
            this.buttonPlayComponent.Name = "buttonPlayComponent";
            this.buttonPlayComponent.Size = new System.Drawing.Size(75, 23);
            this.buttonPlayComponent.TabIndex = 2;
            this.buttonPlayComponent.Text = "Play";
            this.buttonPlayComponent.UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(546, 463);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "Clear";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // buttonPlay
            // 
            this.buttonPlay.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonPlay.Location = new System.Drawing.Point(3, 494);
            this.buttonPlay.Name = "buttonPlay";
            this.buttonPlay.Size = new System.Drawing.Size(134, 23);
            this.buttonPlay.TabIndex = 5;
            this.buttonPlay.Text = "Play";
            this.buttonPlay.UseVisualStyleBackColor = true;
            this.buttonPlay.Click += new System.EventHandler(this.buttonPlay_Click);
            // 
            // labelPlayTime
            // 
            this.labelPlayTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.labelPlayTime.Location = new System.Drawing.Point(143, 494);
            this.labelPlayTime.Name = "labelPlayTime";
            this.labelPlayTime.Size = new System.Drawing.Size(108, 23);
            this.labelPlayTime.TabIndex = 7;
            this.labelPlayTime.Text = "00:00 / 00:00";
            this.labelPlayTime.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // trackTimeLine
            // 
            this.trackTimeLine.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.trackTimeLine.AutoSize = false;
            this.trackTimeLine.BackColor = System.Drawing.SystemColors.Control;
            this.trackTimeLine.Location = new System.Drawing.Point(257, 494);
            this.trackTimeLine.Maximum = 100;
            this.trackTimeLine.Name = "trackTimeLine";
            this.trackTimeLine.Size = new System.Drawing.Size(378, 23);
            this.trackTimeLine.TabIndex = 6;
            this.trackTimeLine.TickFrequency = 0;
            this.trackTimeLine.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackTimeLine.Scroll += new System.EventHandler(this.trackTimeLine_Scroll);
            // 
            // menuStrip
            // 
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Size = new System.Drawing.Size(892, 24);
            this.menuStrip.TabIndex = 4;
            this.menuStrip.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.reloadToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.closeToolStripMenuItem,
            this.toolStripMenuItem1,
            this.setworkdirToolStripMenuItem,
            this.toolStripSeparator1,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.ShortcutKeyDisplayString = "(F)";
            this.fileToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.F)));
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // reloadToolStripMenuItem
            // 
            this.reloadToolStripMenuItem.Name = "reloadToolStripMenuItem";
            this.reloadToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.reloadToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.R)));
            this.reloadToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.reloadToolStripMenuItem.Text = "Reload";
            this.reloadToolStripMenuItem.Click += new System.EventHandler(this.reloadToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.saveAsToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.A)));
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.saveAsToolStripMenuItem.Text = "Save As...";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.closeToolStripMenuItem.Text = "Close";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(230, 6);
            // 
            // setworkdirToolStripMenuItem
            // 
            this.setworkdirToolStripMenuItem.Name = "setworkdirToolStripMenuItem";
            this.setworkdirToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.setworkdirToolStripMenuItem.Text = "Change working directory...";
            this.setworkdirToolStripMenuItem.Click += new System.EventHandler(this.setworkdirToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(230, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.exitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(233, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoToolStripMenuItem,
            this.redoToolStripMenuItem,
            this.toolStripSeparator2,
            this.validationToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.ShortcutKeyDisplayString = "(E)";
            this.editToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.E)));
            this.editToolStripMenuItem.Size = new System.Drawing.Size(38, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // undoToolStripMenuItem
            // 
            this.undoToolStripMenuItem.Enabled = false;
            this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
            this.undoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.undoToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.undoToolStripMenuItem.Text = "Undo";
            this.undoToolStripMenuItem.Click += new System.EventHandler(this.undoToolStripMenuItem_Click);
            // 
            // redoToolStripMenuItem
            // 
            this.redoToolStripMenuItem.Enabled = false;
            this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
            this.redoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
            this.redoToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.redoToolStripMenuItem.Text = "Redo";
            this.redoToolStripMenuItem.Click += new System.EventHandler(this.redoToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(151, 6);
            // 
            // validationToolStripMenuItem
            // 
            this.validationToolStripMenuItem.Name = "validationToolStripMenuItem";
            this.validationToolStripMenuItem.ShortcutKeyDisplayString = "(V)";
            this.validationToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.V)));
            this.validationToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.validationToolStripMenuItem.Text = "Validation";
            this.validationToolStripMenuItem.Click += new System.EventHandler(this.validationToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.ShortcutKeyDisplayString = "(H)";
            this.helpToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.H)));
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(42, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F1)));
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // timerUpdate
            // 
            this.timerUpdate.Interval = 1;
            this.timerUpdate.Tick += new System.EventHandler(this.timerUpdate_Tick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(892, 566);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.menuStrip);
            this.MainMenuStrip = this.menuStrip;
            this.Name = "MainForm";
            this.Text = "Sound Manager - MAIET Entertainment";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.tabSoundEnvironment.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            this.splitContainer3.ResumeLayout(false);
            this.splitContainer4.Panel1.ResumeLayout(false);
            this.splitContainer4.Panel2.ResumeLayout(false);
            this.splitContainer4.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureSoundPositionInfo)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureSoundControlInfo)).EndInit();
            this.tabPage2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackTimeLine)).EndInit();
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView treeSoundList;
        private System.Windows.Forms.PropertyGrid propertySoundInfo;
        private System.Windows.Forms.TabControl tabSoundEnvironment;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.ComboBox comboReverb;
        private System.Windows.Forms.ImageList imageSoundType;
        private System.Windows.Forms.Button buttonAddSound;
        private System.Windows.Forms.Button buttonAddGroup;
        private System.Windows.Forms.Button buttonRemove;
        private System.Windows.Forms.ImageList imageButtons;
        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel;
        private System.Windows.Forms.ToolStripMenuItem reloadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem validationToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem setworkdirToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem undoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem redoToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.PropertyGrid propertyReverb;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.PictureBox pictureSoundControlInfo;
        private System.Windows.Forms.PictureBox pictureSoundPositionInfo;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button buttonPositionScaleUp;
        private System.Windows.Forms.Button buttonPositionScaleDown;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.SplitContainer splitContainer4;
        private System.Windows.Forms.ListView listSounds;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.Button buttonPlay;
        private System.Windows.Forms.Button buttonRemoveSubSound;
        private System.Windows.Forms.Button buttonAddSubSound;
        private System.Windows.Forms.TrackBar trackTimeLine;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button buttonPlayComponent;
        private System.Windows.Forms.Button buttonPauseComponent;
        private System.Windows.Forms.Label labelPlayTime;
        private System.Windows.Forms.Panel panelComponent;
        private System.Windows.Forms.RadioButton radioPanlevel;
        private System.Windows.Forms.RadioButton radioLowpass;
        private System.Windows.Forms.RadioButton radioReverb;
        private System.Windows.Forms.RadioButton radioDirect;
        private System.Windows.Forms.RadioButton radioRolloff;
        private System.Windows.Forms.Timer timerUpdate;
        private System.Windows.Forms.Label labelSoundControl;
    }
}

