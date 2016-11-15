using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor.DB;

namespace ServerMonitor
{
    class MonitoringLogger_DB : MonitoringLogger
    {
        private int m_nServerMonitorID = 0;
        private DBMgr m_refDBMgr = null;

        public MonitoringLogger_DB(int nServerMonitorID, DBMgr refDBMgr)
            : base(LOGGER_TYPE.DB)
        {
            m_nServerMonitorID = nServerMonitorID;
            m_refDBMgr = refDBMgr;
        }

        public override void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage)
        {
            m_refDBMgr.InsertLogServerMonitor(m_nServerMonitorID, nWorldID, nServerID, strLogMessage);
        }

    }
}
