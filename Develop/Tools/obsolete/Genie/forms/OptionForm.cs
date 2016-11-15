using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;

namespace Genie
{
	/// <summary>
	/// OptionForm에 대한 요약 설명입니다.
	/// </summary>
	public class OptionForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button btnSetFolder;
		private System.Windows.Forms.Button btnOptionCancel;
		private System.Windows.Forms.Button btnOptionOK;
		private System.Windows.Forms.TextBox tbFolder;
		private System.Windows.Forms.FolderBrowserDialog fbdRootPath;
		private System.Windows.Forms.Button btnSetDefault;
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public OptionForm()
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
				if(components != null)
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
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.btnSetFolder = new System.Windows.Forms.Button();
			this.tbFolder = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.btnOptionCancel = new System.Windows.Forms.Button();
			this.btnOptionOK = new System.Windows.Forms.Button();
			this.fbdRootPath = new System.Windows.Forms.FolderBrowserDialog();
			this.btnSetDefault = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btnSetFolder);
			this.groupBox1.Controls.Add(this.tbFolder);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Location = new System.Drawing.Point(10, 14);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(540, 66);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "일반";
			// 
			// btnSetFolder
			// 
			this.btnSetFolder.Location = new System.Drawing.Point(484, 18);
			this.btnSetFolder.Name = "btnSetFolder";
			this.btnSetFolder.Size = new System.Drawing.Size(42, 24);
			this.btnSetFolder.TabIndex = 0;
			this.btnSetFolder.Text = "...";
			this.btnSetFolder.Click += new System.EventHandler(this.btnSetFolder_Click);
			// 
			// tbFolder
			// 
			this.tbFolder.Enabled = false;
			this.tbFolder.Location = new System.Drawing.Point(97, 20);
			this.tbFolder.Name = "tbFolder";
			this.tbFolder.ReadOnly = true;
			this.tbFolder.Size = new System.Drawing.Size(378, 21);
			this.tbFolder.TabIndex = 3;
			this.tbFolder.Text = "";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 23);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(80, 24);
			this.label1.TabIndex = 0;
			this.label1.Text = "루트폴더 :";
			// 
			// btnOptionCancel
			// 
			this.btnOptionCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnOptionCancel.Location = new System.Drawing.Point(474, 88);
			this.btnOptionCancel.Name = "btnOptionCancel";
			this.btnOptionCancel.TabIndex = 2;
			this.btnOptionCancel.Text = "취소";
			// 
			// btnOptionOK
			// 
			this.btnOptionOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOptionOK.Location = new System.Drawing.Point(394, 88);
			this.btnOptionOK.Name = "btnOptionOK";
			this.btnOptionOK.TabIndex = 1;
			this.btnOptionOK.Text = "확인";
			this.btnOptionOK.Click += new System.EventHandler(this.btnOptionOK_Click);
			// 
			// fbdRootPath
			// 
			this.fbdRootPath.SelectedPath = ".";
			// 
			// btnSetDefault
			// 
			this.btnSetDefault.Location = new System.Drawing.Point(312, 88);
			this.btnSetDefault.Name = "btnSetDefault";
			this.btnSetDefault.TabIndex = 3;
			this.btnSetDefault.Text = "초기화";
			this.btnSetDefault.Click += new System.EventHandler(this.btnSetDefault_Click);
			// 
			// OptionForm
			// 
			this.AcceptButton = this.btnOptionOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.CancelButton = this.btnOptionCancel;
			this.ClientSize = new System.Drawing.Size(560, 125);
			this.Controls.Add(this.btnSetDefault);
			this.Controls.Add(this.btnOptionOK);
			this.Controls.Add(this.btnOptionCancel);
			this.Controls.Add(this.groupBox1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "OptionForm";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "옵션";
			this.Load += new System.EventHandler(this.OptionForm_Load);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void OptionForm_Load(object sender, System.EventArgs e)
		{
			Serialize();
		}

		private void btnSetFolder_Click(object sender, System.EventArgs e)
		{
			//fbdRootPath.RootFolder = Application.StartupPath;
			if (fbdRootPath.ShowDialog() == DialogResult.OK)
			{
				tbFolder.Text = fbdRootPath.SelectedPath;
			}
		}

		private void btnOptionOK_Click(object sender, System.EventArgs e)
		{
			if (Global.szRootPath.Length <= 0) return;

			Global.szRootPath = tbFolder.Text;

			char last_char = Global.szRootPath[Global.szRootPath.Length-1];

			if ((last_char != '/') && (last_char != '\\')) Global.szRootPath = Global.szRootPath + "/";
			Global.szRootPath = Path.GetFullPath(Global.szRootPath);
		}

		private void Serialize()
		{
			tbFolder.Text = Global.szRootPath;
		}

		private void btnSetDefault_Click(object sender, System.EventArgs e)
		{
			Global.szRootPath = "../../";
			Serialize();
		}
	}
}
