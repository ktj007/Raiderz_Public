using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor.DB;
using SMCommon;

namespace ServerMonitor
{
    class ServerDBInfoUpdater
    {
        public static readonly float TIME_UPDATE_DELAY_SEC = 5.0f;

        private ServerInfoMgr m_refServerInfoMgr = null;
        private DBMgr m_refDBMgr = null;
        private TimeRegulator m_rgltDelayToCheck = new TimeRegulator();

        public ServerDBInfoUpdater(ServerInfoMgr refServerInfoMgr, DBMgr refDBMgr)
        {
            m_refServerInfoMgr = refServerInfoMgr;
            m_refDBMgr = refDBMgr;

            m_rgltDelayToCheck.Start(TIME_UPDATE_DELAY_SEC);
        }

        public void Update(float fDelta)
        {
            if (false == m_rgltDelayToCheck.IsReady(fDelta)) return;
            if (false == m_refDBMgr.IsSyncDBOpen)
                if (!m_refDBMgr.CheckSyncDBReconnect()) return;

            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfo refServerInfo = node.Value;

                bool bIsRun = false;
                bool bIsServable = false;
                short nPlayerCount = 0;
                m_refDBMgr.GetServerStatusInfo_Sync(refServerInfo.WorldID, refServerInfo.ServerID
                    , out bIsRun
                    , out bIsServable
                    , out nPlayerCount);

                refServerInfo.SetDBInfo(bIsRun, bIsServable, nPlayerCount);
            }
        }

    }

}
