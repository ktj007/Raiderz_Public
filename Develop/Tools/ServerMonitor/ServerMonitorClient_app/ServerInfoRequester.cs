using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public class ServerInfoRequester
    {
        private readonly float DELAY_TO_REQUEST = 10.0f;

        private MachineInfoMgr m_refMachineInfoMgr = null;
        private CommandSender m_refCommandSender = null;
        private TimeRegulator m_timeRegulator = new TimeRegulator();

        public ServerInfoRequester(MachineInfoMgr refMachineInfoMgr, CommandSender refCommandSender)
        {
            m_refMachineInfoMgr = refMachineInfoMgr;
            m_refCommandSender = refCommandSender;

            m_timeRegulator.Start(DELAY_TO_REQUEST);
        }

        public void Update(float fDelta)
        {
            if (m_timeRegulator.IsReady(fDelta) == false)
                return;

            LinkedListNode<MachineInfo> node = m_refMachineInfoMgr.MachineInfoList.First;
            for (; node != null; node = node.Next)
            {
                if (false == node.Value.IsConnected) continue;

                ServerInfoMgr refServerInfoMgr = node.Value.ServerInfoMgr;
                string[] arrServerNames;
                refServerInfoMgr.GetAllServerName(out arrServerNames);

                if (arrServerNames.Length == 0) continue;

                m_refCommandSender.SendRequestServerInfo(node.Value.m_nSessionID, arrServerNames);
            }
        }

    }
}
