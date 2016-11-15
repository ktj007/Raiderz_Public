using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using SMCommon;

namespace ServerMonitor
{
    public class LocalCMD
    {
        public const ushort ACCEPTED = 1;              ///< 연결 됨
        //  8   UInt64  세션 ID

        public const ushort DISCONNECTED = 2;           ///< 연결 끊어짐
        //  8   UInt64  세션 ID
    }

    public class NetServer
    {
        private NetAcceptor m_acceptor = null;
        private NetSessionMgr m_sessionMgr = new NetSessionMgr();
        private NetCmdQueue m_commandQueue = new NetCmdQueue();
        private CommandSender m_commandSender = null;
        private CommandHandler m_commandHandler = null;

        public CommandSender commandSender { get { return m_commandSender; } }

        public NetServer(ServerMonitor refServerMonitor)
        {
            m_acceptor = new NetAcceptor(CreateSession, m_commandQueue);
            m_commandSender = new CommandSender(this);
            m_commandHandler = new CommandHandler(refServerMonitor, m_commandSender);
        }

        public bool Start(int nPort)
        {
            if (m_acceptor.Start(nPort) == false)
                return false;

            return true;
        }

        public void Stop()
        {
            m_acceptor.Stop();
            m_sessionMgr.Destroy();
        }

        public void Update()
        {
            while(true)
            {
                NetCommand command = m_commandQueue.PopCommand();
                if (command == null) break;

                if (m_commandHandler.OnCommand(command) == false)
                {
                    FileLog.Instance.Write("Error! NetServer.Update(), SessionID={0}, CommandID={1}", command.SessionID, command.CommandID);
                }

            }

            m_sessionMgr.Update();
        }

        public UInt64 CreateSession(Socket socket)
        {
            return m_sessionMgr.CreateSession(socket, m_commandQueue);
        }
        public void DeleteSession(UInt64 nSessionID)
        {
            m_sessionMgr.DeleteSession(nSessionID);
        }

        public bool Send(NetCommand command)
        {
            NetSession sessionReceiver = m_sessionMgr.FindSession(command.SessionID);
            if (null == sessionReceiver) return false;

            if (sessionReceiver.Send(command.Buffer, command.BufferSize) == false)
                return false;

            return true;
        }
    }
}
