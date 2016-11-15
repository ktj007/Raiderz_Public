using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Genie
{
	/// <summary>
	/// ConstDataInputForm에 대한 요약 설명입니다.
	/// </summary>
	public class ConstDataInputForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblKey;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnOK;
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private string m_szKey;
		private string m_szValue;
		private string m_szDesc;
		private System.Windows.Forms.NumericUpDown intEdit;
		private System.Windows.Forms.TextBox tbDesc;
		private System.Windows.Forms.TextBox tbValue;
		private System.Windows.Forms.TextBox tbValue3;
		private System.Windows.Forms.TextBox tbValue4;
		private System.Windows.Forms.CheckBox cbValue;
		private System.Windows.Forms.TextBox tbValue2;
		private System.Windows.Forms.OpenFileDialog ofdFile;
		private System.Windows.Forms.Button btnOpenFileDlg;
		private ValueType m_nValueType;

		public ConstDataInputForm(string szKey, string szValue, string szDesc, ValueType nValueType)
		{
			m_szKey = szKey;
			m_szValue = szValue;
			m_szDesc = szDesc;
			m_nValueType = nValueType;

			//
			// Windows Form 디자이너 지원에 필요합니다.
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent를 호출한 다음 생성자 코드를 추가합니다.
			//

			Serialize();
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ConstDataInputForm));
			this.lblKey = new System.Windows.Forms.Label();
			this.intEdit = new System.Windows.Forms.NumericUpDown();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnOK = new System.Windows.Forms.Button();
			this.tbDesc = new System.Windows.Forms.TextBox();
			this.tbValue = new System.Windows.Forms.TextBox();
			this.tbValue2 = new System.Windows.Forms.TextBox();
			this.tbValue3 = new System.Windows.Forms.TextBox();
			this.tbValue4 = new System.Windows.Forms.TextBox();
			this.cbValue = new System.Windows.Forms.CheckBox();
			this.ofdFile = new System.Windows.Forms.OpenFileDialog();
			this.btnOpenFileDlg = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.intEdit)).BeginInit();
			this.SuspendLayout();
			// 
			// lblKey
			// 
			this.lblKey.Location = new System.Drawing.Point(16, 16);
			this.lblKey.Name = "lblKey";
			this.lblKey.Size = new System.Drawing.Size(100, 16);
			this.lblKey.TabIndex = 0;
			this.lblKey.Text = "Key";
			// 
			// intEdit
			// 
			this.intEdit.Location = new System.Drawing.Point(147, 14);
			this.intEdit.Maximum = new System.Decimal(new int[] {
																	2000000000,
																	0,
																	0,
																	0});
			this.intEdit.Minimum = new System.Decimal(new int[] {
																	2000000000,
																	0,
																	0,
																	-2147483648});
			this.intEdit.Name = "intEdit";
			this.intEdit.Size = new System.Drawing.Size(88, 21);
			this.intEdit.TabIndex = 1;
			// 
			// btnCancel
			// 
			this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point(344, 112);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 2;
			this.btnCancel.Text = "취소(&C)";
			// 
			// btnOK
			// 
			this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOK.Location = new System.Drawing.Point(256, 112);
			this.btnOK.Name = "btnOK";
			this.btnOK.TabIndex = 3;
			this.btnOK.Text = "확인(&O)";
			this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
			// 
			// tbDesc
			// 
			this.tbDesc.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tbDesc.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.tbDesc.Enabled = false;
			this.tbDesc.ForeColor = System.Drawing.SystemColors.WindowText;
			this.tbDesc.Location = new System.Drawing.Point(11, 47);
			this.tbDesc.Multiline = true;
			this.tbDesc.Name = "tbDesc";
			this.tbDesc.ReadOnly = true;
			this.tbDesc.Size = new System.Drawing.Size(400, 48);
			this.tbDesc.TabIndex = 4;
			this.tbDesc.Text = "";
			// 
			// tbValue
			// 
			this.tbValue.Location = new System.Drawing.Point(256, 14);
			this.tbValue.Name = "tbValue";
			this.tbValue.Size = new System.Drawing.Size(56, 21);
			this.tbValue.TabIndex = 5;
			this.tbValue.Text = "";
			// 
			// tbValue2
			// 
			this.tbValue2.Location = new System.Drawing.Point(32, 56);
			this.tbValue2.Name = "tbValue2";
			this.tbValue2.Size = new System.Drawing.Size(56, 21);
			this.tbValue2.TabIndex = 6;
			this.tbValue2.Text = "";
			// 
			// tbValue3
			// 
			this.tbValue3.Location = new System.Drawing.Point(104, 56);
			this.tbValue3.Name = "tbValue3";
			this.tbValue3.Size = new System.Drawing.Size(56, 21);
			this.tbValue3.TabIndex = 7;
			this.tbValue3.Text = "";
			// 
			// tbValue4
			// 
			this.tbValue4.Location = new System.Drawing.Point(168, 56);
			this.tbValue4.Name = "tbValue4";
			this.tbValue4.Size = new System.Drawing.Size(56, 21);
			this.tbValue4.TabIndex = 8;
			this.tbValue4.Text = "";
			// 
			// cbValue
			// 
			this.cbValue.Location = new System.Drawing.Point(16, 104);
			this.cbValue.Name = "cbValue";
			this.cbValue.Size = new System.Drawing.Size(24, 24);
			this.cbValue.TabIndex = 9;
			// 
			// ofdFile
			// 
			this.ofdFile.Filter = "All Files(*.*)|*.*";
			// 
			// btnOpenFileDlg
			// 
			this.btnOpenFileDlg.Location = new System.Drawing.Point(369, 13);
			this.btnOpenFileDlg.Name = "btnOpenFileDlg";
			this.btnOpenFileDlg.Size = new System.Drawing.Size(31, 23);
			this.btnOpenFileDlg.TabIndex = 10;
			this.btnOpenFileDlg.Text = "...";
			this.btnOpenFileDlg.Click += new System.EventHandler(this.btnOpenFileDlg_Click);
			// 
			// ConstDataInputForm
			// 
			this.AcceptButton = this.btnOK;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.CancelButton = this.btnCancel;
			this.ClientSize = new System.Drawing.Size(424, 141);
			this.Controls.Add(this.btnOpenFileDlg);
			this.Controls.Add(this.cbValue);
			this.Controls.Add(this.tbValue4);
			this.Controls.Add(this.tbValue3);
			this.Controls.Add(this.tbValue2);
			this.Controls.Add(this.tbValue);
			this.Controls.Add(this.tbDesc);
			this.Controls.Add(this.btnOK);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.intEdit);
			this.Controls.Add(this.lblKey);
			this.ForeColor = System.Drawing.SystemColors.ControlText;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ConstDataInputForm";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			((System.ComponentModel.ISupportInitialize)(this.intEdit)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void Serialize()
		{
			lblKey.Text = m_szKey;
			lblKey.AutoSize = true;
			tbDesc.Text = m_szDesc;

			intEdit.Visible = false;
			tbValue.Visible = false;
			tbValue2.Visible = false;
			tbValue3.Visible = false;
			tbValue4.Visible = false;
			cbValue.Visible = false;
			btnOpenFileDlg.Visible = false;
			this.Width = 432;

			int y = lblKey.Top;
			intEdit.Top = y;
			cbValue.Top = y - 5;
			tbValue.Top = y;
			tbValue2.Top = y;
			tbValue3.Top = y;
			tbValue4.Top = y;
			


			switch (m_nValueType)
			{
			case ValueType.VT_INTEGER:
				{
					intEdit.Visible = true;
					intEdit.Left = lblKey.Left + lblKey.Width + 10;
					intEdit.Value = Convert.ToInt32(m_szValue);
				}
				break;
			case ValueType.VT_FLOAT:
				{
					tbValue.Visible = true;
					tbValue.Left = lblKey.Left + lblKey.Width + 10;
					tbValue.Width = 80;
					tbValue.Text = m_szValue;
				}
				break;
			case ValueType.VT_STRING:
				{
					tbValue.Visible = true;
					tbValue.Left = lblKey.Left + lblKey.Width + 10;
					tbValue.Width = 150;
					tbValue.Text = m_szValue;
				}
				break;
			case ValueType.VT_PATH:
				{
					tbValue.Visible = true;
					tbValue.Left = lblKey.Left + lblKey.Width + 10;
					tbValue.Width = 310;
					tbValue.Text = m_szValue;

					btnOpenFileDlg.Visible = true;
					btnOpenFileDlg.Left = tbValue.Left + tbValue.Width + 15;

					this.Width = lblKey.Width + btnOpenFileDlg.Width + tbValue.Width + 50;
				}
				break;
			case ValueType.VT_BOOLEAN:
				{
					cbValue.Visible = true;
					cbValue.Left = lblKey.Left + lblKey.Width + 10;
					if (m_szValue == "true") cbValue.Checked = true;
					else cbValue.Checked = false;
				}
				break;
			case ValueType.VT_VECTOR:
				{
					tbValue.Visible = true;
					tbValue2.Visible = true;
					tbValue3.Visible = true;

					tbValue.Width = tbValue2.Width = tbValue3.Width = 70;

					tbValue.Left = lblKey.Left + lblKey.Width + 10;
					tbValue2.Left = tbValue.Left + tbValue.Width + 10;
					tbValue3.Left = tbValue2.Left + tbValue2.Width + 10;

					string [] strValues;
					string delimStr = " ";
					char [] delimiter = delimStr.ToCharArray();

					strValues = m_szValue.Split(delimiter, 3);

					if (strValues.GetLength(0) >= 2)
					{
						tbValue.Text = strValues[0];
						tbValue2.Text = strValues[1];
						tbValue3.Text = strValues[2];
					}
				}
				break;
			case ValueType.VT_COLOR:
				{
					tbValue.Visible = true;
					tbValue2.Visible = true;
					tbValue3.Visible = true;
					tbValue4.Visible = true;

					tbValue.Width = tbValue2.Width = tbValue3.Width = tbValue4.Width = 70;

					tbValue.Left = lblKey.Left + lblKey.Width + 10;
					tbValue2.Left = tbValue.Left + tbValue.Width + 10;
					tbValue3.Left = tbValue2.Left + tbValue2.Width + 10;
					tbValue4.Left = tbValue3.Left + tbValue3.Width + 10;

					string [] strValues;
					string delimStr = " ";
					char [] delimiter = delimStr.ToCharArray();

					strValues = m_szValue.Split(delimiter, 4);

					if (strValues.GetLength(0) >= 3)
					{
						tbValue.Text = strValues[0];
						tbValue2.Text = strValues[1];
						tbValue3.Text = strValues[2];
						tbValue4.Text = strValues[3];
					}

				}
				break;
			};
			
		}

		private void btnOK_Click(object sender, System.EventArgs e)
		{
			switch (m_nValueType)
			{
			case ValueType.VT_INTEGER:
				{
					m_szValue = Convert.ToString(intEdit.Value);
				}
				break;
			case ValueType.VT_FLOAT:
				{
					float f = 0.0f;
					try
					{
						f = (float)Convert.ToDouble(tbValue.Text);
					}
					catch (System.FormatException)
					{
						f = 0.0f;
					}

					m_szValue = Convert.ToString(f);
				}
				break;
			case ValueType.VT_STRING:
				{
					m_szValue = tbValue.Text;
				}
				break;
			case ValueType.VT_PATH:
				{
					m_szValue = tbValue.Text;
				}
				break;
			case ValueType.VT_BOOLEAN:
				{
					if (cbValue.Checked == true) m_szValue = "true";
					else m_szValue = "false";
				}
				break;
			case ValueType.VT_VECTOR:
				{
					m_szValue = tbValue.Text + " " + tbValue2.Text + " " + tbValue3.Text;
				}
				break;
			case ValueType.VT_COLOR:
				{
					m_szValue = tbValue.Text + " " + tbValue2.Text + " " + tbValue3.Text + " " + tbValue4.Text;
				}
				break;
			};
		}

		private void btnOpenFileDlg_Click(object sender, System.EventArgs e)
		{
			if (ofdFile.ShowDialog() == DialogResult.OK)
			{
				string file_lower = ofdFile.FileName.ToLower();
				string file = ofdFile.FileName;

				// 어쩔 수 없이 \data 부터 짜른다. OTL
				
				int idx = file_lower.LastIndexOf("data\\");
				if (idx >= 0)
				{
					file = file.Remove(0, idx);
				}
				file = file.Replace("\\", "/");
				tbValue.Text = file;

			}
		}

		public string Value
		{
			get
			{
				return m_szValue;
			}
		}
	}
}
