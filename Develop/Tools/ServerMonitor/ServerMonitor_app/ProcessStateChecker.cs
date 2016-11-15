using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor.DB;
using SMCommon;

namespace ServerMonitor
{
    public class ProcessStateChecker
    {
        private readonly float TIME_CHECK_DELAY = 5.0f;

        private ServerInfoMgr m_refServerInfoMgr = null;
        private MonitoringLoggerMgr m_refMonitoringLoggerMgr = null;
        private ProcessSystem m_refProcessSystem = null;
        private TimeRegulator m_rgltDelayToCheck = new TimeRegulator();

        public ProcessStateChecker(ServerInfoMgr refServerInfoMgr, ProcessSystem refProcessSystem, MonitoringLoggerMgr refMonitoringLoggerMgr)
        {
            m_refServerInfoMgr = refServerInfoMgr;
            m_refProcessSystem = refProcessSystem;
            m_refMonitoringLoggerMgr = refMonitoringLoggerMgr;

            m_rgltDelayToCheck.Start(TIME_CHECK_DELAY);
        }
        public bool Update(float fDelta)
        {
            if (false == m_rgltDelayToCheck.IsReady(fDelta)) return false;

            bool bIsExistStateChanging = false;

            bool bIsDBRun;
            bool bIsRunningProcess;
            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfo refServerInfo = node.Value;

                if (PROCESS_STATE.TERMINATED == refServerInfo.ProcessState)
                    continue;

                bIsDBRun = refServerInfo.IsRun;
                bIsRunningProcess = m_refProcessSystem.IsRunningProcess(refServerInfo.ProcessName, refServerInfo.ProcessID);

                // 서버 시작 중. 시작 성공
                if (CheckAndUpdate_SuccessStart(refServerInfo, bIsRunningProcess, bIsDBRun))
                {
                    bIsExistStateChanging = true;
                    continue;
                }

                // 서버 정지 중. 정지 성공
                if (CheckAndUpdate_SuccessTerminate(refServerInfo, bIsRunningProcess, bIsDBRun))
                {
                    bIsExistStateChanging = true;
                    continue;
                }

                // 서버 죽음
                if (node.Value.ProcessState == PROCESS_STATE.DIED)
                {
                    continue;
                }

                if (CheckAndUpdate_Died(refServerInfo, bIsRunningProcess))
                {
                    bIsExistStateChanging = true;
                    continue;
                }

                // 서버 처리가 멈춤 = No Update
                if (CheckAndUpdate_NoUpdate(refServerInfo, bIsRunningProcess, bIsDBRun))
                {
                    bIsExistStateChanging = true;
                    continue;
                }

                // 서버 처리가 정상화 됨
                if (CheckAndUpdate_Recorvery(refServerInfo, bIsRunningProcess, bIsDBRun))
                {
                    bIsExistStateChanging = true;
                    continue;
                }

                // 업데이트가 멈춘 서버 종료 됨. 종료 성공
                if (CheckAndUpdate_TerminateNoUpdate(refServerInfo, bIsRunningProcess))
                {
                    bIsExistStateChanging = true;
                    continue;
                }
            }

            return bIsExistStateChanging;
        }

        private bool CheckAndUpdate_SuccessStart(ServerInfo refServerInfo, bool bIsRunningProcess, bool bIsDBRun)
        {
            if (refServerInfo.ProcessState == PROCESS_STATE.STARTING && bIsRunningProcess == true && bIsDBRun == true)
            {
                refServerInfo.SetProcessRunning();
                m_refMonitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SUCCESS_START_SERVER);
                return true;
            }
            return false;
        }
        private bool CheckAndUpdate_SuccessTerminate(ServerInfo refServerInfo, bool bIsRunningProcess, bool bIsDBRun)
        {
            if (refServerInfo.ProcessState == PROCESS_STATE.TERMINATING && bIsRunningProcess == false && bIsDBRun == false)
            {
                refServerInfo.SetProcessTerminated();
                m_refMonitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SUCCESS_TERMINATE_SERVER);
                return true;
            }
            return false;
        }
        private bool CheckAndUpdate_Died(ServerInfo refServerInfo, bool bIsRunningProcess)
        {
            if ((refServerInfo.ProcessState == PROCESS_STATE.RUNNING || refServerInfo.ProcessState == PROCESS_STATE.STARTING) &&
                    bIsRunningProcess == false)
            {
                refServerInfo.SetProcessDied();
                m_refMonitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SERVER_DIED);
                return true;
            }
            return false;
        }
        private bool CheckAndUpdate_NoUpdate(ServerInfo refServerInfo, bool bIsRunningProcess, bool bIsDBRun)
        {
            if (refServerInfo.ProcessState == PROCESS_STATE.RUNNING && bIsRunningProcess == true && bIsDBRun == false)
            {
                refServerInfo.SetProcessNoUpdate();
                m_refMonitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SERVER_NOUPDATE);
                return true;
            }
            return false;
        }
        private bool CheckAndUpdate_Recorvery(ServerInfo refServerInfo, bool bIsRunningProcess, bool bIsDBRun)
        {
            if (refServerInfo.ProcessState == PROCESS_STATE.NOUPDATE && bIsRunningProcess == true && bIsDBRun == true)
            {
                refServerInfo.SetProcessRunning();
                m_refMonitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SERVER_RECOVERY_NOUPDATE);
                return true;
            }
            return false;
        }
        private bool CheckAndUpdate_TerminateNoUpdate(ServerInfo refServerInfo, bool bIsRunningProcess)
        {
            if (refServerInfo.ProcessState == PROCESS_STATE.NOUPDATE && bIsRunningProcess == false)
            {
                refServerInfo.SetProcessTerminated();
                m_refMonitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SUCCESS_TERMINATE_SERVER);
                return true;
            }
            return false;
        }

    }
}
