using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;
using SMCommon;

namespace ServerMonitorClient
{
    public class LocalCMD
    {
        public const ushort CONNECTED = 1;              ///< 연결 됨
        //  8   UInt64  세션 ID
        //  1   bool    접속 성공 여부

        public const ushort DISCONNECTED = 2;           ///< 연결 끊어짐
        //  8   UInt64  세션 ID
    }

    public class NetClient
    {
        private CommandHandler m_refCommandHandler = null;
        private NetSessionMgr m_sessionMgr = null;
        private NetCmdQueue m_commandQueue = new NetCmdQueue();

        public NetClient()
        {
            m_sessionMgr = new NetSessionMgr(m_commandQueue);
        }
        public void SetCommandHandler(CommandHandler refCommandHandler)
        {
            m_refCommandHandler = refCommandHandler;
        }

        public virtual bool NewClientSession(string strServerName, string strIP, int nPort, out UInt64 nSessionID)
        {
            nSessionID = 0;

            if (strServerName.Length == 0 && strIP.Length == 0) return false;

            if (strServerName.Length != 0 && strIP.Length == 0)
            {
                IPHostEntry host;
                try
                {
                    host = Dns.GetHostEntry(strServerName);
                    if (host.AddressList.Length == 0) return false;
                }
                catch (System.Exception e)
                {
                    FileLog.Instance.Write("Error! HostName={0}, Msg={1}", strServerName, e.Message);
                    return false;
                }

                IPAddress addr = host.AddressList[0];
                strIP = addr.ToString();
            }

            NetSession newSession = m_sessionMgr.CreateSession();
            if (newSession.RequestConnect(strIP, nPort) == false)
            {
                m_sessionMgr.DeleteSession(newSession.SessionID);
                return false;
            }
            nSessionID = newSession.SessionID;
            return true;
        }

        public virtual void DeleteClientSession(UInt64 nSessionID)
        {
            m_sessionMgr.DeleteSession(nSessionID);
        }

        public void Destroy()
        {
            m_sessionMgr.Destroy();
        }

        public void Update()
        {
            while (true)
            {
                NetCommand command = m_commandQueue.PopCommand();
                if (command == null) break;

                if (m_refCommandHandler.OnCommand(command) == false)
                    FileLog.Instance.Write("Error! Command 처리 오류. ID={0}", command.CommandID);
            }

            m_sessionMgr.Update();
        }

        public bool Post(NetCommand command)
        {
            if (command.SessionID == 0)
            {
                m_commandQueue.PushCommand(command);
            }
            else
            {
                NetSession session = m_sessionMgr.FindSession(command.SessionID);
                if (session == null) return false;

                if (session.Send(command.Buffer, command.BufferSize) == false)
                    return false;
            }

            return true;
        }

    }
}
