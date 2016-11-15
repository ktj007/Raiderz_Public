using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitor
{
    class MonitoringLogger_LogFile : MonitoringLogger
    {
        public MonitoringLogger_LogFile()
            : base(LOGGER_TYPE.FILE)
        {

        }

        public override void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage)
        {
            string strOutput;
            strOutput = strServerName;
            strOutput += ", WorldID=";
            strOutput += nWorldID.ToString();
            strOutput += ", ServerID=";
            strOutput += nServerID.ToString();
            strOutput += ", MSG=";
            strOutput += strLogMessage;
            FileLog.Instance.Write("동작 로그 - " + strOutput);
        }
    }
}
