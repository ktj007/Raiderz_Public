using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace Genie
{
	/// <summary>
	/// Form1에 대한 요약 설명입니다.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.TabControl tcMain;
		private System.Windows.Forms.TabPage tpModel;
		private System.Windows.Forms.TabPage tpConst;
		private System.Windows.Forms.MenuItem miQuit;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem miOption;
		public System.Windows.Forms.ListView lvConst;
		private System.Windows.Forms.ColumnHeader columnHeader1;
		private System.Windows.Forms.ColumnHeader columnHeader2;
		private System.Windows.Forms.ColumnHeader columnHeader3;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.MenuItem menuItem10;
		private System.Windows.Forms.Splitter splitter2;
		private System.Windows.Forms.ImageList TabPageimageList;
		private System.Windows.Forms.MenuItem menuItem11;
		private System.Windows.Forms.MenuItem menuItem12;
		private System.Windows.Forms.StatusBar statusBar;
		private System.Windows.Forms.StatusBarPanel statusBarPanel1;
		private System.Windows.Forms.MenuItem miSave;
		private System.Windows.Forms.MenuItem miSaveAll;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.ColumnHeader columnHeader4;
		private System.Windows.Forms.ColumnHeader columnHeader5;
		private System.Windows.Forms.ColumnHeader columnHeader6;
		public System.Windows.Forms.TreeView tvModel;
		private System.Windows.Forms.ImageList ModelTreeViewImageList;
		private System.Windows.Forms.TextBox tbModelPath;
		private System.Windows.Forms.TextBox tbBaseModelFileName;
		private System.Windows.Forms.TextBox tbBaseName;
		private System.Windows.Forms.ColumnHeader columnHeader7;
		private System.Windows.Forms.ColumnHeader columnHeader8;
		private System.Windows.Forms.ColumnHeader columnHeader9;
		private System.Windows.Forms.ListView lvNormalAnimation;
		private System.Windows.Forms.ContextMenu cmModelListTreeView;
		private System.Windows.Forms.MenuItem menuItem8;
		private System.Windows.Forms.MenuItem menuItem9;
		private System.Windows.Forms.MenuItem menuItem14;
		private System.Windows.Forms.MenuItem menuItem15;
		private System.Windows.Forms.ListBox lbWeaponAniName;
		private System.Windows.Forms.ListView lvWeaponAni;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.ContextMenu cmNormalAnimationListView;
		private System.Windows.Forms.MenuItem menuItem17;
		private System.Windows.Forms.MenuItem miAddNormalAni;
		private System.Windows.Forms.MenuItem miEditNormalAni;
		private System.Windows.Forms.MenuItem miDelNormalAni;
		private System.Windows.Forms.MenuItem miAbout;
		private System.Windows.Forms.ContextMenu cmWeaponAniNameListBox;
		private System.Windows.Forms.MenuItem menuItem13;
		private System.Windows.Forms.MenuItem menuItem16;
		private System.Windows.Forms.MenuItem menuItem18;
		private System.Windows.Forms.MenuItem menuItem20;
		private System.Windows.Forms.MenuItem menuItem21;
		private System.Windows.Forms.MenuItem menuItem22;
		private System.Windows.Forms.ContextMenu cmWeaponAniListView;
		private System.Windows.Forms.MenuItem miEditWeaponAni;
		private System.Windows.Forms.MenuItem miRemoveWeaponAni;
		private System.Windows.Forms.MenuItem miAddAnimation2;

		#region 버드가 추가한 변수
		
		private ModelData m_CurrentModelData;

		#endregion

		public MainForm()
		{
			//
			// Windows Form 디자이너 지원에 필요합니다.
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent를 호출한 다음 생성자 코드를 추가합니다.
			//
			m_CurrentModelData = null;
		}

		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form 디자이너에서 생성한 코드
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.miSave = new System.Windows.Forms.MenuItem();
			this.miSaveAll = new System.Windows.Forms.MenuItem();
			this.menuItem10 = new System.Windows.Forms.MenuItem();
			this.miQuit = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.miOption = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItem11 = new System.Windows.Forms.MenuItem();
			this.menuItem12 = new System.Windows.Forms.MenuItem();
			this.miAbout = new System.Windows.Forms.MenuItem();
			this.tcMain = new System.Windows.Forms.TabControl();
			this.tpModel = new System.Windows.Forms.TabPage();
			this.panel1 = new System.Windows.Forms.Panel();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.lvNormalAnimation = new System.Windows.Forms.ListView();
			this.columnHeader7 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader8 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader9 = new System.Windows.Forms.ColumnHeader();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.lvWeaponAni = new System.Windows.Forms.ListView();
			this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader5 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader6 = new System.Windows.Forms.ColumnHeader();
			this.label3 = new System.Windows.Forms.Label();
			this.lbWeaponAniName = new System.Windows.Forms.ListBox();
			this.panel2 = new System.Windows.Forms.Panel();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.tbBaseModelFileName = new System.Windows.Forms.TextBox();
			this.tbBaseName = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.tbModelPath = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.splitter2 = new System.Windows.Forms.Splitter();
			this.tvModel = new System.Windows.Forms.TreeView();
			this.cmModelListTreeView = new System.Windows.Forms.ContextMenu();
			this.menuItem8 = new System.Windows.Forms.MenuItem();
			this.menuItem14 = new System.Windows.Forms.MenuItem();
			this.menuItem9 = new System.Windows.Forms.MenuItem();
			this.menuItem15 = new System.Windows.Forms.MenuItem();
			this.ModelTreeViewImageList = new System.Windows.Forms.ImageList(this.components);
			this.tpConst = new System.Windows.Forms.TabPage();
			this.lvConst = new System.Windows.Forms.ListView();
			this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
			this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
			this.TabPageimageList = new System.Windows.Forms.ImageList(this.components);
			this.statusBar = new System.Windows.Forms.StatusBar();
			this.statusBarPanel1 = new System.Windows.Forms.StatusBarPanel();
			this.cmNormalAnimationListView = new System.Windows.Forms.ContextMenu();
			this.miAddNormalAni = new System.Windows.Forms.MenuItem();
			this.menuItem17 = new System.Windows.Forms.MenuItem();
			this.miEditNormalAni = new System.Windows.Forms.MenuItem();
			this.miDelNormalAni = new System.Windows.Forms.MenuItem();
			this.cmWeaponAniNameListBox = new System.Windows.Forms.ContextMenu();
			this.cmWeaponAniListView = new System.Windows.Forms.ContextMenu();
			this.menuItem13 = new System.Windows.Forms.MenuItem();
			this.menuItem16 = new System.Windows.Forms.MenuItem();
			this.menuItem18 = new System.Windows.Forms.MenuItem();
			this.miAddAnimation2 = new System.Windows.Forms.MenuItem();
			this.menuItem20 = new System.Windows.Forms.MenuItem();
			this.menuItem21 = new System.Windows.Forms.MenuItem();
			this.menuItem22 = new System.Windows.Forms.MenuItem();
			this.miEditWeaponAni = new System.Windows.Forms.MenuItem();
			this.miRemoveWeaponAni = new System.Windows.Forms.MenuItem();
			this.tcMain.SuspendLayout();
			this.tpModel.SuspendLayout();
			this.panel1.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.tabPage2.SuspendLayout();
			this.panel2.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.tpConst.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.statusBarPanel1)).BeginInit();
			this.SuspendLayout();
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem1,
																					  this.menuItem5,
																					  this.menuItem2,
																					  this.menuItem3});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem6,
																					  this.menuItem7,
																					  this.miSave,
																					  this.miSaveAll,
																					  this.menuItem10,
																					  this.miQuit});
			this.menuItem1.Text = "파일(&F)";
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 0;
			this.menuItem6.Text = "상수값 열기(&O)...";
			// 
			// menuItem7
			// 
			this.menuItem7.Index = 1;
			this.menuItem7.Text = "-";
			// 
			// miSave
			// 
			this.miSave.Index = 2;
			this.miSave.Shortcut = System.Windows.Forms.Shortcut.CtrlS;
			this.miSave.Text = "저장(&S)...";
			this.miSave.Click += new System.EventHandler(this.miSave_Click);
			// 
			// miSaveAll
			// 
			this.miSaveAll.Index = 3;
			this.miSaveAll.Shortcut = System.Windows.Forms.Shortcut.CtrlShiftS;
			this.miSaveAll.Text = "모두 저장(&L)...";
			this.miSaveAll.Click += new System.EventHandler(this.miSaveAll_Click);
			// 
			// menuItem10
			// 
			this.menuItem10.Index = 4;
			this.menuItem10.Text = "-";
			// 
			// miQuit
			// 
			this.miQuit.Index = 5;
			this.miQuit.Text = "끝내기(&X)";
			this.miQuit.Click += new System.EventHandler(this.miQuit_Click);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 1;
			this.menuItem5.Text = "편집(&E)";
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 2;
			this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.miOption});
			this.menuItem2.Text = "도구(&T)";
			// 
			// miOption
			// 
			this.miOption.Index = 0;
			this.miOption.Text = "옵션(&O)...";
			this.miOption.Click += new System.EventHandler(this.menuItem6_Click);
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 3;
			this.menuItem3.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem4,
																					  this.menuItem11,
																					  this.menuItem12,
																					  this.miAbout});
			this.menuItem3.Text = "도움말(&H)";
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 0;
			this.menuItem4.Text = "피드백(K)..";
			this.menuItem4.Click += new System.EventHandler(this.menuItem4_Click);
			// 
			// menuItem11
			// 
			this.menuItem11.Index = 1;
			this.menuItem11.Text = "도움말(&H)..";
			this.menuItem11.Click += new System.EventHandler(this.menuItem11_Click);
			// 
			// menuItem12
			// 
			this.menuItem12.Index = 2;
			this.menuItem12.Text = "-";
			// 
			// miAbout
			// 
			this.miAbout.Index = 3;
			this.miAbout.Text = "정보(&I)..";
			this.miAbout.Click += new System.EventHandler(this.miAbout_Click);
			// 
			// tcMain
			// 
			this.tcMain.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tcMain.Controls.Add(this.tpModel);
			this.tcMain.Controls.Add(this.tpConst);
			this.tcMain.ImageList = this.TabPageimageList;
			this.tcMain.ItemSize = new System.Drawing.Size(72, 24);
			this.tcMain.Location = new System.Drawing.Point(0, 0);
			this.tcMain.Name = "tcMain";
			this.tcMain.SelectedIndex = 0;
			this.tcMain.Size = new System.Drawing.Size(896, 612);
			this.tcMain.TabIndex = 1;
			this.tcMain.TabIndexChanged += new System.EventHandler(this.tcMain_TabIndexChanged);
			// 
			// tpModel
			// 
			this.tpModel.Controls.Add(this.panel1);
			this.tpModel.Controls.Add(this.splitter2);
			this.tpModel.Controls.Add(this.tvModel);
			this.tpModel.ImageIndex = 1;
			this.tpModel.Location = new System.Drawing.Point(4, 28);
			this.tpModel.Name = "tpModel";
			this.tpModel.Size = new System.Drawing.Size(888, 580);
			this.tpModel.TabIndex = 1;
			this.tpModel.Text = "모델리스트";
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.tabControl1);
			this.panel1.Controls.Add(this.panel2);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(235, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(653, 580);
			this.panel1.TabIndex = 2;
			// 
			// tabControl1
			// 
			this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tabControl1.Controls.Add(this.tabPage1);
			this.tabControl1.Controls.Add(this.tabPage2);
			this.tabControl1.ItemSize = new System.Drawing.Size(100, 22);
			this.tabControl1.Location = new System.Drawing.Point(16, 160);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(616, 396);
			this.tabControl1.TabIndex = 1;
			// 
			// tabPage1
			// 
			this.tabPage1.Controls.Add(this.lvNormalAnimation);
			this.tabPage1.Location = new System.Drawing.Point(4, 26);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Size = new System.Drawing.Size(608, 366);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "일반 애니메이션";
			// 
			// lvNormalAnimation
			// 
			this.lvNormalAnimation.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																								this.columnHeader7,
																								this.columnHeader8,
																								this.columnHeader9});
			this.lvNormalAnimation.ContextMenu = this.cmNormalAnimationListView;
			this.lvNormalAnimation.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lvNormalAnimation.FullRowSelect = true;
			this.lvNormalAnimation.Location = new System.Drawing.Point(0, 0);
			this.lvNormalAnimation.Name = "lvNormalAnimation";
			this.lvNormalAnimation.Size = new System.Drawing.Size(608, 366);
			this.lvNormalAnimation.TabIndex = 4;
			this.lvNormalAnimation.View = System.Windows.Forms.View.Details;
			this.lvNormalAnimation.DoubleClick += new System.EventHandler(this.lvNormalAnimation_DoubleClick);
			// 
			// columnHeader7
			// 
			this.columnHeader7.Text = "Name";
			this.columnHeader7.Width = 140;
			// 
			// columnHeader8
			// 
			this.columnHeader8.Text = "Filename";
			this.columnHeader8.Width = 280;
			// 
			// columnHeader9
			// 
			this.columnHeader9.Text = "Loop";
			this.columnHeader9.Width = 80;
			// 
			// tabPage2
			// 
			this.tabPage2.Controls.Add(this.lvWeaponAni);
			this.tabPage2.Controls.Add(this.label3);
			this.tabPage2.Controls.Add(this.lbWeaponAniName);
			this.tabPage2.Location = new System.Drawing.Point(4, 26);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Size = new System.Drawing.Size(608, 366);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "무기별 애니메이션";
			// 
			// lvWeaponAni
			// 
			this.lvWeaponAni.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.lvWeaponAni.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																						  this.columnHeader4,
																						  this.columnHeader5,
																						  this.columnHeader6});
			this.lvWeaponAni.ContextMenu = this.cmWeaponAniListView;
			this.lvWeaponAni.FullRowSelect = true;
			this.lvWeaponAni.Location = new System.Drawing.Point(184, 40);
			this.lvWeaponAni.Name = "lvWeaponAni";
			this.lvWeaponAni.Size = new System.Drawing.Size(408, 292);
			this.lvWeaponAni.TabIndex = 2;
			this.lvWeaponAni.View = System.Windows.Forms.View.Details;
			this.lvWeaponAni.DoubleClick += new System.EventHandler(this.lvWeaponAni_DoubleClick);
			// 
			// columnHeader4
			// 
			this.columnHeader4.Text = "무기타입";
			// 
			// columnHeader5
			// 
			this.columnHeader5.Text = "파일";
			this.columnHeader5.Width = 200;
			// 
			// columnHeader6
			// 
			this.columnHeader6.Text = "Loop";
			this.columnHeader6.Width = 80;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(17, 15);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(100, 16);
			this.label3.TabIndex = 1;
			this.label3.Text = "애니메이션";
			// 
			// lbWeaponAniName
			// 
			this.lbWeaponAniName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left)));
			this.lbWeaponAniName.ContextMenu = this.cmWeaponAniNameListBox;
			this.lbWeaponAniName.ItemHeight = 12;
			this.lbWeaponAniName.Location = new System.Drawing.Point(16, 40);
			this.lbWeaponAniName.Name = "lbWeaponAniName";
			this.lbWeaponAniName.Size = new System.Drawing.Size(152, 280);
			this.lbWeaponAniName.TabIndex = 0;
			this.lbWeaponAniName.SelectedIndexChanged += new System.EventHandler(this.lbWeaponAniName_SelectedIndexChanged);
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.groupBox1);
			this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel2.Location = new System.Drawing.Point(0, 0);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(653, 144);
			this.panel2.TabIndex = 0;
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.tbBaseModelFileName);
			this.groupBox1.Controls.Add(this.tbBaseName);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.tbModelPath);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(13, 10);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(626, 128);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "정보";
			// 
			// tbBaseModelFileName
			// 
			this.tbBaseModelFileName.Location = new System.Drawing.Point(263, 70);
			this.tbBaseModelFileName.Name = "tbBaseModelFileName";
			this.tbBaseModelFileName.ReadOnly = true;
			this.tbBaseModelFileName.Size = new System.Drawing.Size(193, 21);
			this.tbBaseModelFileName.TabIndex = 4;
			this.tbBaseModelFileName.Text = "";
			// 
			// tbBaseName
			// 
			this.tbBaseName.Location = new System.Drawing.Point(129, 69);
			this.tbBaseName.Name = "tbBaseName";
			this.tbBaseName.ReadOnly = true;
			this.tbBaseName.Size = new System.Drawing.Size(119, 21);
			this.tbBaseName.TabIndex = 3;
			this.tbBaseName.Text = "";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(28, 73);
			this.label2.Name = "label2";
			this.label2.TabIndex = 2;
			this.label2.Text = "베이스모델 :";
			// 
			// tbModelPath
			// 
			this.tbModelPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tbModelPath.Location = new System.Drawing.Point(129, 42);
			this.tbModelPath.Name = "tbModelPath";
			this.tbModelPath.ReadOnly = true;
			this.tbModelPath.Size = new System.Drawing.Size(447, 21);
			this.tbModelPath.TabIndex = 1;
			this.tbModelPath.Text = "";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(28, 46);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(100, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "모델 XML 파일 : ";
			// 
			// splitter2
			// 
			this.splitter2.Location = new System.Drawing.Point(232, 0);
			this.splitter2.Name = "splitter2";
			this.splitter2.Size = new System.Drawing.Size(3, 580);
			this.splitter2.TabIndex = 1;
			this.splitter2.TabStop = false;
			// 
			// tvModel
			// 
			this.tvModel.ContextMenu = this.cmModelListTreeView;
			this.tvModel.Dock = System.Windows.Forms.DockStyle.Left;
			this.tvModel.ImageList = this.ModelTreeViewImageList;
			this.tvModel.Location = new System.Drawing.Point(0, 0);
			this.tvModel.Name = "tvModel";
			this.tvModel.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
																				new System.Windows.Forms.TreeNode("Player", new System.Windows.Forms.TreeNode[] {
																																									new System.Windows.Forms.TreeNode("human_male")}),
																				new System.Windows.Forms.TreeNode("Monster"),
																				new System.Windows.Forms.TreeNode("NPC"),
																				new System.Windows.Forms.TreeNode("Weapon")});
			this.tvModel.Size = new System.Drawing.Size(232, 580);
			this.tvModel.TabIndex = 0;
			this.tvModel.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvModel_AfterSelect);
			// 
			// cmModelListTreeView
			// 
			this.cmModelListTreeView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																								this.menuItem8,
																								this.menuItem14,
																								this.menuItem9,
																								this.menuItem15});
			this.cmModelListTreeView.Popup += new System.EventHandler(this.cmModelListTreeView_Popup);
			// 
			// menuItem8
			// 
			this.menuItem8.Index = 0;
			this.menuItem8.Text = "추가";
			// 
			// menuItem14
			// 
			this.menuItem14.Index = 1;
			this.menuItem14.Text = "편집";
			// 
			// menuItem9
			// 
			this.menuItem9.Index = 2;
			this.menuItem9.Text = "-";
			// 
			// menuItem15
			// 
			this.menuItem15.Index = 3;
			this.menuItem15.Text = "삭제";
			// 
			// ModelTreeViewImageList
			// 
			this.ModelTreeViewImageList.ImageSize = new System.Drawing.Size(16, 16);
			this.ModelTreeViewImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ModelTreeViewImageList.ImageStream")));
			this.ModelTreeViewImageList.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// tpConst
			// 
			this.tpConst.Controls.Add(this.lvConst);
			this.tpConst.ImageIndex = 2;
			this.tpConst.Location = new System.Drawing.Point(4, 28);
			this.tpConst.Name = "tpConst";
			this.tpConst.Size = new System.Drawing.Size(888, 580);
			this.tpConst.TabIndex = 2;
			this.tpConst.Text = "상수값";
			// 
			// lvConst
			// 
			this.lvConst.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																					  this.columnHeader1,
																					  this.columnHeader2,
																					  this.columnHeader3});
			this.lvConst.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lvConst.FullRowSelect = true;
			this.lvConst.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
			this.lvConst.Location = new System.Drawing.Point(0, 0);
			this.lvConst.Name = "lvConst";
			this.lvConst.Size = new System.Drawing.Size(888, 580);
			this.lvConst.TabIndex = 0;
			this.lvConst.View = System.Windows.Forms.View.Details;
			this.lvConst.DoubleClick += new System.EventHandler(this.lvConst_DoubleClick);
			this.lvConst.SelectedIndexChanged += new System.EventHandler(this.lvConst_SelectedIndexChanged);
			// 
			// columnHeader1
			// 
			this.columnHeader1.Text = "Name";
			this.columnHeader1.Width = 180;
			// 
			// columnHeader2
			// 
			this.columnHeader2.Text = "Value";
			this.columnHeader2.Width = 400;
			// 
			// columnHeader3
			// 
			this.columnHeader3.Text = "Type";
			// 
			// TabPageimageList
			// 
			this.TabPageimageList.ImageSize = new System.Drawing.Size(16, 16);
			this.TabPageimageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("TabPageimageList.ImageStream")));
			this.TabPageimageList.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// statusBar
			// 
			this.statusBar.Location = new System.Drawing.Point(0, 617);
			this.statusBar.Name = "statusBar";
			this.statusBar.Panels.AddRange(new System.Windows.Forms.StatusBarPanel[] {
																						 this.statusBarPanel1});
			this.statusBar.ShowPanels = true;
			this.statusBar.Size = new System.Drawing.Size(896, 24);
			this.statusBar.TabIndex = 2;
			// 
			// statusBarPanel1
			// 
			this.statusBarPanel1.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Spring;
			this.statusBarPanel1.Width = 880;
			// 
			// cmNormalAnimationListView
			// 
			this.cmNormalAnimationListView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																									  this.miEditNormalAni,
																									  this.miDelNormalAni,
																									  this.menuItem17,
																									  this.miAddNormalAni});
			// 
			// miAddNormalAni
			// 
			this.miAddNormalAni.Index = 3;
			this.miAddNormalAni.Text = "추가(&A)...";
			this.miAddNormalAni.Click += new System.EventHandler(this.miAddNormalAni_Click);
			// 
			// menuItem17
			// 
			this.menuItem17.Index = 2;
			this.menuItem17.Text = "-";
			// 
			// miEditNormalAni
			// 
			this.miEditNormalAni.Index = 0;
			this.miEditNormalAni.Text = "편집(&E)...";
			this.miEditNormalAni.Click += new System.EventHandler(this.miEditNormalAni_Click);
			// 
			// miDelNormalAni
			// 
			this.miDelNormalAni.Index = 1;
			this.miDelNormalAni.Text = "삭제(&D)...";
			this.miDelNormalAni.Click += new System.EventHandler(this.miDelNormalAni_Click);
			// 
			// cmWeaponAniNameListBox
			// 
			this.cmWeaponAniNameListBox.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																								   this.menuItem20,
																								   this.menuItem21,
																								   this.menuItem22,
																								   this.menuItem13,
																								   this.menuItem16,
																								   this.menuItem18,
																								   this.miAddAnimation2});
			// 
			// cmWeaponAniListView
			// 
			this.cmWeaponAniListView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																								this.miEditWeaponAni,
																								this.miRemoveWeaponAni});
			// 
			// menuItem13
			// 
			this.menuItem13.Index = 3;
			this.menuItem13.Text = "편집(&E)...";
			// 
			// menuItem16
			// 
			this.menuItem16.Index = 4;
			this.menuItem16.Text = "삭제(&D)...";
			// 
			// menuItem18
			// 
			this.menuItem18.Index = 5;
			this.menuItem18.Text = "-";
			// 
			// miAddAnimation2
			// 
			this.miAddAnimation2.Index = 6;
			this.miAddAnimation2.Text = "추가(&I)...";
			this.miAddAnimation2.Click += new System.EventHandler(this.miAddAnimation2_Click);
			// 
			// menuItem20
			// 
			this.menuItem20.Index = 0;
			this.menuItem20.Text = "위로";
			// 
			// menuItem21
			// 
			this.menuItem21.Index = 1;
			this.menuItem21.Text = "아래로";
			// 
			// menuItem22
			// 
			this.menuItem22.Index = 2;
			this.menuItem22.Text = "-";
			// 
			// miEditWeaponAni
			// 
			this.miEditWeaponAni.Index = 0;
			this.miEditWeaponAni.Text = "편집(&E)...";
			this.miEditWeaponAni.Click += new System.EventHandler(this.miEditWeaponAni_Click);
			// 
			// miRemoveWeaponAni
			// 
			this.miRemoveWeaponAni.Index = 1;
			this.miRemoveWeaponAni.Text = "삭제(&D)...";
			this.miRemoveWeaponAni.Click += new System.EventHandler(this.miRemoveWeaponAni_Click);
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(896, 641);
			this.Controls.Add(this.statusBar);
			this.Controls.Add(this.tcMain);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Menu = this.mainMenu1;
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Genie 0.1";
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.tcMain.ResumeLayout(false);
			this.tpModel.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.tabPage2.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.tpConst.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.statusBarPanel1)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// 해당 응용 프로그램의 주 진입점입니다.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new MainForm());
		}

		private void miQuit_Click(object sender, System.EventArgs e)
		{
			this.Dispose();
		}

		private void MainForm_Load(object sender, System.EventArgs e)
		{
			Global.Init(this);
			this.Text = "Genie " + Convert.ToString(Const.fVersion);
		}

		private void menuItem4_Click(object sender, System.EventArgs e)
		{
			System.Diagnostics.Process.Start( Global.szFeedbackUrl );
		}

		private void menuItem6_Click(object sender, System.EventArgs e)
		{
			OptionForm dlgOption = new OptionForm();
			dlgOption.ShowDialog();
			DialogResult res = dlgOption.DialogResult;
			if (res == DialogResult.OK)
			{
				Global.SaveOption();
				Global.LoadAllData();
			}
		}

		private void lvConst_DoubleClick(object sender, System.EventArgs e)
		{
			string szKey="", szValue="";
			string szDesc="";
			ValueType nValueType=ValueType.VT_STRING;

			if (lvConst.SelectedItems.Count > 0)
			{
				szKey = lvConst.SelectedItems[0].Text;
				szValue = lvConst.SelectedItems[0].SubItems[1].Text;
				nValueType = Global.ini_data.GetValueTypeFromConstKey(szKey);
				szDesc = Global.ini_data.GetStringFromConstKey(szKey);

				ConstDataInputForm dlgInput = new ConstDataInputForm(szKey, szValue, szDesc, nValueType);
				dlgInput.ShowDialog();
				DialogResult res = dlgInput.DialogResult;
				if (res == DialogResult.OK)
				{
					Global.const_data.SetValue(szKey, dlgInput.Value);
					Global.const_data.Serialize(true, this.lvConst);

					InvalidateTabNames();
				}
			}

		}

		private void menuItem11_Click(object sender, System.EventArgs e)
		{
			System.Diagnostics.Process.Start( Global.szHelpUrl );
		}

		public void ShowStatusBarTip(string text)
		{
			statusBar.Panels[0].Text = text;
		}

		private void lvConst_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (lvConst.SelectedItems.Count > 0)
			{
				string szKey = lvConst.SelectedItems[0].Text;
				string szDesc = Global.ini_data.GetStringFromConstKey(szKey);
				
				ShowStatusBarTip(szDesc);
			}
		}

		private void miSave_Click(object sender, System.EventArgs e)
		{
			if (tcMain.SelectedIndex == 0) // 모델 애니메이션
			{
				m_CurrentModelData.Save();
				SerializeModelTreeNode(m_CurrentModelData);
			}
			else if (tcMain.SelectedIndex == 1)	// 상수값
			{
				Global.const_data.Save(Global.szConstFileNameFullPath);
			}

			InvalidateTabNames();
		}

		public void InvalidateTabNames()
		{
			if (Global.const_data.Edited == true)
			{
				tpConst.Text = Const.szTabNameConst + " *";
			}
			else
			{
				tpConst.Text = Const.szTabNameConst;
			}

		}

		private void miSaveAll_Click(object sender, System.EventArgs e)
		{
			// const
			Global.const_data.Save(Global.szConstFileNameFullPath);


			InvalidateTabNames();
		}

		private void tvModel_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			TreeNode selNode = tvModel.SelectedNode;
			if (selNode == null) return;

			ModelData model_data = Global.modellist_data.FindModelData(selNode.Text);
			if (model_data == null) return;

			tbModelPath.Text = model_data.m_szPath;
			tbBaseName.Text = model_data.m_BaseModel.szName;
			tbBaseModelFileName.Text = model_data.m_BaseModel.szFileName;

			SerializeModelAnimation(model_data);
		}

		private void tcMain_TabIndexChanged(object sender, System.EventArgs e)
		{

		}


		private void cmModelListTreeView_Popup(object sender, System.EventArgs e)
		{
		
		}

		private void lbWeaponAniName_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			SerializeWeaponAnimation(m_CurrentModelData);
		}


		#region 버드가 추가한 함수

		public void SerializeModelAnimation(ModelData data)
		{
			m_CurrentModelData = data;

			lvNormalAnimation.Items.Clear();

			for (int i = 0; i < data.m_Animations.Count; i++)
			{
				ModelAnimation ani = (ModelAnimation)data.m_Animations[i];

				// 일반 애니메이션
				if (ani.nWeaponType == WeaponType.WT_NONE)
				{
					ListViewItem new_item = new ListViewItem();
					new_item.Text = ani.Name;
					new_item.SubItems.Add(ani.szFileName);
					
					new_item.SubItems.Add(ConvertString.GetAnimationLoopTypeStr(ani.nLoopType));

					lvNormalAnimation.Items.Add(new_item);
				}
			}

			// 무기 애니메이션 이름 목록
			lbWeaponAniName.Items.Clear();
			for (int i = 0; i < data.m_WeaponAnimationNames.Count; i++)
			{
				string str = (string)data.m_WeaponAnimationNames[i];
				
				lbWeaponAniName.Items.Add(str);
			}

			SerializeWeaponAnimation(data);
		}


		private void SerializeWeaponAnimation(ModelData data)
		{
			if (data == null) return;

			lvWeaponAni.Items.Clear();

			string szSelectedWeaponAnimation="";
			if (lbWeaponAniName.SelectedIndex >= 0)
			{
				szSelectedWeaponAnimation = (string)lbWeaponAniName.SelectedItem;
			}

			// 무기별 항목
			for (WeaponType type=WeaponType.WT_1H_SLASH; type <= WeaponType.WT_GUNPOWDER; type++)
			{
				ListViewItem new_item = new ListViewItem();
				new_item.Text = ConvertString.GetWeaponTypeStr(type);

				ModelAnimation ani = null;
				if (szSelectedWeaponAnimation != null) ani = data.FindAnimation(type, szSelectedWeaponAnimation);
				if (ani != null)
				{
					new_item.SubItems.Add(ani.szFileName);
					new_item.SubItems.Add(ConvertString.GetAnimationLoopTypeStr(ani.nLoopType));
				}

				lvWeaponAni.Items.Add(new_item);
			}

			SerializeModelTreeNode(data);
		}

		private void EditSelectedNormalAnimation()
		{
			if (lvNormalAnimation.SelectedItems.Count <= 0) return;
			if (m_CurrentModelData == null) return;

			ModelAniEdit dlgInput = new ModelAniEdit(ModelAniEdit.EditType.ET_EDIT_NORMAL);

			string szAniName = lvNormalAnimation.SelectedItems[0].Text;
			string szFileName = lvNormalAnimation.SelectedItems[0].SubItems[1].Text;
			bool bLoop=false;
			if (lvNormalAnimation.SelectedItems[0].SubItems[2].Text == "loop") bLoop=true;

			dlgInput.InitForEdit(m_CurrentModelData, szAniName, szFileName, WeaponType.WT_NONE, bLoop);

			dlgInput.ShowDialog();
			DialogResult res = dlgInput.DialogResult;
			if (res == DialogResult.OK)
			{
				string szNewAniName = dlgInput.m_szAniName;
				string szNewFileName = dlgInput.m_szFileName;
				bool bNewLoop = dlgInput.m_bLoop;
				
				AnimationLoopType nNewLoopType = AnimationLoopType.ALT_LASTFRAME;
				if (bNewLoop) nNewLoopType = AnimationLoopType.ALT_LOOP;

				m_CurrentModelData.EditAnimation(WeaponType.WT_NONE, szAniName, szNewAniName, szNewFileName, nNewLoopType);



				SerializeModelAnimation(m_CurrentModelData);
			}
		}

		private void EditSelectedWeaponAnimation()
		{
			if (lvWeaponAni.SelectedItems.Count <= 0) return;
			if (m_CurrentModelData == null) return;

			string szAniName="";
			if (lbWeaponAniName.SelectedIndex >= 0)
			{
				szAniName = (string)lbWeaponAniName.SelectedItem;
			}
			else
			{
				return;
			}

			int nLastAniSelIndex = lbWeaponAniName.SelectedIndex;

			ModelAniEdit dlgInput = new ModelAniEdit(ModelAniEdit.EditType.ET_EDIT_WEAPON);

			string szFileName = "";
			bool bLoop=false;
			bool bNewInsert = true;		// 사실은 애니메이션 새로 생성

			if (lvWeaponAni.SelectedItems[0].SubItems.Count > 1)
			{
				szFileName = lvWeaponAni.SelectedItems[0].SubItems[1].Text;
				if (lvWeaponAni.SelectedItems[0].SubItems[2].Text == "loop") bLoop=true;
				bNewInsert = false;
			}

			WeaponType nWeaponType = WeaponType.WT_NONE;
			for (WeaponType type=WeaponType.WT_1H_SLASH; type<=WeaponType.WT_GUNPOWDER; type++)
			{
				if (ConvertString.GetWeaponTypeStr(type) == lvWeaponAni.SelectedItems[0].Text)
				{
					nWeaponType = type;
					break;
				}
			}
			if (nWeaponType==WeaponType.WT_NONE) return;

			dlgInput.InitForEdit(m_CurrentModelData, szAniName, szFileName, nWeaponType, bLoop);

			dlgInput.ShowDialog();
			DialogResult res = dlgInput.DialogResult;
			if (res == DialogResult.OK)
			{
				string szNewAniName = dlgInput.m_szAniName;
				string szNewFileName = dlgInput.m_szFileName;
				bool bNewLoop = dlgInput.m_bLoop;

				if (szNewFileName.Length <= 0) return;

				AnimationLoopType nNewLoopType = AnimationLoopType.ALT_LASTFRAME;
				if (bNewLoop) nNewLoopType = AnimationLoopType.ALT_LOOP;

				// 실제 데이터 수정
				if (!bNewInsert)
				{
					m_CurrentModelData.EditAnimation(nWeaponType, szAniName, szNewAniName, szNewFileName, nNewLoopType);
				}
				else
				{
					m_CurrentModelData.InsertAnimation(nWeaponType, szNewAniName, szNewFileName, nNewLoopType);
				}

				
				SerializeModelAnimation(m_CurrentModelData);

				lbWeaponAniName.SelectedIndex = nLastAniSelIndex;
			}
		}

		private void InsertModelAnimation()
		{
			if (m_CurrentModelData == null) return;

			ModelAniEdit dlgInput = new ModelAniEdit(ModelAniEdit.EditType.ET_INSERT);
			dlgInput.InitForInsert(m_CurrentModelData);

			dlgInput.ShowDialog();
			DialogResult res = dlgInput.DialogResult;
			if (res == DialogResult.OK)
			{
				WeaponType nWeaponType = dlgInput.m_nWeaponType;
				string szAniName = dlgInput.m_szAniName;
				string szFileName = dlgInput.m_szFileName;
				bool bLoop = dlgInput.m_bLoop;
				AnimationLoopType nLoopType = AnimationLoopType.ALT_LASTFRAME;
				if (bLoop) nLoopType = AnimationLoopType.ALT_LOOP;

				//m_CurrentModelData.EditAnimation(WeaponType.WT_NONE, szAniName, szNewAniName, szNewFileName, nNewLoopType);
				m_CurrentModelData.InsertAnimation(nWeaponType, szAniName, szFileName, nLoopType);
				SerializeModelAnimation(m_CurrentModelData);

			}
		}

		private void SerializeModelTreeNode(ModelData model_data)
		{
			if (model_data==null) return;

			TreeNode tn=null;
			if (tvModel.SelectedNode.Text == model_data.m_szName)
			{
				tn=tvModel.SelectedNode;
			}
			else
			{
				IEnumerator tnEnum = tvModel.Nodes.GetEnumerator();
				while(tnEnum.MoveNext())
				{
					TreeNode node = (TreeNode)tnEnum;
					if (node.Text == model_data.m_szName)
					{
						tn=node;
						break;
					}
				}
			}

			if (tn == null) return;
			if (model_data.Edited) 
			{
				tn.ImageIndex = 2;
				tn.SelectedImageIndex = 2;
			}
			else 
			{
				tn.ImageIndex = 1;
				tn.SelectedImageIndex = 1;
			}

		}

		#endregion

		private void lvNormalAnimation_DoubleClick(object sender, System.EventArgs e)
		{
			EditSelectedNormalAnimation();
		}

		private void lvWeaponAni_DoubleClick(object sender, System.EventArgs e)
		{
			EditSelectedWeaponAnimation();
		}

		private void miEditNormalAni_Click(object sender, System.EventArgs e)
		{
			EditSelectedNormalAnimation();
		}

		private void miDelNormalAni_Click(object sender, System.EventArgs e)
		{
			if (lvNormalAnimation.SelectedItems.Count <= 0) return;
			if (m_CurrentModelData == null) return;

			if (MessageBox.Show("정말로 삭제하시겠습니까?", "Genie", MessageBoxButtons.OKCancel, MessageBoxIcon.Question)
				== DialogResult.OK)
			{
				string szAniName = lvNormalAnimation.SelectedItems[0].Text;

				m_CurrentModelData.RemoveAnimation(WeaponType.WT_NONE, szAniName);

				SerializeModelAnimation(m_CurrentModelData);
			}
		}

		private void miAbout_Click(object sender, System.EventArgs e)
		{
			AboutForm dlgInput = new AboutForm();
			dlgInput.ShowDialog();
		}

		private void miEditWeaponAni_Click(object sender, System.EventArgs e)
		{
			EditSelectedWeaponAnimation();
		}

		private void miRemoveWeaponAni_Click(object sender, System.EventArgs e)
		{
			if (lvWeaponAni.SelectedItems.Count <= 0) return;
			if (m_CurrentModelData == null) return;

			string szAniName="";
			if (lbWeaponAniName.SelectedIndex >= 0)
			{
				szAniName = (string)lbWeaponAniName.SelectedItem;
			}
			else
			{
				return;
			}

			int nLastAniSelIndex = lbWeaponAniName.SelectedIndex;
			int nLastWeaponAniCount = lbWeaponAniName.Items.Count;

			if (MessageBox.Show("정말로 삭제하시겠습니까?", "Genie", MessageBoxButtons.OKCancel, MessageBoxIcon.Question)
				== DialogResult.OK)
			{

				WeaponType nWeaponType = WeaponType.WT_NONE;
				for (WeaponType type=WeaponType.WT_1H_SLASH; type<=WeaponType.WT_GUNPOWDER; type++)
				{
					if (ConvertString.GetWeaponTypeStr(type) == lvWeaponAni.SelectedItems[0].Text)
					{
						nWeaponType = type;
						break;
					}
				}
				if (nWeaponType==WeaponType.WT_NONE) return;


				m_CurrentModelData.RemoveAnimation(nWeaponType, szAniName);

				SerializeModelAnimation(m_CurrentModelData);

				if (nLastWeaponAniCount == lbWeaponAniName.Items.Count)
				{
					lbWeaponAniName.SelectedIndex = nLastAniSelIndex;
				}
			}
		}

		/// <summary>
		/// 애니메이션 추가
		/// </summary>
		private void miAddNormalAni_Click(object sender, System.EventArgs e)
		{
			InsertModelAnimation();
		}

		private void miAddAnimation2_Click(object sender, System.EventArgs e)
		{
			InsertModelAnimation();
		}
	}
}
