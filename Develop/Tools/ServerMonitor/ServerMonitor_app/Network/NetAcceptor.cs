using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using SMCommon;

namespace ServerMonitor
{
    class NetAcceptor
    {
        private readonly string ADDR_LOCAL = "127.0.0.1";

        public delegate UInt64 CreateSession(Socket socket);
        private CreateSession m_dlgtCreateSession;

        private NetCmdQueue m_refCmdQueue = null;
        private TcpListener m_tcpListener = null;
        private Thread m_threadAccept = null;

        public NetAcceptor(CreateSession dlgtCreateSession, NetCmdQueue refCmdQueue)
        {
            m_dlgtCreateSession = dlgtCreateSession;
            m_refCmdQueue = refCmdQueue;
        }

        public bool Start(int nPort)
        {
            if (m_tcpListener != null || m_threadAccept != null)
            {
                return false;
            }

            try
            {
                m_tcpListener = new TcpListener(IPAddress.Any, nPort);
                m_tcpListener.Start();

                m_threadAccept = new Thread(new ThreadStart(ProcAccept));
                m_threadAccept.Start();
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("실패! SKNetAcceptor구동에 실패했습니다. (IP:{0}, Port:{1}) 예외 메시지 : {2}", ADDR_LOCAL, nPort, e.Message);
                return false;
            }

            return true;
        }

        public void Stop()
        {
            if (m_tcpListener != null)
                m_tcpListener.Stop();
        }

        private void ProcAccept()
        {
            bool bRun = true;

            while (bRun)
            {
                try
                {
                    Socket socketClient = m_tcpListener.AcceptSocket();
                    if (socketClient.Connected == false) continue;
                    
                    UInt64 nSessionID = m_dlgtCreateSession(socketClient);

                    NetCommand command = new NetCommand(0, LocalCMD.ACCEPTED);
                    command.WriteUInt64(nSessionID);
                    command.MakingComplete();
                    m_refCmdQueue.PushCommand(command);
                }
                catch { bRun = false; }
            }
        }

    }
}
