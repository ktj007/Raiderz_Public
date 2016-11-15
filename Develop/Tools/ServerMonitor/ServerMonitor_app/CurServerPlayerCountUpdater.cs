using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor.DB;
using SMCommon;

namespace ServerMonitor
{
    class CurServerPlayerCountUpdater
    {
        private readonly float TIME_GET_DELAY = 10.0f;

        private DBMgr m_refDBMgr = null;
        private ServerInfoMgr m_refServerInfoMgr = null;

        private TimeRegulator m_timeRegulatorGetCurPlayer = new TimeRegulator();

        public CurServerPlayerCountUpdater(DBMgr refDBMgr, ServerInfoMgr refServerInfoMgr)
        {
            m_refDBMgr = refDBMgr;
            m_refServerInfoMgr = refServerInfoMgr;

            m_timeRegulatorGetCurPlayer.Start(TIME_GET_DELAY);
        }

        public void Update(float fDelta)
        {
            if (m_timeRegulatorGetCurPlayer.IsReady(fDelta) == false)
                return;

            if (m_refDBMgr.IsSyncDBOpen == false)
                return;


            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                if (SERVER_STATE.RUNNING != node.Value.eServerState) continue;

                int nCurPlayerCount = 0;
                if (m_refDBMgr.GetServerCurServerPlayerCount_Sync(node.Value.WorldID, node.Value.ServerID, out nCurPlayerCount) == false)
                    continue;

                node.Value.CurPlayerCount = nCurPlayerCount;
            }
        }

    }
}
