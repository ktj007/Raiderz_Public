using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CharacterToolForms
{
    public partial class TalentEffectRegisterEdit : Form
    {
        public TalentEffectRegisterEdit()
        {
            InitializeComponent();
        }

        private void OkButton_Click(object sender, EventArgs e)
        {
            OK();
        }

        private void OK()
        {
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            GetResultTalentID();
            this.Close();
        }

        private void CancleButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void GetResultTalentID()
        {
            System.Windows.Forms.DialogResult res = DialogResult;
            if (res == System.Windows.Forms.DialogResult.OK)
            {
                m_nTalentID = Convert.ToInt32(TalentIDtextBox.Text);
                m_nTalentMode = TalentModecomboBox.SelectedIndex;
            }
        }

        public void SetText(int nTalentID, int nTalentMode)
        {
            this.TalentIDtextBox.Text = Convert.ToString(nTalentID);
            this.TalentModecomboBox.SelectedIndex = nTalentMode;
        }

        private void TalentIDtextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
		     {
                OK();
		     }
		     else if (e.KeyCode == Keys.Escape)
		     {
                 this.Close();
		     }
        }
    }
}