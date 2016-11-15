using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CharacterToolForms
{
    public partial class TextOneBox : Form
    {
        public TextOneBox()
        {
            InitializeComponent();
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
		     this.DialogResult = System.Windows.Forms.DialogResult.OK;
		     this.Close();
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void TextValue_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (m_bOnlyNumberKeyDown)
            {
                // 오직 숫자키만 사용
                // 백스페이스 가능
                if ((e.KeyChar < 48 || e.KeyChar > 57) && e.KeyChar != 8)
                {
                    e.Handled = true;
                }
            }

            // 자동 엔터
            if (e.KeyChar == 13)
            {
                OKButton_Click(sender, e);
            }
        }
    }
}