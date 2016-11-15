using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace CharacterTool
{
	public class EditForm : WeifenLuo.WinFormsUI.Content
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.PropertyGrid propertyGridChar;
		private System.ComponentModel.IContainer components = null;

		public EditForm()
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
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.propertyGridChar = new System.Windows.Forms.PropertyGrid();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox1.Controls.Add(this.propertyGridChar);
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(177, 360);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "속성";
			// 
			// propertyGridChar
			// 
			this.propertyGridChar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.propertyGridChar.CommandsVisibleIfAvailable = true;
			this.propertyGridChar.LargeButtons = false;
			this.propertyGridChar.LineColor = System.Drawing.SystemColors.ScrollBar;
			this.propertyGridChar.Location = new System.Drawing.Point(17, 21);
			this.propertyGridChar.Name = "propertyGridChar";
			this.propertyGridChar.Size = new System.Drawing.Size(144, 331);
			this.propertyGridChar.TabIndex = 0;
			this.propertyGridChar.Text = "PropertyGrid";
			this.propertyGridChar.ViewBackColor = System.Drawing.SystemColors.Window;
			this.propertyGridChar.ViewForeColor = System.Drawing.SystemColors.WindowText;
			// 
			// EditForm
			// 
			this.AllowedStates = ((WeifenLuo.WinFormsUI.ContentStates)(((((WeifenLuo.WinFormsUI.ContentStates.Float | WeifenLuo.WinFormsUI.ContentStates.DockLeft) 
				| WeifenLuo.WinFormsUI.ContentStates.DockRight) 
				| WeifenLuo.WinFormsUI.ContentStates.DockTop) 
				| WeifenLuo.WinFormsUI.ContentStates.DockBottom)));
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(192, 645);
			this.Controls.Add(this.groupBox1);
			this.DockPadding.Left = 3;
			this.DockPadding.Right = 3;
			this.HideOnClose = true;
			this.Name = "EditForm";
			this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockRight;
			this.Text = "편집";
			this.Load += new System.EventHandler(this.EditForm_Load);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void EditForm_Load(object sender, System.EventArgs e)
		{
			CharProperty charProperty = new CharProperty();
			propertyGridChar.SelectedObject = charProperty;
		}
	}
}

