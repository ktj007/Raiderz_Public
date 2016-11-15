using System;
using System.Collections.Generic;
using System.Text;
using SMCommon;
using ServerMonitor.DB;

namespace ServerMonitor
{
    public class ServerController
    {
        private ServerInfoMgr m_refServerInfoMgr = null;
        private ProcessSystem m_refProcessSystem = null;
        private MonitoringLoggerMgr m_refMonitoringLoggerMgr = null;

        public ServerController(ServerInfoMgr refServerInfoMgr, ProcessSystem refProcessSystem, MonitoringLoggerMgr refMonitoringLoggerMgr)
        {
            m_refServerInfoMgr = refServerInfoMgr;
            m_refProcessSystem = refProcessSystem;
            m_refMonitoringLoggerMgr = refMonitoringLoggerMgr;
        }
        public virtual bool StartServer(string strServerName)
        {
            ServerInfo refServerInfo = m_refServerInfoMgr.Find(strServerName);
            if (null == refServerInfo)
                return false;

            if (0 == refServerInfo.WorldID || 0 == refServerInfo.ServerID)
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_START_SERVER_ERROR);
                return false;
            }

            if (PROCESS_STATE.RUNNING == refServerInfo.ProcessState || PROCESS_STATE.STARTING == refServerInfo.ProcessState)
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_START_SERVER_ALREADY_START);
                return false;
            }

            int nProcessID;
            if (m_refProcessSystem.StartProcess(refServerInfo.ExePath, out nProcessID) == false)
            {
                FileLog.Instance.Write("실패! ServerController.StartServer(), 서버 프로세스를 시작하지 못했습니다. (ServerName={0}, ExePath={1})", strServerName, refServerInfo.ExePath);
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_START_SERVER_PROCESS);
                return false;
            }

            refServerInfo.SetProcessStarting(nProcessID);

            m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.STARTING_SERVER);
            return true;
        }
        public virtual bool TerminateServer(string strServerName)
        {
            ServerInfo refServerInfo = m_refServerInfoMgr.Find(strServerName);
            if (null == refServerInfo)
                return false;


            if (0 == refServerInfo.WorldID || 0 == refServerInfo.ServerID)
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_TERMINATE_SERVER_ERROR);
                return false;
            }

            if (PROCESS_STATE.TERMINATED == refServerInfo.ProcessState || PROCESS_STATE.TERMINATING == refServerInfo.ProcessState)
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_TERMINATE_SERVER_ALREADY_TERMINATED);
                return false;
            }

            if ((PROCESS_STATE.RUNNING != refServerInfo.ProcessState && PROCESS_STATE.STARTING != refServerInfo.ProcessState && PROCESS_STATE.NOUPDATE != refServerInfo.ProcessState))
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_TERMINATE_INVALID_STATE);
                return false;
            }

            //if (m_refProcessSystem.SendDestroyMessage(refServerInfo.ProcessID) == false))
            if (m_refProcessSystem.PostSignal_Terminate(refServerInfo) == false)
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.FAILED_TERMINATE_SERVER_SEND_DESTROY_MESSAGE);
                return false;
            }

            if (PROCESS_STATE.NOUPDATE == refServerInfo.ProcessState)
            {
                m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.WARN_TERMINATE_SERVER_NO_UPDATE);
            }
            else
            {
                refServerInfo.SetProcessTerminating();
            }

            m_refMonitoringLoggerMgr.Write(strServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.TERMINATING_SERVER);
            return true;
        }

    }
}
