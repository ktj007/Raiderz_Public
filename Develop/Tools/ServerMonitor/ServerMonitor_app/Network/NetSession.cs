using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using SMCommon;

namespace ServerMonitor
{
    public class NetSession
    {
        private UInt64 m_sessionID = 0;
        private Socket m_socket = null;
        private NetCmdQueue m_refCmdQueue = null;

        private NetworkStream m_netStream = null;
        private NetRecvStream m_recvStream = null;
        private Thread m_threadReceiver = null;
        private bool m_bDestroyed = false;

        public bool Destroyed { get { return m_bDestroyed; } }

        public NetSession(UInt64 sessionID, Socket socket, NetCmdQueue refCmdQueue)
        {
            m_sessionID = sessionID;
            m_socket = socket;
            m_refCmdQueue = refCmdQueue;

            m_netStream = new NetworkStream(socket);
            m_recvStream = new NetRecvStream(sessionID, m_netStream);
        }

        public bool Start()
        {
            m_threadReceiver = new Thread(new ThreadStart(_ThreadReceiver));
            m_threadReceiver.Start();
            return true;
        }

        public void Close()
        {
            m_threadReceiver.Abort();
            m_netStream.Close();
            m_recvStream.Close();
            m_socket.Close();
        }

        public bool Send(byte[] buffer, int nLength)
        {
            try
            {
                m_netStream.Write(buffer, 0, nLength);
                m_netStream.Flush();
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("예외! NetSession.Send(), 예외 메시지 : {0}", e.Message);
                return false;
            }
            return true;
        }

        private void _ThreadReceiver()
        {
            while (true)
            {
                try
                {
                    if (m_recvStream.Recv() == false)
                    {
                        break;
                    }

                    NetCommand command;
                    while (m_recvStream.MakeCommand(out command) == true)
                    {
                        m_refCmdQueue.PushCommand(command);
                    }
                }
                catch (System.Exception e)
                {
                    FileLog.Instance.Write("예외! _ThreadReceiver(), 예외 메시지 : {0}", e.Message);
                    break;
                }
            }

            PostDisconnected();
            m_bDestroyed = true;
        }

        private void PostDisconnected()
        {
            NetCommand command = new NetCommand(0, LocalCMD.DISCONNECTED);
            command.WriteUInt64(m_sessionID);
            command.MakingComplete();
            m_refCmdQueue.PushCommand(command);
        }

    }
}
