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
    public partial class OptionForm : Form
    {
        public OptionForm()
        {
            InitializeComponent();
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void cbAutoMonitorFile_CheckedChanged(object sender, EventArgs e)
        {
            Option.AutoMonitorFile = this.cbAutoMonitorFile.Checked;
        }

        private void OptionForm_Shown(object sender, EventArgs e)
        {
            this.cbAutoMonitorFile.Checked = Option.AutoMonitorFile;
        }
    }
}
