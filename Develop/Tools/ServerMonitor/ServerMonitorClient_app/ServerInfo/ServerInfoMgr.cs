using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public class ServerInfoMgr
    {
        private LinkedList<ServerInfo> m_listServerInfo = new LinkedList<ServerInfo>();

        public LinkedList<ServerInfo> ServerInfoList { get { return m_listServerInfo; } }
        public int ServerInfoCount { get { return m_listServerInfo.Count; } }

        public void Clear()
        {
            m_listServerInfo.Clear();
        }
        public bool AddServerInfo(ref TRANS_SERVERLISTNODE refServerListNode)
        {
            ServerInfo serverInfo = new ServerInfo();

            serverInfo.SetName(refServerListNode.strServerName);
            serverInfo.SetServerType((SERVER_TYPE)refServerListNode.nServerType);
            serverInfo.WorldID = refServerListNode.nWorldID;
            serverInfo.ServerID = refServerListNode.nServerID;
            m_listServerInfo.AddLast(serverInfo);
            return true;
        }
        public ServerInfo Find(string strServerName)
        {
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.GetName().Equals(strServerName) == true)
                {
                    return node.Value;
                }
            }
            return null;
        }

        public bool HasMasterServer()
        {
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.GetServerType() == SERVER_TYPE.SERVER_MASTER)
                    return true;
            }
            return false;
        }
        public void GetAllServerName(out string[] arrServerNames)
        {
            arrServerNames = new string[m_listServerInfo.Count];

            int nIndex = 0;
            List<string> listServerName = new List<string>();
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                arrServerNames[nIndex] = node.Value.GetName();
                nIndex++;
            }
        }

        public int GetAllServerPlayerCount()
        {
            int nAllServerPlayerCount = 0;
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                nAllServerPlayerCount += node.Value.CurPlayerCount;
            }
            return nAllServerPlayerCount;
        }
        public int GetAllGameServerPlayerCount()
        {
            int nAllGameServerPlayerCount = 0;
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.GetServerType() == SERVER_TYPE.SERVER_GAME)
                    nAllGameServerPlayerCount += node.Value.CurPlayerCount;
            }
            return nAllGameServerPlayerCount;
        }
        
        public void SetServerCurServerPlayerCount(string strServerName, int nCurServerPlayerCount)
        {
            ServerInfo serverInfo = Find(strServerName);
            if (serverInfo == null) return;

            serverInfo.CurPlayerCount = nCurServerPlayerCount;
        }

    }
}
