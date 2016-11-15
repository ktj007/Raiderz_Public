using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using SMCommon;

namespace ServerMonitor
{
    class CommandHandler
    {
        ServerMonitor m_refServerMonitor = null;
        CommandSender m_refCommandSender = null;

        public CommandHandler(ServerMonitor refServerMonitor, CommandSender refComamndSender)
        {
            m_refServerMonitor = refServerMonitor;
            m_refCommandSender = refComamndSender;
        }

        public bool OnCommand(NetCommand command)
        {
            if (false == CheckReliableCommand(command))
            {
                m_refServerMonitor.DisconnectSession(command.SessionID);
                return false;
            }

            switch (command.CommandID)
            {
                case LocalCMD.ACCEPTED:
                    return OnAccepted(command);
                case LocalCMD.DISCONNECTED:
                    return OnDisconnected(command);
                case CMD.CHECK_CMD_VERSION_REQ:
                    return OnCheckCommandVersionReq(command);
                case CMD.START_SERVER_REQ:
                    return OnStartServerReq(command);
                case CMD.TERMINATE_SERVER_REQ:
                    return OnTerminateServerReq(command);
                case CMD.SET_REVIVABLE_ATT_REQ:
                    return OnSetRevivableAttReq(command);
                case CMD.GET_SERVERLIST_REQ:
                    return OnGetServerListReq(command);
                case CMD.GET_SERVERINFO_REQ:
                    return OnGetServerInfoReq(command);
                case CMD.START_ALL_SERVER_REQ:
                    return OnStartAllServerReq(command);
                case CMD.TERMINATE_ALL_SERVER_REQ:
                    return OnTerminatetAllServerReq(command);
            }
            return true;
        }

        public bool OnAccepted(NetCommand command)
        {
            UInt64 nSessionID;

            if (command.ReadUInt64(out nSessionID) == false) return false;

            m_refServerMonitor.OnAccepted(nSessionID);

            return true;
        }

        public bool OnDisconnected(NetCommand command)
        {
            UInt64 nSessionID;

            if (command.ReadUInt64(out nSessionID) == false) return false;

            m_refServerMonitor.OnDisconnected(nSessionID);

            return true;
        }

        public bool OnCheckCommandVersionReq(NetCommand command)
        {
            int nCommadVersion = 0;

            if (command.ReadInt32(out nCommadVersion) == false) return false;

            if (CMD.COMMAND_VERSION == nCommadVersion)
            {
                m_refCommandSender.SendCheckCommandVersionRes(command.SessionID, true);
                m_refServerMonitor.OnSetReliableClient(command.SessionID);
            }
            else
            {
                m_refCommandSender.SendCheckCommandVersionRes(command.SessionID, false);
                m_refServerMonitor.DisconnectSession(command.SessionID);
            }

            return true;
        }

        public bool OnStartServerReq(NetCommand command)
        {
            string strServerName;

            if (command.ReadString(out strServerName) == false) return false;


            if (null == m_refServerMonitor.refServerInfoMgr.Find(strServerName))
                return true;

            m_refServerMonitor.StartServer(strServerName);

            TRANS_SERVERINFONODE[] serverInfo = new TRANS_SERVERINFONODE[1];
            if (false == m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERINFONODE(strServerName, ref serverInfo[0]))
                return false;

            if (false == m_refCommandSender.SendGetServerInfoRes(command.SessionID, ref serverInfo))
                return false;

            return true;
        }

        public bool OnTerminateServerReq(NetCommand command)
        {
            string strServerName;

            if (command.ReadString(out strServerName) == false) return false;


            if (null == m_refServerMonitor.refServerInfoMgr.Find(strServerName))
                return true;

            m_refServerMonitor.TerminateServer(strServerName);


            TRANS_SERVERINFONODE[] serverInfo = new TRANS_SERVERINFONODE[1];
            if (m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERINFONODE(strServerName, ref serverInfo[0]) == false)
                return false;

            if (m_refCommandSender.SendGetServerInfoRes(command.SessionID, ref serverInfo) == false)
                return false;

            return true;
        }

        public bool OnSetRevivableAttReq(NetCommand command)
        {
            string strServerName;
            bool bRevivable;

            if (command.ReadString(out strServerName) == false) return false;
            if (command.ReadBoolean(out bRevivable) == false) return false;

            m_refServerMonitor.SetServerRevivable(strServerName, bRevivable);
            
            TRANS_SERVERINFONODE[] serverInfo = new TRANS_SERVERINFONODE[1];
            if (m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERINFONODE(strServerName, ref serverInfo[0]) == false)
                return false;

            if (m_refCommandSender.SendGetServerInfoRes(command.SessionID, ref serverInfo) == false)
                return false;

            return true;
        }

        public bool OnGetServerListReq(NetCommand command)
        {
            TRANS_SERVERLISTNODE[] arrServerListNode = new TRANS_SERVERLISTNODE[m_refServerMonitor.refServerInfoMgr.ServerInfoCount];
            m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERLISTNODE(ref arrServerListNode);

            if (m_refCommandSender.SendGetServerListRes(command.SessionID, ref arrServerListNode) == false)
                return false;

            return true;
        }

        public bool OnGetServerInfoReq(NetCommand command)
        {
            byte nServerNameCount;
            string[] arrServerNames;

            if (command.ReadByte(out nServerNameCount) == false) return false;

            arrServerNames = new string[nServerNameCount];
            for (int i = 0; i < nServerNameCount; ++i)
            {
                if (command.ReadString(out arrServerNames[i]) == false) return false;
            }


            TRANS_SERVERINFONODE serverInfo = new TRANS_SERVERINFONODE();
            List<TRANS_SERVERINFONODE> listServerInfo = new List<TRANS_SERVERINFONODE>();
            for (int i = 0; i < nServerNameCount; ++i)
            {
                if (m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERINFONODE(arrServerNames[i], ref serverInfo) == false)
                    continue;

                listServerInfo.Add(serverInfo);
            }

            TRANS_SERVERINFONODE[] arrServerInfo = new TRANS_SERVERINFONODE[listServerInfo.Count];
            listServerInfo.CopyTo(arrServerInfo);

            if (m_refCommandSender.SendGetServerInfoRes(command.SessionID, ref arrServerInfo) == false)
                return false;

            return true;
        }

        private bool OnStartAllServerReq(NetCommand command)
        {
            m_refServerMonitor.StartAllServer();

            TRANS_SERVERINFONODE[] serverInfo = new TRANS_SERVERINFONODE[m_refServerMonitor.refServerInfoMgr.ServerInfoCount];
            m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERINFONODE(ref serverInfo);

            if (false == m_refCommandSender.SendGetServerInfoRes(command.SessionID, ref serverInfo))
                return false;

            return true;
        }

        private bool OnTerminatetAllServerReq(NetCommand command)
        {
            m_refServerMonitor.TerminateAllServer();

            TRANS_SERVERINFONODE[] serverInfo = new TRANS_SERVERINFONODE[m_refServerMonitor.refServerInfoMgr.ServerInfoCount];
            m_refServerMonitor.refServerInfoMgr.GetTRANS_SERVERINFONODE(ref serverInfo);

            if (m_refCommandSender.SendGetServerInfoRes(command.SessionID, ref serverInfo) == false)
                return false;

            return true;
        }


        //////////////////////////////////////////////////////////////////////////
        /// private
        private bool CheckReliableCommand(NetCommand command)
        {
            if (true == m_refServerMonitor.refServerMonitorClientMgr.IsReliableClient(command.SessionID))
                return true;

            if (LocalCMD.ACCEPTED == command.CommandID ||
                LocalCMD.DISCONNECTED == command.CommandID ||
                CMD.CHECK_CMD_VERSION_REQ == command.CommandID)
                return true;

            return false;
        }

    }
}
