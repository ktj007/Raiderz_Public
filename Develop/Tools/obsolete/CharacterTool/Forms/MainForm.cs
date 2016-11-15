using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using RSManaged;
using WeifenLuo.WinFormsUI;

namespace CharacterTool
{
	/// <summary>
	/// MainForm에 대한 요약 설명입니다.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		public RenderForm m_RenderForm = new RenderForm();
		public ProjectForm m_ProjectForm = new ProjectForm();
		public EditForm m_EditForm = new EditForm();
		public OutputForm m_OutputForm = new OutputForm();

		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.ToolBar toolBar1;
		private System.Windows.Forms.StatusBar statusBar;
		private DockManager dockManager;
		private System.Windows.Forms.MainMenu mainMenu;
		private System.Windows.Forms.MenuItem menuItem9;
		private System.Windows.Forms.MenuItem menuItem10;
		private System.Windows.Forms.MenuItem menuItemRenderView;
		private System.Windows.Forms.MenuItem menuItemProjectView;
		private System.Windows.Forms.MenuItem menuItemEditView;
		private System.Windows.Forms.MenuItem menuItemOutputView;
		private System.Windows.Forms.MenuItem menuItem11;
		private System.Windows.Forms.MenuItem menuItemViewGrid;
		private System.Windows.Forms.MenuItem menuItemNew;
		private System.Windows.Forms.MenuItem menuItemOpen;
		private System.Windows.Forms.MenuItem menuItemSave;
		private System.Windows.Forms.MenuItem menuItemSaveAs;
		private System.Windows.Forms.MenuItem menuItemQuit;
		private System.Windows.Forms.OpenFileDialog OpenFileDialogModel;
		private System.Windows.Forms.MenuItem miResetCamera;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem miAbout;
		private System.ComponentModel.IContainer components;

		public MainForm()
		{
			//
			// Windows Form 디자이너 지원에 필요합니다.
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent를 호출한 다음 생성자 코드를 추가합니다.
			//
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
			this.mainMenu = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItemNew = new System.Windows.Forms.MenuItem();
			this.menuItemOpen = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItemSave = new System.Windows.Forms.MenuItem();
			this.menuItemSaveAs = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.menuItemQuit = new System.Windows.Forms.MenuItem();
			this.menuItem9 = new System.Windows.Forms.MenuItem();
			this.menuItemProjectView = new System.Windows.Forms.MenuItem();
			this.menuItemEditView = new System.Windows.Forms.MenuItem();
			this.menuItemRenderView = new System.Windows.Forms.MenuItem();
			this.menuItemOutputView = new System.Windows.Forms.MenuItem();
			this.menuItem11 = new System.Windows.Forms.MenuItem();
			this.menuItemViewGrid = new System.Windows.Forms.MenuItem();
			this.menuItem10 = new System.Windows.Forms.MenuItem();
			this.dockManager = new WeifenLuo.WinFormsUI.DockManager();
			this.toolBar1 = new System.Windows.Forms.ToolBar();
			this.statusBar = new System.Windows.Forms.StatusBar();
			this.OpenFileDialogModel = new System.Windows.Forms.OpenFileDialog();
			this.miResetCamera = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.miAbout = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// mainMenu
			// 
			this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.menuItem1,
																					 this.menuItem9,
																					 this.menuItem2,
																					 this.menuItem10});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItemNew,
																					  this.menuItemOpen,
																					  this.menuItem4,
																					  this.menuItemSave,
																					  this.menuItemSaveAs,
																					  this.menuItem7,
																					  this.menuItemQuit});
			this.menuItem1.Text = "파일(&F)";
			// 
			// menuItemNew
			// 
			this.menuItemNew.Index = 0;
			this.menuItemNew.Text = "새로 만들기(&N)";
			// 
			// menuItemOpen
			// 
			this.menuItemOpen.Index = 1;
			this.menuItemOpen.Text = "열기(&O)...";
			this.menuItemOpen.Click += new System.EventHandler(this.menuItemOpen_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 2;
			this.menuItem4.Text = "-";
			// 
			// menuItemSave
			// 
			this.menuItemSave.Index = 3;
			this.menuItemSave.Text = "저장(&S)";
			// 
			// menuItemSaveAs
			// 
			this.menuItemSaveAs.Index = 4;
			this.menuItemSaveAs.Text = "다른 이름으로 저장(&A)";
			// 
			// menuItem7
			// 
			this.menuItem7.Index = 5;
			this.menuItem7.Text = "-";
			// 
			// menuItemQuit
			// 
			this.menuItemQuit.Index = 6;
			this.menuItemQuit.Text = "종료(&X)";
			this.menuItemQuit.Click += new System.EventHandler(this.menuItem8_Click);
			// 
			// menuItem9
			// 
			this.menuItem9.Index = 1;
			this.menuItem9.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItemProjectView,
																					  this.menuItemEditView,
																					  this.menuItemRenderView,
																					  this.menuItemOutputView,
																					  this.menuItem11,
																					  this.menuItemViewGrid,
																					  this.miResetCamera});
			this.menuItem9.Text = "보기(&V)";
			// 
			// menuItemProjectView
			// 
			this.menuItemProjectView.Index = 0;
			this.menuItemProjectView.Shortcut = System.Windows.Forms.Shortcut.Alt1;
			this.menuItemProjectView.Text = "프로젝트 창";
			this.menuItemProjectView.Click += new System.EventHandler(this.menuItemProject_Click);
			// 
			// menuItemEditView
			// 
			this.menuItemEditView.Index = 1;
			this.menuItemEditView.Shortcut = System.Windows.Forms.Shortcut.Alt2;
			this.menuItemEditView.Text = "편집 창";
			this.menuItemEditView.Click += new System.EventHandler(this.menuItemEditView_Click);
			// 
			// menuItemRenderView
			// 
			this.menuItemRenderView.Index = 2;
			this.menuItemRenderView.Shortcut = System.Windows.Forms.Shortcut.Alt2;
			this.menuItemRenderView.Text = "RenderView";
			this.menuItemRenderView.Click += new System.EventHandler(this.menuItemRenderView_Click);
			// 
			// menuItemOutputView
			// 
			this.menuItemOutputView.Index = 3;
			this.menuItemOutputView.Shortcut = System.Windows.Forms.Shortcut.Alt4;
			this.menuItemOutputView.Text = "출력 창";
			this.menuItemOutputView.Click += new System.EventHandler(this.menuItemOutputView_Click);
			// 
			// menuItem11
			// 
			this.menuItem11.Index = 4;
			this.menuItem11.Text = "-";
			// 
			// menuItemViewGrid
			// 
			this.menuItemViewGrid.Index = 5;
			this.menuItemViewGrid.Shortcut = System.Windows.Forms.Shortcut.Ctrl1;
			this.menuItemViewGrid.Text = "격자";
			this.menuItemViewGrid.Click += new System.EventHandler(this.menuItemViewGrid_Click);
			// 
			// menuItem10
			// 
			this.menuItem10.Index = 3;
			this.menuItem10.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					   this.miAbout});
			this.menuItem10.Text = "도움말(&H)";
			// 
			// dockManager
			// 
			this.dockManager.ActiveAutoHideContent = null;
			this.dockManager.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dockManager.Location = new System.Drawing.Point(0, 28);
			this.dockManager.Name = "dockManager";
			this.dockManager.Size = new System.Drawing.Size(916, 781);
			this.dockManager.TabIndex = 1;
			this.dockManager.ContentRemoved += new WeifenLuo.WinFormsUI.DockManager.ContentEventHandler(this.dockManager_ContentRemoved);
			this.dockManager.ActiveDocumentChanged += new System.EventHandler(this.dockManager_ActiveDocumentChanged);
			this.dockManager.ContentAdded += new WeifenLuo.WinFormsUI.DockManager.ContentEventHandler(this.dockManager_ContentAdded);
			// 
			// toolBar1
			// 
			this.toolBar1.ButtonSize = new System.Drawing.Size(23, 22);
			this.toolBar1.DropDownArrows = true;
			this.toolBar1.Location = new System.Drawing.Point(0, 0);
			this.toolBar1.Name = "toolBar1";
			this.toolBar1.ShowToolTips = true;
			this.toolBar1.Size = new System.Drawing.Size(916, 28);
			this.toolBar1.TabIndex = 1;
			// 
			// statusBar
			// 
			this.statusBar.Location = new System.Drawing.Point(0, 809);
			this.statusBar.Name = "statusBar";
			this.statusBar.ShowPanels = true;
			this.statusBar.Size = new System.Drawing.Size(916, 24);
			this.statusBar.TabIndex = 2;
			// 
			// miResetCamera
			// 
			this.miResetCamera.Index = 6;
			this.miResetCamera.Shortcut = System.Windows.Forms.Shortcut.F5;
			this.miResetCamera.Text = "카메라 초기화";
			this.miResetCamera.Click += new System.EventHandler(this.miResetCamera_Click);
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 2;
			this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem3});
			this.menuItem2.Text = "도구(&T)";
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 0;
			this.menuItem3.Text = "옵션(&O)...";
			this.menuItem3.Click += new System.EventHandler(this.menuItem3_Click);
			// 
			// miAbout
			// 
			this.miAbout.Index = 0;
			this.miAbout.Text = "정보...";
			this.miAbout.Click += new System.EventHandler(this.miAbout_Click);
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(916, 833);
			this.Controls.Add(this.dockManager);
			this.Controls.Add(this.statusBar);
			this.Controls.Add(this.toolBar1);
			this.IsMdiContainer = true;
			this.Menu = this.mainMenu;
			this.Name = "MainForm";
			this.Text = "CharacterTool";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyDown);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MainForm_Closing);
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.Closed += new System.EventHandler(this.MainForm_Closed);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// 해당 응용 프로그램의 주 진입점입니다.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			MFramework.Init();
			Invalidator.Init();
			ProjectData.Init();

			Global.mainform = new MainForm();

			// Application 이벤트 연결
			Application.Idle += new EventHandler(Global.mainform.App_Idle);
			Application.Run(Global.mainform);

			ProjectData.Final();
			Invalidator.Final();
			MFramework.Final();
		}

		void App_Idle(object sender, EventArgs e)
		{
			if (this.Visible &&
				this.WindowState != FormWindowState.Minimized && 
				Form.ActiveForm == this)
			{
				Invalidator.Instance.Update(true);
			}
		}

		private void menuItem8_Click(object sender, System.EventArgs e)
		{
			Application.Exit();
		}

		private void MainForm_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
#if (DEBUG)
			if (e.KeyCode == Keys.Escape)
			{
				Application.Exit();
			}
#endif

			if (sender is MainForm)
			{
				MessageBox.Show("AAA");
			}
		}

		private void MainForm_Load(object sender, System.EventArgs e)
		{
			string configFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "UI.config");

			if (File.Exists(configFile))
			{
				dockManager.LoadFromXml(configFile, new GetContentCallback(GetContentFromPersistString));
			}
			else
			{
				// 기본값
				m_RenderForm.Show(dockManager);
				m_ProjectForm.Show(dockManager);
				m_EditForm.Show(dockManager);
				m_OutputForm.Show(dockManager);
				m_ProjectForm.Activate();
			}

			RefreshMenuItemCheck();

			OutputText.Print("프로그램 초기화가 완료되었습니다.");
		}

		private void MainForm_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			string configFile = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "UI.config");
			dockManager.SaveAsXml(configFile);
		}

		private void MainForm_Closed(object sender, System.EventArgs e)
		{
			
		}

		private void dockManager_ActiveDocumentChanged(object sender, System.EventArgs e)
		{
//			if (m_options.ActiveDocumentChanged)
//			{
//				string text = "Event: ActiveDocumentChanged.\n";
//				if (dockManager.ActiveDocument != null)
//					text += "ActiveDocument.Text = " + dockManager.ActiveDocument.Text;
//				else
//					text += "ActiveDocument = (null)";
//				MessageBox.Show(text);
//			}
		}
		private void dockManager_ContentAdded(object sender, WeifenLuo.WinFormsUI.ContentEventArgs e)
		{
//			if (m_options.ContentAdded)
//			{
//				string text = "Event: ContentAdded.\n";
//				text += "Content.Text = " + e.Content.Text;
//				MessageBox.Show(text);
//			}
		}
		private void dockManager_ContentRemoved(object sender, WeifenLuo.WinFormsUI.ContentEventArgs e)
		{
//			if (m_options.ContentRemoved)
//			{
//				string text = "Event: ContentRemoved.\n";
//				text += "Content.Text = " + e.Content.Text;
//				MessageBox.Show(text);
//			}
		}

		private Content FindContent(string text)
		{
			Content[] documents = dockManager.Documents;

			foreach (Content c in documents)
			{
				if (c.Text == text) return c;
			}

			return null;
		}

		private void menuItemRenderView_Click(object sender, System.EventArgs e)
		{
			m_RenderForm.Show(dockManager);

//			RenderForm renderForm = new RenderForm();
//
//			int count = 1;
//			string text = "Document" + count.ToString();
//			while (FindContent(text) != null)
//			{
//				count++;
//				text = "Document" + count.ToString();
//			}
//			renderForm.Text = text;
//			renderForm.Show(dockManager);
		}

		private void menuItemProject_Click(object sender, System.EventArgs e)
		{
			m_ProjectForm.Show(dockManager);
		}

		private void menuItemEditView_Click(object sender, System.EventArgs e)
		{
			m_EditForm.Show(dockManager);
		}

		private void menuItemOutputView_Click(object sender, System.EventArgs e)
		{
			m_OutputForm.Show(dockManager);
		}

		private Content GetContentFromPersistString(string persistString)
		{
			if (persistString == typeof(RenderForm).ToString())
				return m_RenderForm;
			else if (persistString == typeof(ProjectForm).ToString())
				return m_ProjectForm;
			else if (persistString == typeof(EditForm).ToString())
				return m_EditForm;
			else if (persistString == typeof(OutputForm).ToString())
				return m_OutputForm;
			else
				return null;
		}

		private void menuItemViewGrid_Click(object sender, System.EventArgs e)
		{
			Global.renderer.IsDrawGrid = !Global.renderer.IsDrawGrid;
			menuItemViewGrid.Checked = Global.renderer.IsDrawGrid;
		}

		private void RefreshMenuItemCheck()
		{
			menuItemViewGrid.Checked = Global.renderer.IsDrawGrid;
		}


		private void menuItemOpen_Click(object sender, System.EventArgs e)
		{
			CToolHelper.OpenFileDlg(this);
		}

		private void miResetCamera_Click(object sender, System.EventArgs e)
		{
			Global.renderer.ResetCamera();
		}

		private void menuItem3_Click(object sender, System.EventArgs e)
		{
			OptionForm dlgOption = new OptionForm();
			dlgOption.ShowDialog();
			DialogResult res = dlgOption.DialogResult;
			if (res == DialogResult.OK)
			{
				MessageBox.Show("확인");

			}
		}

		private void miAbout_Click(object sender, System.EventArgs e)
		{
			AboutForm dlgAbout = new AboutForm();
			dlgAbout.ShowDialog();
		}
	}
}
