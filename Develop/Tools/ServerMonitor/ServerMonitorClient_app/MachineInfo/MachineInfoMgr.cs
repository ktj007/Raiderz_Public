using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitorClient
{
    public class MachineInfoMgr
    {
        private LinkedList<MachineInfo> m_listMachineInfo = new LinkedList<MachineInfo>();

        public LinkedList<MachineInfo> MachineInfoList { get { return m_listMachineInfo; } }
        public int MachineInfoCount { get { return m_listMachineInfo.Count; } }

        public void AddMachineInfo(MachineInfo[] arrMachineInfo)
        {
            for (int i = 0; i < arrMachineInfo.Length; ++i)
            {
                m_listMachineInfo.AddLast(arrMachineInfo[i]);
            }
        }
        public void Clear()
        {
            m_listMachineInfo.Clear();
        }

        public MachineInfo Find(string strMachineName)
        {
            if (strMachineName.Length == 0) return null;

            LinkedListNode<MachineInfo> node = m_listMachineInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.MachineName.Equals(strMachineName) == true)
                {
                    return node.Value;
                }
            }
            return null;
        }
        public MachineInfo Find(UInt64 nSessionID)
        {
            if (nSessionID == 0) return null;

            LinkedListNode<MachineInfo> node = m_listMachineInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.m_nSessionID == nSessionID)
                {
                    return node.Value;
                }
            }
            return null;
        }

        
        public ServerInfoMgr GetServerInfoMgr(string strMachineName)
        {
            MachineInfo machineInfo = Find(strMachineName);
            if (machineInfo == null) return null;

            return machineInfo.ServerInfoMgr;
        }
        public int GetMachinePlayerCount(string strMachineName)
        {
            MachineInfo machineInfo = Find(strMachineName);
            if (machineInfo == null) return 0;

            int nPlayerCount = 0;

            ServerInfoMgr refServerInfoMgr = machineInfo.ServerInfoMgr;
            if (refServerInfoMgr.HasMasterServer() == true)
            {
                nPlayerCount = refServerInfoMgr.GetAllGameServerPlayerCount();
            }
            else
            {
                nPlayerCount = refServerInfoMgr.GetAllServerPlayerCount();
            }

            return nPlayerCount;
        }


    }
}
