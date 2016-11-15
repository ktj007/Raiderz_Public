using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor.DB;
using SMCommon;

namespace ServerMonitor
{
    public class ServerMonitor
    {
        private FormServerMonitor       m_refView = null;
        private Config                  m_config = null;
        private NetServer               m_netServer = null;
        private DBMgr                   m_dbMgr = new DBMgr();
        private ServerInfoMgr           m_serverInfoMgr = new ServerInfoMgr();
        private ServerDBInfoUpdater     m_updaterServerDBInfo = null;
        private ServerController        m_serverController = null;
        private ServerReviver           m_serverReviver = null;
        private ProcessStateChecker     m_checkerServerState = null;
        private ServableStateUpdater    m_updaterServableState = null;
        private ServerListViewUpdater   m_updaterServerListView = null;
        private LogStringMgr            m_logStringMgr = new LogStringMgr();
        private MonitoringLoggerMgr     m_monitoringLoggerMgr = null;
        private ServerMonitorClientMgr  m_serverMonitorClientMgr = new ServerMonitorClientMgr();

        public ServerInfoMgr refServerInfoMgr { get { return m_serverInfoMgr; } }
        public ServerMonitorClientMgr refServerMonitorClientMgr { get { return m_serverMonitorClientMgr; } }

        public bool Init(FormServerMonitor refServerMonitorView)
        {
            m_refView = refServerMonitorView;

            m_config = new Config(new ConfigFileController());
            if (m_config.ReadConfigINI() == false)
            {
                FileLog.Instance.Write("실패! INI파일의 설정을 읽지 못했습니다!");
                return false;
            }

            if (m_config.LoadConfig() == false)
            {
                FileLog.Instance.Write("실패! Config 파일 읽기에 실패했습니다!");
                return false;
            }
            FileLog.Instance.Write("Config 파일 로드.");

            m_netServer = new NetServer(this);
            if (m_netServer.Start(m_config.PortNumber) == false)
            {
                FileLog.Instance.Write("실패! Network 초기화에 실패했습니다!");
                return false;
            }
            FileLog.Instance.Write("Network 초기화.");

            if (m_dbMgr.Create(m_config.DB_DataSource, m_config.DB_InitialCatalog, m_config.DB_UserID, m_config.DB_Password) == false)
            {
                FileLog.Instance.Write("실패! DBMgr 생성에 실패했습니다!");
                return false;
            }
            FileLog.Instance.Write("DBMgr 생성.");

            if (m_serverInfoMgr.Init(m_config.refListServerInfo, m_config.AddServerInfo, m_config.DeleteServerInfo) == false)
            {
                FileLog.Instance.Write("실패! ServerInfoMgr 초기화에 실패했습니다!");
                return false;
            }
            FileLog.Instance.Write("ServerInfoMgr 초기화.");

            if (InitMonitoringLog(m_dbMgr, m_logStringMgr) == false)
            {
                FileLog.Instance.Write("실패! Monitoring Log 초기화에 실패했습니다!");
                return false;
            }
            FileLog.Instance.Write("MonitoringLoggerMgr 초기화.");


            m_updaterServerDBInfo = new ServerDBInfoUpdater(m_serverInfoMgr, m_dbMgr);

            ProcessSystem processSystem = new ProcessSystem();
            m_serverController = new ServerController(m_serverInfoMgr, processSystem, m_monitoringLoggerMgr);
            m_serverReviver = new ServerReviver(m_serverInfoMgr, m_serverController);
            m_checkerServerState = new ProcessStateChecker(m_serverInfoMgr, processSystem, m_monitoringLoggerMgr);
            m_updaterServableState = new ServableStateUpdater(m_serverInfoMgr, m_monitoringLoggerMgr);
            m_updaterServerListView = new ServerListViewUpdater(m_refView.refLViewServerList, m_refView.refColServerName.Index, m_serverInfoMgr);
            m_updaterServerListView.BuildServerListView();

            FileLog.Instance.Write("ServerMonitor 초기화.");

            m_refView.UpdateStatusBar(m_serverMonitorClientMgr.SessionCount);

            return true;
        }
        public void Destroy()
        {
            FileLog.Instance.Write("DB 종료.");
            m_dbMgr.Release();
            FileLog.Instance.Write("Net 종료.");
            m_netServer.Stop();
            FileLog.Instance.Write("종료 끝.");
        }
        public void Update(float fDelta)
        {
            m_netServer.Update();
            m_dbMgr.Update();
            m_updaterServerDBInfo.Update(fDelta);

            bool bUpdateServerView = false;
            if (m_checkerServerState.Update(fDelta))    bUpdateServerView = true;
            if (m_serverReviver.Update(fDelta))         bUpdateServerView = true;
            if (m_updaterServableState.Update(fDelta))  bUpdateServerView = true;

            if (bUpdateServerView)
                m_updaterServerListView.UpdateAllServerListInfo();
        }

        public void StartServer(string strServerName)
        {
            PROCESS_STATE eProcessState;
            m_serverInfoMgr.GetProcessState(strServerName, out eProcessState);
            if (eProcessState == PROCESS_STATE.RUNNING || eProcessState == PROCESS_STATE.STARTING)
                return;

            if (m_serverController.StartServer(strServerName) == false)
                return;

            m_updaterServerListView.UpdateServerListInfo(strServerName);
        }
        public void StartAllServer()
        {
            LinkedListNode<ServerInfo> node = m_serverInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfo refServerInfo = node.Value;

                if (refServerInfo.ProcessState == PROCESS_STATE.RUNNING || refServerInfo.ProcessState == PROCESS_STATE.STARTING)
                    continue;

                if (m_serverController.StartServer(refServerInfo.ServerName) == false)
                    continue;
            }

            m_updaterServerListView.UpdateAllServerListInfo();
        }
        public void TerminateServer(string strServerName)
        {
            PROCESS_STATE eProcessState;
            m_serverInfoMgr.GetProcessState(strServerName, out eProcessState);
            if (eProcessState == PROCESS_STATE.TERMINATED)
                return;

            if (m_serverController.TerminateServer(strServerName) == false)
                return;

            m_updaterServerListView.UpdateServerListInfo(strServerName);
        }
        public void TerminateAllServer()
        {
            LinkedListNode<ServerInfo> node = m_serverInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfo refServerInfo = node.Value;

                if (refServerInfo.ProcessState == PROCESS_STATE.TERMINATED)
                    continue;

                if (m_serverController.TerminateServer(refServerInfo.ServerName) == false)
                    continue;
            }

            m_updaterServerListView.UpdateAllServerListInfo();
        }
        public bool SetServerRevivable(string strServerName, bool bCheckedRevive)
        {
            bool bCurRevivable = m_serverInfoMgr.IsServerRevivable(strServerName);
            if (bCheckedRevive == bCurRevivable) return true;

            ServerInfo refServerInfo = m_serverInfoMgr.Find(strServerName);
            if (null == refServerInfo) return false;

            refServerInfo.SetProcessRevivable(bCheckedRevive);

            m_updaterServerListView.UpdateServerListInfo(strServerName);
            return true;
        }
        public void ReloadAllServersINIFile()
        {
            LinkedListNode<ServerInfo> node = m_serverInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfo refServerInfo = node.Value;

                int nWorldID;
                int nServerID;
                Config.ReadServerINI_ServerID(refServerInfo.ExePath, out nWorldID, out nServerID);

                refServerInfo.SetServerID(nWorldID, nServerID);
            }

            RefreshServerList();
        }
        public void RefreshServerList()
        {
            m_updaterServerListView.BuildServerListView();

            TRANS_SERVERLISTNODE[] arrServerListNode = new TRANS_SERVERLISTNODE[m_serverInfoMgr.ServerInfoCount];
            m_serverInfoMgr.GetTRANS_SERVERLISTNODE(ref arrServerListNode);

            UInt64[] arrSessionID;
            m_serverMonitorClientMgr.GetAllSessionID(out arrSessionID);

            for (int i = 0; i < arrSessionID.Length; ++i)
            {
                m_netServer.commandSender.SendGetServerListRes(arrSessionID[i], ref arrServerListNode);
            }
        }


        public void OnAccepted(UInt64 nSessionID)
        {
            m_serverMonitorClientMgr.AddServerMonitorClient(nSessionID);
            m_refView.UpdateStatusBar(m_serverMonitorClientMgr.SessionCount);
        }
        public void OnDisconnected(UInt64 nSessionID)
        {
            m_serverMonitorClientMgr.DeleteServerMonitorClient(nSessionID);
            m_refView.UpdateStatusBar(m_serverMonitorClientMgr.SessionCount);
        }
        public void OnSetReliableClient(UInt64 nSessionID)
        {
            m_serverMonitorClientMgr.SetReliableClient(nSessionID);
        }

        public void DisconnectSession(UInt64 nSessionID)
        {
            m_netServer.DeleteSession(nSessionID);
        }


        private bool InitMonitoringLog(DBMgr refDBMgr, LogStringMgr refLogStringMgr)
        {
            if (refDBMgr == null || refLogStringMgr == null) return false;

            if (refLogStringMgr.LoadStringTable() == false)
            {
                FileLog.Instance.Write("실패! LogStringMgr 로드에 실패했습니다!");
                return false;
            }

            m_monitoringLoggerMgr = new MonitoringLoggerMgr(m_logStringMgr);
            m_monitoringLoggerMgr.AddLogger(new MonitoringLogger_LogFile());
            m_monitoringLoggerMgr.AddLogger(new MonitoringLogger_ListBox(m_refView.refLBoxMonitoringLog));
            m_monitoringLoggerMgr.AddLogger(new MonitoringLogger_Network(m_serverMonitorClientMgr, m_netServer.commandSender));
            /// DB Log 는 작성하지 않게 해둠. aibeast (2010/05/26)
            //m_monitoringLoggerMgr.AddLogger(new MonitoringLogger_DB(m_config.ServerMonitorID, refDBMgr));
            return true;
        }

    }
}
