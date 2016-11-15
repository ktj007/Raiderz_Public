using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;

namespace Velixian
{
    public partial class OutputForm : EditForm
    {
        protected string m_KeepOutputMessage;

        public OutputForm()
        {
            InitializeComponent();
        }

        public void Output(string strMessage)
        {
            rtbOutput.Text += strMessage;
            rtbOutput.Select(rtbOutput.Text.Length + 1, 2);
            rtbOutput.ScrollToCaret();
        }


    }
}
