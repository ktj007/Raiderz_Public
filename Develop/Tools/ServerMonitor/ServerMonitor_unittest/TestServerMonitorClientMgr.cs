using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitor;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestServerMonitorClientMgr
    {
        private readonly UInt64 TEST_SESSION_ID = 1;

        private ServerMonitorClientMgr m_serverMonitorClientMgr = null;

        [SetUp]
        public void SetUpServerMonitorClientMgr()
        {
            m_serverMonitorClientMgr = new ServerMonitorClientMgr();
        }

        [Test]
        public void TestAddServerMonitorClient()
        {
            Assert.AreEqual(0, m_serverMonitorClientMgr.SessionCount);

            m_serverMonitorClientMgr.AddServerMonitorClient(TEST_SESSION_ID);

            Assert.AreEqual(1, m_serverMonitorClientMgr.SessionCount);
        }

        [Test]
        public void TestDeleteServerMonitorClient()
        {
            m_serverMonitorClientMgr.AddServerMonitorClient(TEST_SESSION_ID);
            Assert.AreEqual(1, m_serverMonitorClientMgr.SessionCount);

            m_serverMonitorClientMgr.DeleteServerMonitorClient(TEST_SESSION_ID);
            Assert.AreEqual(0, m_serverMonitorClientMgr.SessionCount);
        }
    }
}
