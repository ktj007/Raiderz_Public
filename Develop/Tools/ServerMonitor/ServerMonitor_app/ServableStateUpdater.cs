using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor.DB;
using SMCommon;

namespace ServerMonitor
{
    public class ServableStateUpdater
    {
        public static readonly float TIME_CHECK_DELAY = 5.0f;

        private ServerInfoMgr m_refServerInfoMgr = null;
        private MonitoringLoggerMgr m_refMinitoringLoggerMgr = null;
        private TimeRegulator m_rgltDelayToCheck = new TimeRegulator();

        public ServableStateUpdater(ServerInfoMgr refServerInfoMgr, MonitoringLoggerMgr refMonitoringLoggerMgr)
        {
            m_refServerInfoMgr = refServerInfoMgr;
            m_refMinitoringLoggerMgr = refMonitoringLoggerMgr;

            m_rgltDelayToCheck.Start(TIME_CHECK_DELAY);
        }

        public bool Update(float fDelta)
        {
            if (false == m_rgltDelayToCheck.IsReady(fDelta)) return false;

            bool bIsChangineState = false;

            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfo refServerInfo = node.Value;

                if (refServerInfo.ProcessState != PROCESS_STATE.RUNNING) continue;

                SERVABLE_STATE eServableState = (refServerInfo.IsServable ? SERVABLE_STATE.ACTIVE : SERVABLE_STATE.INACTIVE);
                
                if (refServerInfo.ServableState == eServableState) continue;

                if (SERVABLE_STATE.INACTIVE == refServerInfo.ServableState &&
                    SERVABLE_STATE.ACTIVE == eServableState)
                {
                    m_refMinitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SERVER_ACTIVE);
                }
                else if (SERVABLE_STATE.ACTIVE == refServerInfo.ServableState &&
                       SERVABLE_STATE.INACTIVE == eServableState)
                {
                    m_refMinitoringLoggerMgr.Write(refServerInfo.ServerName, refServerInfo.WorldID, refServerInfo.ServerID, LogID.SERVER_INACTIVE);
                }

                refServerInfo.SetServableState(eServableState);
                bIsChangineState = true;
            }

            return bIsChangineState;
        }

    }
}
