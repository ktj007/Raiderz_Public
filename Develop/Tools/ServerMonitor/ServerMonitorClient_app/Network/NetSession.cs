using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using SMCommon;

namespace ServerMonitorClient
{
    class NetSession
    {
        private UInt64 m_nSessionID = 0;
        private TcpClient m_client = new TcpClient();
        private Thread m_threadReceiver = null;
        private NetworkStream m_netStream = null;

        private NetCmdQueue m_refCommandQueue = null;
        private NetRecvStream m_recvStream = null;
        private bool m_bDestroyed = false;

        public UInt64 SessionID { get { return m_nSessionID; } }
        public bool Destroyed { get { return m_bDestroyed; } }

        public NetSession(UInt64 nSessionID, NetCmdQueue refCommandQueue)
        {
            m_nSessionID = nSessionID;
            m_refCommandQueue = refCommandQueue;
        }

        public bool RequestConnect(string strAddr, int nPort)
        {
            try
            {
                IPAddress addrServer = IPAddress.Parse(strAddr);
                m_client.BeginConnect(addrServer, nPort, _CallbackAsyncConnect, null);
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("실패! 비동기 연결 시작에 실패했습니다. (Addr:{0}, Port:{1}), 예외 메시지 : {2}", strAddr, nPort, e.Message);
                return false;
            }
            return true;
        }

        public bool Send(byte[] buffer, int nCLength)
        {
            try
            {
                m_netStream.Write(buffer, 0, nCLength);
                m_netStream.Flush();
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("예외! SKNetSession.Send(), 메시지 : {0}", e.Message);
                return false;
            }
            return true;
        }

        public void Close()
        {
            if (m_netStream != null)
                m_netStream.Close();

            if (m_recvStream != null)
                m_recvStream.Close();

            m_client.Close();
        }

        private void _CallbackAsyncConnect(IAsyncResult ar)
        {
            try
            {
                m_client.EndConnect(ar);

                m_netStream = m_client.GetStream();
                m_recvStream = new NetRecvStream(m_nSessionID, m_netStream);

                PostConnected(true);
                m_threadReceiver = new Thread(new ThreadStart(_ThreadReceiver));
                m_threadReceiver.Start();
            }
            catch (System.Exception e)
            {
                FileLog.Instance.Write("실패! 비동기 연결 완료에 실패했습니다. 예외 메시지 : {0}", e.Message);
                PostConnected(false);
            }
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
                        m_refCommandQueue.PushCommand(command);
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

        private void PostConnected(bool bIsSuccess)
        {
            NetCommand command = new NetCommand(0, LocalCMD.CONNECTED);
            command.WriteUInt64(m_nSessionID);
            command.WriteBoolean(bIsSuccess);
            command.MakingComplete();
            m_refCommandQueue.PushCommand(command);
        }

        private void PostDisconnected()
        {
            NetCommand command = new NetCommand(0, LocalCMD.DISCONNECTED);
            command.WriteUInt64(m_nSessionID);
            command.MakingComplete();
            m_refCommandQueue.PushCommand(command);
        }

    }
}
