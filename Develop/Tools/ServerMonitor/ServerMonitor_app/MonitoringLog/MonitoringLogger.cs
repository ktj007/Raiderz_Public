using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitor
{
    public enum LOGGER_TYPE
    {
        DB,
        LIST_BOX,
        FILE,
        NETWORK
    }

    public abstract class MonitoringLogger
    {
        private LOGGER_TYPE m_eType;

        public LOGGER_TYPE Type { get { return m_eType; } }

        public MonitoringLogger(LOGGER_TYPE eType)
        {
            m_eType = eType;
        }

        public abstract void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage);
    }
}
