using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace CharacterTool
{
	public class ProjectForm : WeifenLuo.WinFormsUI.Content
	{
		private System.Windows.Forms.ToolBar toolBar1;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.TreeView tvProject;
		private System.ComponentModel.IContainer components = null;

		public ProjectForm()
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
			this.toolBar1 = new System.Windows.Forms.ToolBar();
			this.tvProject = new System.Windows.Forms.TreeView();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.SuspendLayout();
			// 
			// toolBar1
			// 
			this.toolBar1.ButtonSize = new System.Drawing.Size(24, 24);
			this.toolBar1.DropDownArrows = true;
			this.toolBar1.Location = new System.Drawing.Point(0, 25);
			this.toolBar1.Name = "toolBar1";
			this.toolBar1.ShowToolTips = true;
			this.toolBar1.Size = new System.Drawing.Size(184, 30);
			this.toolBar1.TabIndex = 0;
			// 
			// tvProject
			// 
			this.tvProject.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tvProject.ImageIndex = -1;
			this.tvProject.Location = new System.Drawing.Point(0, 55);
			this.tvProject.Name = "tvProject";
			this.tvProject.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
																				  new System.Windows.Forms.TreeNode("CharacterModel", new System.Windows.Forms.TreeNode[] {
																																											  new System.Windows.Forms.TreeNode("Mesh"),
																																											  new System.Windows.Forms.TreeNode("Animation", new System.Windows.Forms.TreeNode[] {
																																																																	 new System.Windows.Forms.TreeNode("idle"),
																																																																	 new System.Windows.Forms.TreeNode("run"),
																																																																	 new System.Windows.Forms.TreeNode("run_back"),
																																																																	 new System.Windows.Forms.TreeNode("run_left"),
																																																																	 new System.Windows.Forms.TreeNode("run_right"),
																																																																	 new System.Windows.Forms.TreeNode("jump")}),
																																											  new System.Windows.Forms.TreeNode("Motion")})});
			this.tvProject.SelectedImageIndex = -1;
			this.tvProject.Size = new System.Drawing.Size(184, 228);
			this.tvProject.TabIndex = 1;
			this.tvProject.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvProject_AfterSelect);
			// 
			// imageList1
			// 
			this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// ProjectForm
			// 
			this.AllowedStates = ((WeifenLuo.WinFormsUI.ContentStates)(((((WeifenLuo.WinFormsUI.ContentStates.Float | WeifenLuo.WinFormsUI.ContentStates.DockLeft) 
				| WeifenLuo.WinFormsUI.ContentStates.DockRight) 
				| WeifenLuo.WinFormsUI.ContentStates.DockTop) 
				| WeifenLuo.WinFormsUI.ContentStates.DockBottom)));
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(184, 285);
			this.Controls.Add(this.tvProject);
			this.Controls.Add(this.toolBar1);
			this.DockPadding.Bottom = 2;
			this.DockPadding.Top = 25;
			this.Name = "ProjectForm";
			this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockLeft;
			this.Text = "프로젝트";
			this.ResumeLayout(false);

		}
		#endregion

		private void tvProject_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
		
		}
	}
}

