using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using RSManaged;

namespace CharacterTool
{
	public class RenderForm : WeifenLuo.WinFormsUI.Content
	{
		public System.Windows.Forms.Panel pnlRender;
		private CharacterTool.RenderPanel ctlRenderPanel;	// 동적생성
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.ToolBarButton toolBarButton1;
		private System.Windows.Forms.ImageList imageListRenderView;
		private System.Windows.Forms.ToolBar toolBarRender;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.TrackBar trackBar1;
		private System.ComponentModel.IContainer components = null;

		public RenderForm()
		{
			// 이 호출은 Windows Form 디자이너에 필요합니다.
			InitializeComponent();

			// TODO: InitializeComponent를 호출한 다음 초기화 작업을 추가합니다.
		}

		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				Invalidator.Instance.UnregisterControl(ctlRenderPanel);
				this.ctlRenderPanel = null;

				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region 디자이너에서 생성한 코드
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(RenderForm));
			this.toolBarRender = new System.Windows.Forms.ToolBar();
			this.toolBarButton1 = new System.Windows.Forms.ToolBarButton();
			this.imageListRenderView = new System.Windows.Forms.ImageList(this.components);
			this.pnlRender = new System.Windows.Forms.Panel();
			this.button1 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.trackBar1 = new System.Windows.Forms.TrackBar();
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
			this.SuspendLayout();
			// 
			// toolBarRender
			// 
			this.toolBarRender.Appearance = System.Windows.Forms.ToolBarAppearance.Flat;
			this.toolBarRender.AutoSize = false;
			this.toolBarRender.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
																							 this.toolBarButton1});
			this.toolBarRender.ButtonSize = new System.Drawing.Size(18, 18);
			this.toolBarRender.DropDownArrows = true;
			this.toolBarRender.ImageList = this.imageListRenderView;
			this.toolBarRender.Location = new System.Drawing.Point(0, 3);
			this.toolBarRender.Name = "toolBarRender";
			this.toolBarRender.ShowToolTips = true;
			this.toolBarRender.Size = new System.Drawing.Size(664, 32);
			this.toolBarRender.TabIndex = 0;
			// 
			// toolBarButton1
			// 
			this.toolBarButton1.ImageIndex = 0;
			this.toolBarButton1.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
			this.toolBarButton1.ToolTipText = "카메라";
			// 
			// imageListRenderView
			// 
			this.imageListRenderView.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
			this.imageListRenderView.ImageSize = new System.Drawing.Size(24, 24);
			this.imageListRenderView.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListRenderView.ImageStream")));
			this.imageListRenderView.TransparentColor = System.Drawing.Color.Fuchsia;
			// 
			// pnlRender
			// 
			this.pnlRender.Location = new System.Drawing.Point(8, 48);
			this.pnlRender.Name = "pnlRender";
			this.pnlRender.Size = new System.Drawing.Size(648, 480);
			this.pnlRender.TabIndex = 1;
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(8, 536);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(80, 24);
			this.button1.TabIndex = 2;
			this.button1.Text = "button1";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(96, 536);
			this.button2.Name = "button2";
			this.button2.TabIndex = 3;
			this.button2.Text = "button2";
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// trackBar1
			// 
			this.trackBar1.LargeChange = 4;
			this.trackBar1.Location = new System.Drawing.Point(216, 528);
			this.trackBar1.Maximum = 30;
			this.trackBar1.Name = "trackBar1";
			this.trackBar1.Size = new System.Drawing.Size(240, 42);
			this.trackBar1.TabIndex = 4;
			this.trackBar1.TickStyle = System.Windows.Forms.TickStyle.Both;
			// 
			// RenderForm
			// 
			this.AllowedStates = WeifenLuo.WinFormsUI.ContentStates.Document;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(664, 629);
			this.CloseButton = false;
			this.Controls.Add(this.trackBar1);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.pnlRender);
			this.Controls.Add(this.toolBarRender);
			this.DockPadding.Bottom = 3;
			this.DockPadding.Top = 3;
			this.KeyPreview = true;
			this.Name = "RenderForm";
			this.Text = "RenderView";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.RenderForm_KeyDown);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.RenderForm_MouseDown);
			this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RenderForm_KeyPress);
			this.Load += new System.EventHandler(this.RenderForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion


		private void button1_Click(object sender, System.EventArgs e)
		{
			string file = "D:/My Projects/Maiet/SoulHunt/Develop/Game/Runtime/Data/Model/Monster/Skeleton/Skeleton01.elu";
			// field
			MAABBTreeSceneNode fieldNode = new MAABBTreeSceneNode();
			if (fieldNode.Open())
			{
				MFramework.Instance.SceneManager.AddChild(fieldNode);
			}

//			MFramework.Instance.SceneManager.AttachAABBTreeNode(fieldNode);

		}

		private void RenderForm_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.W)
			{
			}
		}

		private void RenderForm_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			switch (e.KeyChar)
			{
				case 's':
				break;
			}

		}

		private void RenderForm_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			
		}


		private void button2_Click(object sender, System.EventArgs e)
		{
			if (MFramework.Instance.MeshMgr.LoadXmlList("../../../Game/Runtime/Data/Model/CharacterObjectList.xml") == 0)
			{
				MessageBox.Show("파일이 없습니다.");
			}

			MMesh mesh = MFramework.Instance.MeshMgr.Get("human_male");
			if (mesh.Valid)
			{
				
//				MSceneNode node = Global.actor.Create(mesh);
//				if (node.Valid)
//				{
//					MFramework.Instance.SceneManager.AddChild(node);
//
//					Global.actor.SetAnimation(RSManaged.RAniPartsMode.AMode_All, "idle");
//
//					OutputText.Print("로드가 완료되었습니다.");
//				}
				
			}
		}

		private void RenderForm_Load(object sender, System.EventArgs e)
		{
			this.ctlRenderPanel = new CharacterTool.RenderPanel();
			// 
			// ctlRenderPanel
			// 
			this.ctlRenderPanel.BackColor = System.Drawing.SystemColors.AppWorkspace;
			this.ctlRenderPanel.Location = new System.Drawing.Point(24, 56);
			this.ctlRenderPanel.Name = "ctlRenderPanel";
			this.ctlRenderPanel.Size = new System.Drawing.Size(560, 404);
			this.ctlRenderPanel.TabIndex = 0;

			pnlRender.Controls.Add(this.ctlRenderPanel);
			//this.Controls.Add(this.ctlRenderPanel);

			ctlRenderPanel.InitRenderer();

			Invalidator.Instance.RegisterControl(ctlRenderPanel);
		}

	}
}

