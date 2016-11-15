using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitor
{
    public class ServerMonitorClient
    {
        public ServerMonitorClient(UInt64 nSessionID)
        {
            m_SessionID = nSessionID;
        }

        public UInt64 m_SessionID;
        public bool m_Reliable = false;
    }

    public class ServerMonitorClientMgr
    {
        private LinkedList<ServerMonitorClient> m_listServerMonitorClient = new LinkedList<ServerMonitorClient>();

        public int SessionCount { get { return m_listServerMonitorClient.Count; } }

        public void AddServerMonitorClient(UInt64 nSessionID)
        {
            ServerMonitorClient serverMonitorClient = new ServerMonitorClient(nSessionID);
            m_listServerMonitorClient.AddLast(serverMonitorClient);
        }
        public void DeleteServerMonitorClient(UInt64 nSessionID)
        {
            ServerMonitorClient node = FindSessionNode(nSessionID);
            if (null == node) return;

            m_listServerMonitorClient.Remove(node);
        }
        public void SetReliableClient(UInt64 nSessionID)
        {
            ServerMonitorClient node = FindSessionNode(nSessionID);
            if (null == node) return;

            node.m_Reliable = true;
        }
        public bool IsReliableClient(UInt64 nSessionID)
        {
            ServerMonitorClient node = FindSessionNode(nSessionID);
            if (null == node) return false;

            return node.m_Reliable;
        }

        public void GetAllSessionID(out UInt64[] arrSessionID)
        {
            arrSessionID = new UInt64[m_listServerMonitorClient.Count];

            int i = 0;
            LinkedListNode<ServerMonitorClient> node = m_listServerMonitorClient.First;
            for (; node != null; node = node.Next)
            {
                arrSessionID[i] = node.Value.m_SessionID;
                i++;
            }
        }

        private ServerMonitorClient FindSessionNode(UInt64 nSessionID)
        {
            LinkedListNode<ServerMonitorClient> node = m_listServerMonitorClient.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.m_SessionID == nSessionID)
                {
                    return node.Value;
                }
            }
            return null;
        }

    }
}
