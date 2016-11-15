using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public class MachineConnector
    {
        public static readonly float DELAY_TO_REQUEST = 60.0f;

        private MachineInfoMgr m_refMachineInfoMgr = null;
        private NetClient m_refNetClient = null;
        private TimeRegulator m_timeRegulator = new TimeRegulator();

        public MachineConnector(MachineInfoMgr refMachineInfoMgr, NetClient refNetClient)
        {
            m_refMachineInfoMgr = refMachineInfoMgr;
            m_refNetClient = refNetClient;

            m_timeRegulator.Start(DELAY_TO_REQUEST);
        }

        public void Update(float fDelta)
        {
            if (m_timeRegulator.IsReady(fDelta) == true)
            {
                ConnectToAllMachines();
            }
        }

        public void ConnectToAllMachines()
        {
            LinkedListNode<MachineInfo> node = m_refMachineInfoMgr.MachineInfoList.First;
            for (; node != null; node = node.Next)
            {
                MachineInfo refMachineInfo = node.Value;

                if (false == refMachineInfo.IsConnectable) continue;

                UInt64 nSessionID;
                if (m_refNetClient.NewClientSession(refMachineInfo.MachineName, refMachineInfo.IP, refMachineInfo.Port, out nSessionID) == false)
                    continue;

                refMachineInfo.SetRequestConnect(nSessionID);
            }
        }

        public void DisconnectAllMachines()
        {
            LinkedListNode<MachineInfo> node = m_refMachineInfoMgr.MachineInfoList.First;
            for (; node != null; node = node.Next)
            {
                if (false == node.Value.IsConnected) continue;

                m_refNetClient.DeleteClientSession(node.Value.m_nSessionID);
            }
        }

    }
}
