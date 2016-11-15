using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using SMCommon;

namespace ServerMonitor
{
    class NetSessionMgr
    {
        private SortedDictionary<UInt64, NetSession> m_dicSession = new SortedDictionary<UInt64, NetSession>();
        private UInt64 m_SessionIDGenerator = 1;

        public int SessionCount { get { return m_dicSession.Count; } }

        public UInt64 CreateSession(Socket socket, NetCmdQueue refCmdQueue)
        {
            UInt64 nNewSessionID = 0;
            lock (m_dicSession)
            {
                nNewSessionID = GetNewSessionID();
                NetSession newSession = new NetSession(nNewSessionID, socket, refCmdQueue);

                newSession.Start();

                m_dicSession.Add(nNewSessionID, newSession);
            }
            return nNewSessionID;
        }
        public void DeleteSession(UInt64 nSessionID)
        {
            NetSession sessionReceiver = FindSession(nSessionID);
            if (null == sessionReceiver) return;

            sessionReceiver.Close();
            lock (m_dicSession)
            {
                m_dicSession.Remove(nSessionID);
            }
        }
        public void Destroy()
        {
            foreach (KeyValuePair<UInt64, NetSession> pair in m_dicSession)
            {
                NetSession session = pair.Value;
                session.Close();
            }

            lock (m_dicSession)
            {
                m_dicSession.Clear();
            }
        }

        public void Update()
        {
            List<UInt64> listDestroyedSession = new List<UInt64>();
            foreach (KeyValuePair<UInt64, NetSession> pair in m_dicSession)
            {
                NetSession session = pair.Value;
                if (session.Destroyed)
                {
                    listDestroyedSession.Add(pair.Key);
                }
            }

            if (listDestroyedSession.Count > 0)
            {
                foreach (UInt64 nSessionID in listDestroyedSession)
                {
                    DeleteSession(nSessionID);
                }
            }
        }

        public NetSession FindSession(UInt64 nSessionID)
        {
            NetSession session = null;
            try
            {
                session = m_dicSession[nSessionID];
            }
            catch { }
            return session;
        }

        private UInt64 GetNewSessionID()
        {
            return m_SessionIDGenerator++;
        }
    }
}
