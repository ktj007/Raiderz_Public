using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SMCommon;

namespace ServerMonitor
{
    public partial class FormServerMonitor : Form
    {
        private readonly string ERRSTR_FAILED_CHECK_SERVER_REVIVE = "실패! 서버의 되살리기 옵션 적용에 실패했습니다.";

        private ServerMonitor m_refModel = null;

        public ListBox refLBoxMonitoringLog { get { return listBoxMonitoringLog; } }
        public ListView refLViewServerList { get { return lviewServerList; } }
        public ColumnHeader refColServerName { get { return colServerName; } }

        public FormServerMonitor(ServerMonitor refModel)
        {
            m_refModel = refModel;
            InitializeComponent();
        }

        public void UpdateStatusBar(int nSessionCount)
        {
            statusServerMonitor.Items[0].Text = "Connected : " + nSessionCount;
        }
        public void UpdateButtonEnableState()
        {
            if (lviewServerList.SelectedIndices.Count == 0)
            {
                btnStart.Enabled = false;
                btnTerminate.Enabled = false;
                btnShowDetail.Enabled = false;
            }
            else
            {
                ListViewItem selectedLViewItem = lviewServerList.SelectedItems[0];
                string strServerName = selectedLViewItem.SubItems[colServerName.Index].Text;

                PROCESS_STATE eProcessState;
                m_refModel.refServerInfoMgr.GetProcessState(strServerName, out eProcessState);

                btnShowDetail.Enabled = true;

                if (eProcessState == PROCESS_STATE.RUNNING ||
                    eProcessState == PROCESS_STATE.STARTING ||
                    eProcessState == PROCESS_STATE.NOUPDATE)
                {
                    btnStart.Enabled = false;
                    btnTerminate.Enabled = true;
                }
                else if (eProcessState == PROCESS_STATE.TERMINATED ||
                         eProcessState == PROCESS_STATE.TERMINATING ||
                         eProcessState == PROCESS_STATE.DIED)
                {
                    btnStart.Enabled = true;
                    btnTerminate.Enabled = false;
                }
            }
        }


        private bool ConfirmTerminateServer(string strAskMessage)
        {
            DialogResult result = MessageBox.Show(this, strAskMessage, "서버를 정지합니다.", MessageBoxButtons.OKCancel);
            if (result == DialogResult.Cancel) return false;

            return true;
        }
        

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (lviewServerList.SelectedItems.Count == 0) return;

            for (int i = 0; i < lviewServerList.SelectedItems.Count; ++i)
            {
                ListViewItem selectedLViewItem = lviewServerList.SelectedItems[i];
                string strServerName = selectedLViewItem.SubItems[colServerName.Index].Text;

                m_refModel.StartServer(strServerName);
            }

            UpdateButtonEnableState();
        }
        private void btnTerminate_Click(object sender, EventArgs e)
        {
            if (lviewServerList.SelectedItems.Count == 0) return;

            // 되묻기 다이얼로그 박스
            string[] arrServerName = new string[lviewServerList.SelectedItems.Count];
            for (int i = 0; i < lviewServerList.SelectedItems.Count; ++i)
            {
                ListViewItem selectedLViewItem = lviewServerList.SelectedItems[i];
                arrServerName[i] = selectedLViewItem.SubItems[colServerName.Index].Text;
            }

            string strAskMessage = "서버를 정지하시겠습니까?\r\n";
            strAskMessage += "(!주의! NOUPDATE 상태인 서버는 프로세스가 정상 종료 되지 않을 수 있습니다.)\r\n";
            for (int i = 0; i < arrServerName.Length; ++i)
            {
                strAskMessage += " - ";
                strAskMessage += arrServerName[i];
                strAskMessage += "\r\n";
            }

            if (ConfirmTerminateServer(strAskMessage) == false) return;

            for (int i = 0; i < arrServerName.Length; ++i)
            {
                string strServerName = arrServerName[i];
                m_refModel.TerminateServer(strServerName);
            }

            UpdateButtonEnableState();
        }
        private void btnShowDetail_Click(object sender, EventArgs e)
        {
            if (lviewServerList.SelectedItems.Count == 0) return;

            ListViewItem selectedLViewItem = lviewServerList.SelectedItems[lviewServerList.SelectedItems.Count-1];
            string strServerName = selectedLViewItem.SubItems[colServerName.Index].Text;

            FormServerDetailInfo dlgDetailInfo = new FormServerDetailInfo();
            dlgDetailInfo.Init(strServerName, m_refModel.refServerInfoMgr);
            dlgDetailInfo.Show(this);
        }
        private void lviewServerList_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (lviewServerList.Items[e.Index] == null) return;

            string strServerName = lviewServerList.Items[e.Index].SubItems[colServerName.Index].Text;
            bool bChangedRevivable = (e.NewValue == CheckState.Checked) ? true : false;

            bool bCurRevivable = m_refModel.refServerInfoMgr.IsServerRevivable(strServerName);
            if (bCurRevivable == bChangedRevivable) return;

            if (m_refModel.SetServerRevivable(strServerName, bChangedRevivable) == false)
            {
                string strMessage = String.Format("{0} ({1})", ERRSTR_FAILED_CHECK_SERVER_REVIVE, strServerName);
                MessageBox.Show(strMessage);
            }
        }
        private void lviewServerList_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateButtonEnableState();
        }

        private void menuSettings_Click(object sender, EventArgs e)
        {
            FormSettings dlgConfig = new FormSettings(m_refModel.refServerInfoMgr);
            if (dlgConfig.ShowDialog(this) == DialogResult.OK)
            {
                m_refModel.RefreshServerList();
            }
        }
        private void reloadAllServersINIFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_refModel.ReloadAllServersINIFile();
        }
        private void menuClearLog_Click(object sender, EventArgs e)
        {
            listBoxMonitoringLog.Items.Clear();
        }
        private void menuExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void timerUpdate_Tick(object sender, EventArgs e)
        {
            Timer eSender = (Timer)sender;
            m_refModel.Update(eSender.Interval * 0.001f);
        }

        #region 윈도우 조작 관련 이벤트 핸들러
        private void ctxtmenuShowServerMonitor_Click(object sender, EventArgs e)
        {
            Show();
        }
        private void ctxtmenuExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
        private void notifyIconServerMonitor_DoubleClick(object sender, EventArgs e)
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