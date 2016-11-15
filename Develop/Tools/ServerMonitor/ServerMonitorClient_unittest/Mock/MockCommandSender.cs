using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitorClient;

namespace ServerMonitorClient_unittest.Mock
{
    public class MockCommandSender : CommandSender
    {
        public MockCommandSender() : base(null) {}

        public override bool SendStartServer(UInt64 nSessionID, string strServerName)
        {
            return true;
        }

        public override bool SendToggleReviveServer(UInt64 nSessionID, string strServerName, bool bRevive)
        {
            return true;
        }

        public override bool SendTerminateServer(UInt64 nSessionID, string strServerName)
        {
            return true;
        }

        public override bool SendRequestServerList(UInt64 nSessionID)
        {
            return true;
        }

        public override bool SendRequestServerInfo(UInt64 nSessionID, string[] arrServerNames)
        {
            return true;
        }

    }
}
