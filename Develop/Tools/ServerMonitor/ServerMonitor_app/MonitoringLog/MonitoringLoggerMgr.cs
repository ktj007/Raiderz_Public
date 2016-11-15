using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitor
{
    public class MonitoringLoggerMgr
    {
        private LogStringMgr m_refLogStringMgr = null;
        private List<MonitoringLogger> m_listLoggers = new List<MonitoringLogger>();
        public int LoggerCount { get { return m_listLoggers.Count; } }

        public MonitoringLoggerMgr(LogStringMgr refLogStringMgr)
        {
            m_refLogStringMgr = refLogStringMgr;
        }

        public void AddLogger(MonitoringLogger refLogger)
        {
            m_listLoggers.Add(refLogger);
        }

        public void ClearLogger()
        {
            m_listLoggers.Clear();
        }

        public virtual bool Write(string strServerName, int nWorldID, int nServerID, ushort nLogID)
        {
            string strLogMessage;
            if (m_refLogStringMgr.GetString(nLogID, out strLogMessage) == false)
                return false;
            
            for (int i = 0; i < m_listLoggers.Count; ++i)
            {
                m_listLoggers[i].Write(strServerName, nWorldID, nServerID, strLogMessage);
            }
            return true;
        }

        public virtual bool Write(string strServerName, int nWorldID, int nServerID, ushort nLogID, params object[] args)
        {
            string strLogMessage;
            if (m_refLogStringMgr.GetString(nLogID, out strLogMessage) == false)
                return false;

            string strFormatedMessage = String.Format(strLogMessage, args);

            for (int i = 0; i < m_listLoggers.Count; ++i)
            {
                m_listLoggers[i].Write(strServerName, nWorldID, nServerID, strFormatedMessage);
            }
            return true;
        }

    }
}
