using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ColtViewer.Model.Types;
using AIMonitor.Model.Types;

namespace ColtViewer
{
    public partial class frmMain : Form
    {
        private NPCAttr npc_attr;

        public frmMain()
        {
            InitializeComponent();

            this.DoubleBuffered = true;            

            // 리스트 컬럼 설정
            listBJobQueue.Columns.Add("JobName", 100, HorizontalAlignment.Left);
            listBJobQueue.Columns.Add("TaskName", 60, HorizontalAlignment.Left);
            listBJobQueue.Columns.Add("Param", -2, HorizontalAlignment.Left);

            listMJobQueue.Columns.Add("JobName", 100, HorizontalAlignment.Left);
            listMJobQueue.Columns.Add("Param", -2, HorizontalAlignment.Left);

            listSJobQueue.Columns.Add("JobName", 100, HorizontalAlignment.Left);
            listSJobQueue.Columns.Add("Param", -2, HorizontalAlignment.Left);

            ClearListLog();
        }

        void ClearListLog()
        {
            listLog.Clear();
            listLog.Columns.Add("Time", 60, HorizontalAlignment.Left);
            listLog.Columns.Add("Type", 60, HorizontalAlignment.Left);
            listLog.Columns.Add("Text", -2, HorizontalAlignment.Left);
        }

        private void frmMain_Load(object sender, EventArgs e)
        {

        }

        private void btnClearLog_Click(object sender, EventArgs e)
        {
            ClearListLog();
        }
    }
}


