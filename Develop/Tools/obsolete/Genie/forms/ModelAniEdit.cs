using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;

namespace Genie
{
	/// <summary>
	/// ModelAniEdit에 대한 요약 설명입니다.
	/// </summary>
	public class ModelAniEdit : System.Windows.Forms.Form
	{
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label lblModelName;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.ComboBox cbWeapon;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox tbFile;
		private System.Windows.Forms.Button btnOpenFile;
		private System.Windows.Forms.CheckBox cbLoop;
		private System.Windows.Forms.OpenFileDialog ofdAni;

		public enum EditType { ET_INSERT, ET_EDIT_NORMAL, ET_EDIT_WEAPON };


		private EditType	m_nEditType;
		public string		m_szAniName;
		public WeaponType	m_nWeaponType;
		public bool			m_bLoop;
		public string		m_szFileName;
		private System.Windows.Forms.TextBox tbAniName;
		private ModelData	m_ModelData;


		public ModelAniEdit(EditType nEditType)
		{
			//
			// Windows Form 디자이너 지원에 필요합니다.
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent를 호출한 다음 생성자 코드를 추가합니다.
			//
			m_nEditType = nEditType;
			m_szAniName = m_szFileName = "";
			m_bLoop = false;
			m_nWeaponType = WeaponType.WT_NONE;
			m_ModelData = null;
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
			this.btnOK = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.lblModelName = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.tbAniName = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.cbWeapon = new System.Windows.Forms.ComboBox();
			this.label4 = new System.Windows.Forms.Label();
			this.tbFile = new System.Windows.Forms.TextBox();
			this.ofdAni = new System.Windows.Forms.OpenFileDialog();
			this.btnOpenFile = new System.Windows.Forms.Button();
			this.cbLoop = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// btnOK
			// 
			this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOK.Location = new System.Drawing.Point(256, 176);
			this.btnOK.Name = "btnOK";
			this.btnOK.TabIndex = 6;
			this.btnOK.Text = "확인(&O)";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point(344, 176);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 7;
			this.btnCancel.Text = "취소(&C)";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 14);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(56, 23);
			this.label1.TabIndex = 6;
			this.label1.Text = "모델 : ";
			// 
			// lblModelName
			// 
			this.lblModelName.Location = new System.Drawing.Point(72, 13);
			this.lblModelName.Name = "lblModelName";
			this.lblModelName.TabIndex = 7;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(24, 48);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(104, 23);
			this.label2.TabIndex = 8;
			this.label2.Text = "애니메이션 이름 : ";
			// 
			// tbAniName
			// 
			this.tbAniName.Location = new System.Drawing.Point(136, 46);
			this.tbAniName.Name = "tbAniName";
			this.tbAniName.Size = new System.Drawing.Size(133, 21);
			this.tbAniName.TabIndex = 1;
			this.tbAniName.Text = "";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(24, 88);
			this.label3.Name = "label3";
			this.label3.TabIndex = 10;
			this.label3.Text = "무기타입 :";
			// 
			// cbWeapon
			// 
			this.cbWeapon.Location = new System.Drawing.Point(136, 88);
			this.cbWeapon.Name = "cbWeapon";
			this.cbWeapon.Size = new System.Drawing.Size(121, 20);
			this.cbWeapon.TabIndex = 2;
			this.cbWeapon.Text = "NONE";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(24, 128);
			this.label4.Name = "label4";
			this.label4.TabIndex = 12;
			this.label4.Text = "파일 : ";
			// 
			// tbFile
			// 
			this.tbFile.Enabled = false;
			this.tbFile.Location = new System.Drawing.Point(136, 128);
			this.tbFile.Name = "tbFile";
			this.tbFile.ReadOnly = true;
			this.tbFile.Size = new System.Drawing.Size(224, 21);
			this.tbFile.TabIndex = 4;
			this.tbFile.Text = "";
			// 
			// ofdAni
			// 
			this.ofdAni.DefaultExt = "ani";
			this.ofdAni.Filter = "애니메이션 파일(*.ani)|*.ani";
			this.ofdAni.Title = "애니메이션 파일 열기";
			// 
			// btnOpenFile
			// 
			this.btnOpenFile.Location = new System.Drawing.Point(376, 128);
			this.btnOpenFile.Name = "btnOpenFile";
			this.btnOpenFile.Size = new System.Drawing.Size(40, 23);
			this.btnOpenFile.TabIndex = 5;
			this.btnOpenFile.Text = "...";
			this.btnOpenFile.Click += new System.EventHandler(this.btnOpenFile_Click);
			// 
			// cbLoop
			// 
			this.cbLoop.Location = new System.Drawing.Point(296, 87);
			this.cbLoop.Name = "cbLoop";
			this.cbLoop.Size = new System.Drawing.Size(64, 24);
			this.cbLoop.TabIndex = 3;
			this.cbLoop.Text = "Loop";
			// 
			// ModelAniEdit
			// 
			this.AcceptButton = this.btnOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size(424, 205);
			this.Controls.Add(this.cbLoop);
			this.Controls.Add(this.btnOpenFile);
			this.Controls.Add(this.tbFile);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.cbWeapon);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.tbAniName);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.lblModelName);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.btnCancel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MinimizeBox = false;
			this.Name = "ModelAniEdit";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.ResumeLayout(false);

		}
		#endregion

		private void Serialize()
		{
			if (m_ModelData == null) return;

			if (m_nEditType == EditType.ET_INSERT) this.Text = "애니메이션 추가";
			else this.Text = "애니메이션 수정";

			cbWeapon.Items.Clear();
			for (WeaponType type = WeaponType.WT_NONE; type <= WeaponType.WT_GUNPOWDER; type++)
			{
				cbWeapon.Items.Add(ConvertString.GetWeaponTypeStr(type));
				if (m_nWeaponType == type) cbWeapon.SelectedIndex = (int)type;
			}

			if (m_nEditType == EditType.ET_INSERT)
			{
				cbWeapon.Enabled = true;
			}
			else
			{
				lblModelName.Text = m_ModelData.m_szName;
				tbAniName.Text = m_szAniName;
				cbLoop.Checked = m_bLoop;
				tbFile.Text = m_szFileName;
				cbWeapon.Enabled = false;

				if (m_nEditType == EditType.ET_EDIT_WEAPON)
				{
					tbAniName.Enabled = false;
				}
				else
				{
					tbAniName.Enabled = true;
				}
			}
		}

		public void InitForEdit(ModelData model_data, string szAniName, string szFilename, WeaponType nWeaponType, bool bLoop)
		{
			m_ModelData = model_data;
			m_szAniName = szAniName;
			m_szFileName = szFilename;
			m_nWeaponType = nWeaponType;
			m_bLoop = bLoop;

			Serialize();
		}

		public void InitForInsert(ModelData model_data)
		{
			m_ModelData = model_data;
			m_szAniName = "";
			m_szFileName = "";
			m_nWeaponType = WeaponType.WT_NONE;
			m_bLoop = false;

			Serialize();
		}

		private void btnOpenFile_Click(object sender, System.EventArgs e)
		{
			if (ofdAni.ShowDialog() == DialogResult.OK)
			{
				m_szFileName = Path.GetFileName(ofdAni.FileName);
				tbFile.Text = m_szFileName;
			}
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
			if (tbAniName.Text.Length <= 0)
			{
				MessageBox.Show("애니메이션 이름을 입력하지 않았습니다.");
				this.DialogResult = DialogResult.Cancel;
				return;
			}
			else if (tbFile.Text.Length <= 0)
			{
				MessageBox.Show("파일 이름을 입력하지 않았습니다.");
				this.DialogResult = DialogResult.Cancel;
				return;
			}

			m_szAniName = tbAniName.Text;
			m_bLoop = cbLoop.Checked;
			m_szFileName = tbFile.Text;
			m_nWeaponType = (WeaponType)cbWeapon.SelectedIndex;

			if (m_nEditType == EditType.ET_INSERT)
			{
				// 이미 같은 애니메이션이 있나 체크
				if (m_ModelData.FindAnimation(m_nWeaponType, m_szAniName) != null)
				{
					MessageBox.Show("이미 같은 이름의 애니메이션이 있습니다.");
					this.DialogResult = DialogResult.Cancel;
					return;
				}
			}
		}
	}
}
