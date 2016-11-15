using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Velixian
{
    public partial class EditDialogBox : Form
    {
        public EditDialogBox()
        {
            InitializeComponent();
        }

        public class EditDialogBoxResult
        {
            public DialogResult DialogResult;
            private string _strValue;

            public EditDialogBoxResult(DialogResult result, string strValue)
            {
                this.DialogResult = result;
                this._strValue = strValue;
            }
            public string ValueString
            {
                get { return _strValue; }
            }

            public int ValueInt
            {
                get { return Convert.ToInt32(_strValue); }
            }
        }

        static public EditDialogBoxResult ShowDialog(string title, string desc, string strValue)
        {
            using (EditDialogBox dialog = new EditDialogBox())
            {
                dialog.Text = title;
                dialog.lbDesc.Text = desc;
                dialog.tbValue.Text = strValue;
                
                DialogResult result = dialog.ShowDialog();
                return new EditDialogBoxResult(result, dialog.tbValue.Text);
            }
        }

        static public EditDialogBoxResult ShowDialog(string title, string desc, int nValue)
        {
            string strValue = nValue.ToString();
            return ShowDialog(title, desc, strValue);

        }

        private void EditDialogBox_Load(object sender, EventArgs e)
        {
            this.tbValue.Focus();
        }

        public string GetValueString()
        {
            return this.tbValue.Text;
        }

        public int GetValueInt()
        {
            return Convert.ToInt32(GetValueString());
        }
    }
}
