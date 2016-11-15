using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using SMCommon;
using ServerMonitor.DB;

namespace ServerMonitor
{
    public class ServerListViewUpdater
    {
        private readonly float TIME_UPDATE_DELAY = 5.0f;

        private ListView m_refLviewServerList = null;
        private int m_nColumnIndex_ServerName = -1;
        private ServerInfoMgr m_refServerInfoMgr = null;

        private TimeRegulator m_timeRegulatorToUpdate = new TimeRegulator();

        public ServerListViewUpdater(ListView refLviewServerList, int nServerNameColIndex, ServerInfoMgr refServerInfoMgr)
        {
            m_refLviewServerList = refLviewServerList;
            m_refServerInfoMgr = refServerInfoMgr;
            m_nColumnIndex_ServerName = nServerNameColIndex;

            m_timeRegulatorToUpdate.Start(TIME_UPDATE_DELAY);
        }

        public void BuildServerListView()
        {
            m_refLviewServerList.Items.Clear();

            SERVERLISTVIEWINFO[] arrServerListViewInfo = new SERVERLISTVIEWINFO[m_refServerInfoMgr.ServerInfoCount];
            m_refServerInfoMgr.GetSERVERLISTVIEWINFO(ref arrServerListViewInfo);

            for (int i = 0; i < arrServerListViewInfo.Length; ++i)
            {
                ListViewItem lviewItem = new ListViewItem();
                for (int j = 0; j < 8; ++j)
                    lviewItem.SubItems.Add("");

                SetListViewSebItem(lviewItem, ref arrServerListViewInfo[i]);
                m_refLviewServerList.Items.Add(lviewItem);
            }
        }

        public void UpdateAllServerListInfo()
        {
            for (int i = 0; i < m_refLviewServerList.Items.Count; ++i)
            {
                ListViewItem lviewItem = m_refLviewServerList.Items[i];

                if (lviewItem.SubItems[m_nColumnIndex_ServerName] == null) continue;
                string strServerNameItem = lviewItem.SubItems[m_nColumnIndex_ServerName].Text;

                SERVERLISTVIEWINFO serverListViewInfo = new SERVERLISTVIEWINFO();
                if (m_refServerInfoMgr.GetSERVERLISTVIEWINFO(strServerNameItem, ref serverListViewInfo) == false)
                    continue;

                SetListViewSebItem(lviewItem, ref serverListViewInfo);
            }
        }

        public void UpdateServerListInfo(string strServerName)
        {
            for (int i = 0; i < m_refLviewServerList.Items.Count; ++i)
            {
                ListViewItem lviewItem = m_refLviewServerList.Items[i];

                if (lviewItem.SubItems[m_nColumnIndex_ServerName] == null) break;
                string strServerNameItem = lviewItem.SubItems[m_nColumnIndex_ServerName].Text;

                if (strServerNameItem.Equals(strServerName) == false) continue;

                SERVERLISTVIEWINFO serverListViewInfo = new SERVERLISTVIEWINFO();
                if (m_refServerInfoMgr.GetSERVERLISTVIEWINFO(strServerName, ref serverListViewInfo) == false)
                    break;

                SetListViewSebItem(lviewItem, ref serverListViewInfo);
                break;
            }
        }

        private void SetListViewSebItem(ListViewItem refLViewItem, ref SERVERLISTVIEWINFO refServerLViewInfo)
        {
            int i = 1;
            refLViewItem.Checked = refServerLViewInfo.bRevivable;
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.eProcessState.ToString();
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.eServableState.ToString();
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.strServerName;
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.eServerType.ToString();
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.nWorldID.ToString();
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.nServerID.ToString();
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.nCurPlayerCount.ToString();
            refLViewItem.SubItems[i++].Text = refServerLViewInfo.timeLatestStart.ToString("yyyy/MM/dd-HH:mm:ss");
        }

    }
}
