using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    class NetSessionMgr
    {
        private SortedDictionary<UInt64, NetSession> m_dicSession = new SortedDictionary<UInt64, NetSession>();
        private UInt64 m_nSessionIDGenerator = 1;
        private NetCmdQueue m_refCommandQueue;

        public NetSessionMgr(NetCmdQueue refCommandQueue)
        {
            m_refCommandQueue = refCommandQueue;
        }

        public NetSession CreateSession()
        {
            UInt64 nNewSessionID = GetNewSessionID();
            NetSession newSession = new NetSession(nNewSessionID, m_refCommandQueue);

            m_dicSession.Add(nNewSessionID, newSession);
            return newSession;
        }

        public void DeleteSession(UInt64 nSessionID)
        {
            try
            {
                NetSession session = m_dicSession[nSessionID];
                m_dicSession.Remove(nSessionID);

                session.Close();
            }
            catch {}
        }

        public void Destroy()
        {
            List<UInt64> listDestroyedSession = new List<UInt64>();
            foreach (KeyValuePair<UInt64, NetSession> pair in m_dicSession)
            {
                NetSession session = pair.Value;
                session.Close();
            }

            m_dicSession.Clear();
        }

        public void Update()
        {
            List<UInt64> listDestroyedSession = new List<UInt64>();
            foreach (KeyValuePair<UInt64, NetSession> pair in m_dicSession)
            {
                NetSession session = pair.Value;
                if (session.Destroyed)
                {
                    session.Close();
                    listDestroyedSession.Add(pair.Key);
                }
            }

            foreach (UInt64 Key in listDestroyedSession)
            {
                m_dicSession.Remove(Key);
            }
        }

        public NetSession FindSession(UInt64 nSessionID)
        {
            NetSession session = null;
            try
            {
                session = m_dicSession[nSessionID];
            }
            catch {}
            return session;
        }

        private UInt64 GetNewSessionID()
        {
            return m_nSessionIDGenerator++;
        }
    }
}
