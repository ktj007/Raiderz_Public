using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitor
{
    public class MonitoringLogger_Network : MonitoringLogger
    {
        private ServerMonitorClientMgr m_refServerMonitorClientMgr = null;
        private CommandSender m_refCommandSender = null;

        public MonitoringLogger_Network(ServerMonitorClientMgr refServerMonitorClientMgr, CommandSender refCommandSender)
            : base(LOGGER_TYPE.NETWORK)
        {
            m_refServerMonitorClientMgr = refServerMonitorClientMgr;
            m_refCommandSender = refCommandSender;
        }

        public override void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage)
        {
            UInt64[] arrSessionID;
            m_refServerMonitorClientMgr.GetAllSessionID(out arrSessionID);

            string strDateTime = DateTime.Now.ToString("yyyy/MM/dd-HH:mm:ss");

            for (int i = 0; i < arrSessionID.Length; ++i)
            {
                m_refCommandSender.SendSetverMonitoringLog(arrSessionID[i], strDateTime, strServerName, nWorldID, nServerID, strLogMessage);
            }
        }

    }
}
