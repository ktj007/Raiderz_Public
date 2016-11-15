using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitor
{
    public class CommandSender
    {
        private NetServer m_refNetServer = null;

        public CommandSender(NetServer refNetServer)
        {
            m_refNetServer = refNetServer;
        }

        public bool SendCheckCommandVersionRes(UInt64 nSessionID, bool bSuccess)
        {
            NetCommand commandSend = new NetCommand(nSessionID, CMD.CHECK_CMD_VERSION_RES);
            if (commandSend.WriteBoolean(bSuccess) == false) return false;

            if (commandSend.MakingComplete() == false) return false;

            if (m_refNetServer.Send(commandSend) == false)
                return false;

            return true;
        }

        public bool SendGetServerListRes(UInt64 nSessionID, ref TRANS_SERVERLISTNODE[] arrServerListNodes)
        {
            NetCommand commandSend = new NetCommand(nSessionID, CMD.GET_SERVERLIST_RES);
            if (commandSend.WriteByte((byte)arrServerListNodes.Length) == false) return false;
            for (int i = 0; i < arrServerListNodes.Length; ++i)
            {
                if (commandSend.WriteString(arrServerListNodes[i].strServerName) == false) return false;
                if (commandSend.WriteByte((byte)arrServerListNodes[i].nServerType) == false) return false;
                if (commandSend.WriteInt32(arrServerListNodes[i].nWorldID) == false) return false;
                if (commandSend.WriteInt32(arrServerListNodes[i].nServerID) == false) return false;
            }

            if (commandSend.MakingComplete() == false) return false;

            if (m_refNetServer.Send(commandSend) == false)
                return false;

            return true;
        }

        public bool SendGetServerInfoRes(UInt64 nSessionID, ref TRANS_SERVERINFONODE[] arrServerInfo)
        {
            NetCommand commandSend = new NetCommand(nSessionID, CMD.GET_SERVERINFO_RES);
            if (commandSend.WriteByte((byte)arrServerInfo.Length) == false) return false;
            for (int i = 0; i < arrServerInfo.Length; ++i)
            {
                if (commandSend.WriteString(arrServerInfo[i].strServerName) == false) return false;
                if (commandSend.WriteByte((byte)arrServerInfo[i].nProcessState) == false) return false;
                if (commandSend.WriteByte((byte)arrServerInfo[i].nServableState) == false) return false;
                if (commandSend.WriteBoolean(arrServerInfo[i].bRevivable) == false) return false;
                if (commandSend.WriteInt32(arrServerInfo[i].nCurServerPlayerCount) == false) return false;
            }
            
            if (commandSend.MakingComplete() == false) return false;

            if (m_refNetServer.Send(commandSend) == false)
                return false;

            return true;
        }

        public bool SendSetverMonitoringLog(UInt64 nSessionID, string strDateTime, string strServerName, int nWorldID, int nServerID, string strLogString)
        {
            NetCommand commandSend = new NetCommand(nSessionID, CMD.MONITORING_LOG);
            if (commandSend.WriteString(strDateTime) == false) return false;
            if (commandSend.WriteString(strServerName) == false) return false;
            if (commandSend.WriteInt32(nWorldID) == false) return false;
            if (commandSend.WriteInt32(nServerID) == false) return false;
            if (commandSend.WriteString(strLogString) == false) return false;

            if (commandSend.MakingComplete() == false) return false;

            if (m_refNetServer.Send(commandSend) == false)
                return false;

            return true;
        }

    }
}
