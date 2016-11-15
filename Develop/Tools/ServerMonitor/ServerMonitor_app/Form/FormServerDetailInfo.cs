using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SMCommon;

namespace ServerMonitor
{
    public partial class FormServerDetailInfo : Form
    {
        public FormServerDetailInfo()
        {
            InitializeComponent();
        }

        public void Init(string strServerName, ServerInfoMgr refServerInfoMgr)
        {
            ServerInfo refServerInfo = refServerInfoMgr.Find(strServerName);
            if (null == refServerInfo)
                return;

            listboxDetailInfo.Items.Add("Name : " + refServerInfo.ServerName);
            listboxDetailInfo.Items.Add("Process State : " + refServerInfo.ProcessState.ToString());
            listboxDetailInfo.Items.Add("Exe Path : " + refServerInfo.ExePath);
            listboxDetailInfo.Items.Add("Process Name : " + refServerInfo.ProcessName);
            listboxDetailInfo.Items.Add("Process ID : " + refServerInfo.ProcessID.ToString());
            listboxDetailInfo.Items.Add("World ID : " + refServerInfo.WorldID.ToString());
            listboxDetailInfo.Items.Add("Server ID : " + refServerInfo.ServerID.ToString());
            listboxDetailInfo.Items.Add("Latest Start Time : " + refServerInfo.LatestStartTime.ToString("yyyy/MM/dd-HH:mm:ss"));
        }
    }
}
