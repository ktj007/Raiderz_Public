using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public class CommandSender
    {
        private NetClient m_refNetClient = null;

        public CommandSender(NetClient refNetClient)
        {
            m_refNetClient = refNetClient;
        }

        public virtual bool SendCheckCommandVersionReq(UInt64 nSessionID, int nCommandVersion)
        {
            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.CHECK_CMD_VERSION_REQ);
                if (commandSend.WriteInt32(nCommandVersion) == false) return false;
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }

        public virtual bool SendStartServer(UInt64 nSessionID, string strServerName)
        {
            if (nSessionID == 0 || strServerName.Length == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.START_SERVER_REQ);
                if (commandSend.WriteString(strServerName) == false) return false;
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }
        public virtual bool SendStartAllServer(UInt64 nSessionID)
        {
            if (nSessionID == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.START_ALL_SERVER_REQ);
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }
        public virtual bool SendToggleReviveServer(UInt64 nSessionID, string strServerName, bool bRevive)
        {
            if (nSessionID == 0 || strServerName.Length == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.SET_REVIVABLE_ATT_REQ);
                if (commandSend.WriteString(strServerName) == false) return false;
                if (commandSend.WriteBoolean(bRevive) == false) return false;
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }

        public virtual bool SendTerminateServer(UInt64 nSessionID, string strServerName)
        {
            if (nSessionID == 0 || strServerName.Length == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.TERMINATE_SERVER_REQ);
                if (commandSend.WriteString(strServerName) == false) return false;
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }
        public virtual bool SendTerminateAllServer(UInt64 nSessionID)
        {
            if (nSessionID == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.TERMINATE_ALL_SERVER_REQ);
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }

        public virtual bool SendRequestServerList(UInt64 nSessionID)
        {
            if (nSessionID == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.GET_SERVERLIST_REQ);
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }

        public virtual bool SendRequestServerInfo(UInt64 nSessionID, string[] arrServerNames)
        {
            if (nSessionID == 0 || arrServerNames.Length == 0) return false;

            try
            {
                NetCommand commandSend = new NetCommand(nSessionID, CMD.GET_SERVERINFO_REQ);
                if (commandSend.WriteByte((byte)arrServerNames.Length) == false) return false;
                for (int i = 0; i < arrServerNames.Length; ++i)
                {
                    if (commandSend.WriteString(arrServerNames[i]) == false) return false;
                }
                if (commandSend.MakingComplete() == false) return false;

                if (m_refNetClient.Post(commandSend) == false)
                    return false;
            }
            catch { return false; }
            return true;
        }

    }
}
