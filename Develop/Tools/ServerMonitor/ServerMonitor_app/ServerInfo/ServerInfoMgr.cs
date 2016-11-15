using System;
using System.Collections.Generic;
using System.Text;
using SMCommon;

namespace ServerMonitor
{
    #region Data Structures
    public struct SERVERLISTVIEWINFO
    {
        public string strServerName;
        public bool bRevivable;
        public PROCESS_STATE eProcessState;
        public SERVABLE_STATE eServableState;
        public SERVER_TYPE eServerType;
        public int nWorldID;
        public int nServerID;
        public int nCurPlayerCount;
        public DateTime timeLatestStart;
    }
    #endregion

    public class ServerInfoMgr
    {
        private LinkedList<ServerInfo> m_listServerInfo = new LinkedList<ServerInfo>();
        public delegate bool AddServerToConfig(SERVER_TYPE eServerType, string strServerExePath, string strProcessName);
        public delegate bool DeleteServerToConfig(string strServerExePath);

        private AddServerToConfig m_dlgtAddserverToConfig;
        private DeleteServerToConfig m_dlgtDeleteServerToConfig;

        public int ServerInfoCount { get { return m_listServerInfo.Count; } }
        public LinkedList<ServerInfo> ServerInfoList { get { return m_listServerInfo; } }

        public bool Init(List<ServerInfo> refListServerInfo, AddServerToConfig dlgtAddserverToConfig, DeleteServerToConfig dlgtDeleteServerToConfig)
        {
            for (int i = 0; i < refListServerInfo.Count; ++i)
            {
                m_listServerInfo.AddLast(refListServerInfo[i]);
            }
            m_dlgtAddserverToConfig = dlgtAddserverToConfig;
            m_dlgtDeleteServerToConfig = dlgtDeleteServerToConfig;
            return true;
        }
        public bool AddServerInfo(SERVER_TYPE eServerType, string strServerName, string strExePath, int nWorldID, int nServerID)
        {
            if (strServerName.Length == 0 || strExePath.Length == 0 || nWorldID == 0 || nServerID == 0)
                return false;

            if (IsExistServerInfo(strExePath) == true)
                return false;

            string strProcessName = System.IO.Path.GetFileNameWithoutExtension(strExePath);
            if (strProcessName.Length == 0)
                return false;

            if (m_dlgtAddserverToConfig(eServerType, strExePath, strProcessName) == false)
                return false;

            ServerInfo serverInfo = new ServerInfo(eServerType, strExePath, strProcessName, strServerName, nWorldID, nServerID);

            m_listServerInfo.AddLast(serverInfo);
            return true;
        }
        public bool DeleteServerInfo(string strServerName)
        {
            if (strServerName.Length == 0)
                return false;

            ServerInfo serverInfo = Find(strServerName);
            if (null == serverInfo)
                return false;

            if (m_dlgtDeleteServerToConfig(serverInfo.ExePath) == false)
                return false;

            return m_listServerInfo.Remove(serverInfo);
        }
        public bool IsExistServerInfo(string strExePath)
        {
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.ExePath.Equals(strExePath) == true)
                    return true;
            }
            return false;
        }
        public bool IsRevivableServer(string strServerName)
        {
            ServerInfo serverInfo = Find(strServerName);
            if (null == serverInfo)
                return false;

            if (serverInfo.ProcessState == PROCESS_STATE.DIED && serverInfo.IsRevivable)
                return true;

            return false;
        }

        public ServerInfo Find(string strServerName)
        {
            if (strServerName.Length == 0)
                return null;

            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                if (node.Value.ServerName.Equals(strServerName) == true)
                {
                    return node.Value;
                }
            }
            return null;
        }

        #region getter
        public void GetSERVERLISTVIEWINFO(ref SERVERLISTVIEWINFO[] refArrServerListViewInfo)
        {
            int i = 0;
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                GetSERVERLISTVIEWINFO(node.Value.ServerName, ref refArrServerListViewInfo[i]);
                i++;
            }
        }
        public bool GetSERVERLISTVIEWINFO(string strServerName, ref SERVERLISTVIEWINFO refServerListViewInfo)
        {
            ServerInfo serverInfo = Find(strServerName);
            if (null == serverInfo)
                return false;

            refServerListViewInfo.strServerName = serverInfo.ServerName;
            refServerListViewInfo.bRevivable = serverInfo.IsRevivable;
            refServerListViewInfo.eProcessState = serverInfo.ProcessState;
            refServerListViewInfo.eServableState = serverInfo.ServableState;
            refServerListViewInfo.eServerType = serverInfo.Type;
            refServerListViewInfo.nWorldID = serverInfo.WorldID;
            refServerListViewInfo.nServerID = serverInfo.ServerID;
            refServerListViewInfo.nCurPlayerCount = serverInfo.CurPlayerCount;
            refServerListViewInfo.timeLatestStart = serverInfo.LatestStartTime;
            return true;
        }
        public void GetTRANS_SERVERLISTNODE(ref TRANS_SERVERLISTNODE[] refArrTransServerListNode)
        {
            int i = 0;
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                refArrTransServerListNode[i].strServerName = node.Value.ServerName;
                refArrTransServerListNode[i].nServerType = (byte)node.Value.Type;
                refArrTransServerListNode[i].nWorldID = node.Value.WorldID;
                refArrTransServerListNode[i].nServerID = node.Value.ServerID;
                i++;
            }
        }
        public void GetTRANS_SERVERINFONODE(ref TRANS_SERVERINFONODE[] refArrTransServerInfo)
        {
            int i = 0;
            LinkedListNode<ServerInfo> node = m_listServerInfo.First;
            for (; node != null; node = node.Next)
            {
                GetTRANS_SERVERINFONODE(node.Value.ServerName, ref refArrTransServerInfo[i]);
                i++;
            }
        }
        public bool GetTRANS_SERVERINFONODE(string strServerName, ref TRANS_SERVERINFONODE refTransServerInfo)
        {
            ServerInfo serverInfo = Find(strServerName);
            if (null == serverInfo)
                return false;

            refTransServerInfo.strServerName = serverInfo.ServerName;
            refTransServerInfo.nProcessState = (byte)serverInfo.ProcessState;
            refTransServerInfo.nServableState = (byte)serverInfo.ServableState;
            refTransServerInfo.bRevivable = serverInfo.IsRevivable;
            refTransServerInfo.nCurServerPlayerCount = serverInfo.CurPlayerCount;
            return true;
        }


        public bool IsServerRevivable(string strServerName)
        {
            ServerInfo serverInfo = Find(strServerName);
            if (null == serverInfo)
                return false;

            return serverInfo.IsRevivable;
        }
        public bool GetProcessState(string strServerName, out PROCESS_STATE eProcessState)
        {
            eProcessState = PROCESS_STATE.TERMINATED;
            ServerInfo serverInfo = Find(strServerName);
            if (null == serverInfo)
                return false;

            eProcessState = serverInfo.ProcessState;
            return true;
        }
        #endregion

    }
}
