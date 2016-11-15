using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ServerMonitor
{
    public partial class FormSettings : Form
    {
        private ServerInfoMgr m_refServerInfoMgr = null;
        private bool m_IsChangeServerInfoList = false;

        public FormSettings(ServerInfoMgr refServerInfoMgr)
        {
            m_refServerInfoMgr = refServerInfoMgr;

            InitializeComponent();
            UpdateServerList();
        }

        private void UpdateServerList()
        {
            listboxServers.Items.Clear();

            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                listboxServers.Items.Add(node.Value.ServerName);
            }
        }

        private void ShowAddServerDlg()
        {
            FormSettingsAddServer dlgAddServer = new FormSettingsAddServer(m_refServerInfoMgr);

            DialogResult ret = dlgAddServer.ShowDialog(this);
            if (ret == DialogResult.OK)
            {
                UpdateServerList();
                textboxDesc.Text = "";

                m_IsChangeServerInfoList = true;
            }
        }

        private void DeleteServerInfo()
        {
            string strSelectedServerName = listboxServers.SelectedItem.ToString();
            if (m_refServerInfoMgr.DeleteServerInfo(strSelectedServerName) == false)
            {
                MessageBox.Show("서버 정보 삭제 실패!");
                return;
            }

            listboxServers.Items.RemoveAt(listboxServers.SelectedIndex);

            m_IsChangeServerInfoList = true;
        }

        private void OutPutServerDescription(string strServerName)
        {
            if (strServerName.Length == 0) return;

            ServerInfo refServerInfo = m_refServerInfoMgr.Find(strServerName);
            if (null == refServerInfo) return;

            string strServerProcessName;
            strServerProcessName = System.IO.Path.GetFileNameWithoutExtension(refServerInfo.ExePath);

            textboxDesc.Text = "Server Name = " + strServerName + "\r\n";
            textboxDesc.Text += "Execute File Path = " + refServerInfo.ExePath + "\r\n";
            textboxDesc.Text += "Process Name = " + strServerProcessName + "\r\n";
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            ShowAddServerDlg();
        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            DeleteServerInfo();
            textboxDesc.Text = "";
        }

        private void listboxServers_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listboxServers.SelectedItem == null) return;

            btnDelete.Enabled = true;
            OutPutServerDescription(listboxServers.SelectedItem.ToString());
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            if (m_IsChangeServerInfoList)
                this.DialogResult = DialogResult.OK;
            else
                this.DialogResult = DialogResult.Cancel;

            Close();
        }
        
    }
}
