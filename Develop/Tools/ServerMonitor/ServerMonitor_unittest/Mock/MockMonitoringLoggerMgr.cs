using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor;

namespace ServerMonitor_unittest.Mock
{
    class MockMonitoringLoggerMgr : MonitoringLoggerMgr
    {
        public int m_nLatestLogID = 0;
        public Queue<int> m_queueLogID = new Queue<int>();

        public MockMonitoringLoggerMgr() : base(null) { }

        public void ClearLogIDStack() { m_queueLogID.Clear(); }

        public override bool Write(string strServerName, int nWorldID, int nServerID, ushort nLogID)
        {
            m_nLatestLogID = nLogID;
            m_queueLogID.Enqueue(nLogID);
            return true;
        }
        public override bool Write(string strServerName, int nWorldID, int nServerID, ushort nLogID, params object[] args)
        {
            m_nLatestLogID = nLogID;
            m_queueLogID.Enqueue(nLogID);
            return true;
        }

    }
}
