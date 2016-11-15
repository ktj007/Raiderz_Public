using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitor
{
    public class ServerReviver
    {
        private readonly float TIME_UPDATE_DELAY = 5.0f;

        private ServerInfoMgr m_refServerInfoMgr = null;
        private ServerController m_refServerController = null;
        private TimeRegulator m_timeRegulatorToUpdate = new TimeRegulator();

        public ServerReviver(ServerInfoMgr refServerInfoMgr, ServerController refServerController)
        {
            m_refServerInfoMgr = refServerInfoMgr;
            m_refServerController = refServerController;

            m_timeRegulatorToUpdate.Start(TIME_UPDATE_DELAY);
        }

        public bool Update(float fDelta)
        {
            if (m_timeRegulatorToUpdate.IsReady(fDelta) == false)
                return false;

            bool bTryRevive = false;

            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.ProcessState != PROCESS_STATE.DIED ||
                    node.Value.IsRevivable == false) continue;

                if (m_refServerController.StartServer(node.Value.ServerName) == false)
                    continue;

                bTryRevive = true;
            }

            return bTryRevive;
        }

        public bool ReviveAllRevivableServer()
        {
            bool bTryRevive = false;

            LinkedListNode<ServerInfo> node = m_refServerInfoMgr.ServerInfoList.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.ProcessState != PROCESS_STATE.DIED) continue;
                if (node.Value.IsRevivable == false) continue;

                if (m_refServerController.StartServer(node.Value.ServerName) == false)
                    continue;

                bTryRevive = true;
            }

            return bTryRevive;
        }

    }
}
