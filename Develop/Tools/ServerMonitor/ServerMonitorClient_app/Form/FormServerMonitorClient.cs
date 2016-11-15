using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;
using SMCommon;

namespace ServerMonitorClient
{
    public partial class FormServerMonitorClient : Form
    {
        private ServerMonitorClient m_refModel = null;
        private TreeViewController m_controllerTreeView = null;
        private MonitoringLogger m_monitoringLogger = null;
        private TrayIconUpdater m_trayIconUpdater = null;

        public TreeViewController refTreeViewCntroller { get { return m_controllerTreeView; } }
        public MonitoringLogger refMonitoringLogger { get { return m_monitoringLogger; } }
        public TrayIconUpdater refTrayIconUpdater { get { return m_trayIconUpdater; } }
        public string SelectedNodeInfoText { set { textboxSelectedNodeInfo.Text = value; } }

        public FormServerMonitorClient(ServerMonitorClient refModel)
        {
            m_refModel = refModel;
            InitializeComponent();

            m_controllerTreeView = new TreeViewController(tviewServerList);
            m_monitoringLogger = new MonitoringLogger(listboxMonitoringLog);

            m_trayIconUpdater = new TrayIconUpdater(notifyIconServerMonitorClient, refModel.refMachineInfoMgr);
            m_trayIconUpdater.RegIcon(TRAY_ICON_TYPE.FINE, notifyIconFine);
            m_trayIconUpdater.RegIcon(TRAY_ICON_TYPE.BAD, notifyIconBad);
            m_trayIconUpdater.RegIcon(TRAY_ICON_TYPE.CHANGING, notifyIconChanging);
        }

        public void UpdateButtonEnableState()
        {
            btnStart.Enabled = false;
            btnTerminate.Enabled = false;
            btnToggleRevivable.Enabled = false;

            if (m_controllerTreeView.IsSelectedMachineNode())
            {
                btnStart.Enabled = true;
                btnTerminate.Enabled = true;
            }
            if (m_controllerTreeView.IsSelectedServerNode())
            {
                NODE_ICON eIcon = m_controllerTreeView.GetSelectedNodeIcon();
                switch (eIcon)
                {
                    case NODE_ICON.ICON_INDEX_SERVER_FINE:
                    case NODE_ICON.ICON_INDEX_SERVER_FINE_NOT_REVIVE:
                    case NODE_ICON.ICON_INDEX_SERVER_PROCESS_CHANGING:
                    case NODE_ICON.ICON_INDEX_SERVER_INACTIVE:
                        btnToggleRevivable.Enabled = true;
                        btnTerminate.Enabled = true;
                        break;
                    case NODE_ICON.ICON_INDEX_SERVER_BAD:
                        btnStart.Enabled = true;
                        break;
                    case NODE_ICON.ICON_INDEX_SERVER_ERROR:
                        btnTerminate.Enabled = true;
                        break;
                }
            }
        }

        public void ShowBalloonTip_ServerInfo(string strMachine, string strServerName, int nWorldID, int nServerID, string strMsg)
        {
            string strBalloonMsg = "";
            strBalloonMsg += "Machine\t: " + strMachine + "\n";
            strBalloonMsg += "ServerName\t: " + strServerName + "\n";
            strBalloonMsg += "WorldID-ServerID\t: " + nWorldID + "-" + nServerID + "\n";
            strBalloonMsg += "Message\t: " + strMsg;

            ShowBalloonTip("서버 상태 변경", strBalloonMsg);
        }
        public void ShowBalloonTip(string strTitle, string strMsg)
        {
            if (false == menuShowBalloonToolTip.Checked) return;

            notifyIconServerMonitorClient.ShowBalloonTip(3000, strTitle, strMsg, ToolTipIcon.Info);
        }


        private void StartAllServerInMachine(string strSelectedMachineName)
        {
            string[] arrServerName;
            ServerInfoMgr refServerInfoMgr = m_refModel.refMachineInfoMgr.GetServerInfoMgr(strSelectedMachineName);
            if (null == refServerInfoMgr) return;

            refServerInfoMgr.GetAllServerName(out arrServerName);

            for (int i = 0; i < arrServerName.Length; ++i)
            {
                string strServerName = arrServerName[i];
                ServerInfo refServerInfo = refServerInfoMgr.Find(strServerName);
                if (null == refServerInfo) continue;

                if (refServerInfo.GetProcessState() == PROCESS_STATE.STARTING ||
                    refServerInfo.GetProcessState() == PROCESS_STATE.RUNNING) continue;

                m_refModel.StartServer(strSelectedMachineName, strServerName);
            }
        }
        private void StartServer(string strSelectedMachineName, string strSelectedServerName)
        {
            ServerInfoMgr refServerInfoMgr = m_refModel.refMachineInfoMgr.GetServerInfoMgr(strSelectedMachineName);
            if (null == refServerInfoMgr) return;

            ServerInfo refServerInfo = refServerInfoMgr.Find(strSelectedServerName);
            if (null == refServerInfo) return;

            if (refServerInfo.GetProcessState() == PROCESS_STATE.STARTING ||
                refServerInfo.GetProcessState() == PROCESS_STATE.RUNNING) return;

            m_refModel.StartServer(strSelectedMachineName, strSelectedServerName);
        }
        private void TerminateAllServerInMachine(string strSelectedMachineName)
        {
            string[] arrServerName;
            ServerInfoMgr refServerInfoMgr = m_refModel.refMachineInfoMgr.GetServerInfoMgr(strSelectedMachineName);
            if (null == refServerInfoMgr) return;

            refServerInfoMgr.GetAllServerName(out arrServerName);

            List<string> listServerName = new List<string>();
            for (int i = 0; i < arrServerName.Length; ++i)
            {
                string strServerName = arrServerName[i];
                ServerInfo refServerInfo = refServerInfoMgr.Find(strServerName);
                if (null == refServerInfo) continue;

                if (refServerInfo.GetProcessState() == PROCESS_STATE.TERMINATED) continue;

                listServerName.Add(strServerName);
            }

            if (listServerName.Count == 0) return;

            string strAskMessage = strSelectedMachineName;
            strAskMessage += " 머신의 모든 서버를 정지하시겠습니까?\r\n";
            strAskMessage += "(!주의! NOUPDATE 상태인 서버는 프로세스가 정상 종료 되지 않을 수 있습니다.)\r\n";
            for (int i = 0; i < listServerName.Count; ++i)
            {
                strAskMessage += " - ";
                strAskMessage += listServerName[i];
                strAskMessage += "\r\n";
            }
            if (ConfirmTerminateServer(strAskMessage) == false) return;

            for (int i = 0; i < listServerName.Count; ++i)
            {
                m_refModel.TerminateServer(strSelectedMachineName, listServerName[i]);
            }
        }
        private void TerminateServer(string strSelectedMachineName, string strSelectedServerName)
        {
            ServerInfoMgr refServerInfoMgr = m_refModel.refMachineInfoMgr.GetServerInfoMgr(strSelectedMachineName);
            if (null == refServerInfoMgr) return;

            ServerInfo refServerInfo = refServerInfoMgr.Find(strSelectedServerName);
            if (null == refServerInfo) return;

            if (refServerInfo.GetProcessState() == PROCESS_STATE.TERMINATED) return;

            string strAskMessage = "{0} 머신의 {1} 를 정지하시겠습니까?";
            strAskMessage = string.Format(strAskMessage, strSelectedMachineName, strSelectedServerName);
            if (ConfirmTerminateServer(strAskMessage) == false) return;

            m_refModel.TerminateServer(strSelectedMachineName, strSelectedServerName);
        }
        private bool ConfirmTerminateServer(string strAskMessage)
        {
            DialogResult result = MessageBox.Show(this, strAskMessage, "서버를 정지합니다.", MessageBoxButtons.OKCancel);
            if (result == DialogResult.Cancel) return false;
            
            return true;
        }
        private void CheckShowBalloonToolTip()
        {
            if (menuShowBalloonToolTip.Checked ||
                ctxtmenuShowBalloonToolTip.Checked)
            {
                menuShowBalloonToolTip.Checked = false;
                ctxtmenuShowBalloonToolTip.Checked = false;
            }
            else
            {
                menuShowBalloonToolTip.Checked = true;
                ctxtmenuShowBalloonToolTip.Checked = true;
            }
        }

        #region ServerMonitorClient 이벤트 핸들러
        private void btnStart_Click(object sender, EventArgs e)
        {
            if (m_controllerTreeView.IsSelectedMachineNode() == true)
            {
                string strMachineName;
                m_controllerTreeView.GetSelectedMachineName(out strMachineName);

                StartAllServerInMachine(strMachineName);
            }
            else if (m_controllerTreeView.IsSelectedServerNode() == true)
            {
                string strMachineName;
                string strServerName;
                m_controllerTreeView.GetSelectedMachineName(out strMachineName);
                m_controllerTreeView.GetSelectedServerName(out strServerName);

                StartServer(strMachineName, strServerName);
            }
        }
        private void btnTerminate_Click(object sender, EventArgs e)
        {
            if (m_controllerTreeView.IsSelectedMachineNode() == true)
            {
                string strMachineName;
                m_controllerTreeView.GetSelectedMachineName(out strMachineName);

                TerminateAllServerInMachine(strMachineName);
            }
            else if (m_controllerTreeView.IsSelectedServerNode() == true)
            {
                string strMachineName;
                string strServerName;
                m_controllerTreeView.GetSelectedMachineName(out strMachineName);
                m_controllerTreeView.GetSelectedServerName(out strServerName);

                TerminateServer(strMachineName, strServerName);
            }
        }
        private void btnToggleRevivable_Click(object sender, EventArgs e)
        {
            if (m_controllerTreeView.IsSelectedMachineNode() == true || m_controllerTreeView.IsSelectedServerNode() == false)
            {
                string strMessage = "서버 노드를 선택해 주세요.";
                MessageBox.Show(strMessage);
                return;
            }

            string strMachineName;
            string strServerName;
            m_controllerTreeView.GetSelectedMachineName(out strMachineName);
            m_controllerTreeView.GetSelectedServerName(out strServerName);

            m_refModel.ToggleServerRevivable(strMachineName, strServerName);
        }
        private void btnRefresh_Click(object sender, EventArgs e)
        {
            m_controllerTreeView.ClearAllServerNodes();
            m_refModel.Refresh();
        }
        private void tviewServerList_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (false == m_controllerTreeView.IsSelectedServerNode() &&
                false == m_controllerTreeView.IsSelectedMachineNode())
                return;

            // 선택한 노드 정보 갱신
            string strMachineName;
            m_controllerTreeView.GetSelectedMachineName(out strMachineName);

            if (m_controllerTreeView.IsSelectedMachineNode())
            {
                m_refModel.UpdateView_SelectedMachineInfo(strMachineName);
            }
            else if (m_controllerTreeView.IsSelectedServerNode())
            {
                string strServerName;
                m_controllerTreeView.GetSelectedServerName(out strServerName);

                m_refModel.UpdateView_SelectedServerInfo(strMachineName, strServerName);
            }

            // 선택한 노드 상태에 맞게 버튼 상태 갱신
            UpdateButtonEnableState();
        }

        private void menuReloadReconnect_Click(object sender, EventArgs e)
        {
            m_controllerTreeView.Clear();
            m_refModel.ReconnectReload();
        }
        private void menuClearLog_Click(object sender, EventArgs e)
        {
            m_monitoringLogger.Clear();
        }

        private void timerMainUpdate_Tick(object sender, EventArgs e)
        {
            Timer eSender = (Timer)sender;
            m_refModel.Update(eSender.Interval * 0.001f);
        }

        private void menuShowBalloonToolTip_Click(object sender, EventArgs e)
        {
            CheckShowBalloonToolTip();
        }
        private void ctxtmenuShowBalloonToolTip_Click(object sender, EventArgs e)
        {
            CheckShowBalloonToolTip();
        }
        #endregion

        #region 윈도우 컨트롤 관련 이벤트 핸들러
        private void menuExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void ctxtmenuShowServerMonitorClient_Click(object sender, EventArgs e)
        {
            Show();
        }

        private void ctxtmenuExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void notifyIconServerMonitorClient_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            TopMost = true;
            Show();
            Activate();
            this.WindowState = FormWindowState.Normal;
        }

        private void MainForm_Deactivate(object sender, EventArgs e)
        {
            TopMost = false;
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Hide();
            if (e.CloseReason == CloseReason.FormOwnerClosing ||
                e.CloseReason == CloseReason.MdiFormClosing ||
                e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                return;
            }
            m_refModel.Destroy();
        }
        #endregion

    }
}
