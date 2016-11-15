using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitorClient;

namespace ServerMonitorClient_unittest.Mock
{
    public class MockNetClient : NetClient
    {
        protected UInt64 m_nSessionIDGenerator = 1;

        public MockNetClient()
            : base()
        {
        }

        public override bool NewClientSession(string strServerName, string strIP, int nPort, out UInt64 nSessionID)
        {
            nSessionID = m_nSessionIDGenerator++;
            return true;
        }

        public override void DeleteClientSession(UInt64 nSessionID)
        {
        }

    }
}
