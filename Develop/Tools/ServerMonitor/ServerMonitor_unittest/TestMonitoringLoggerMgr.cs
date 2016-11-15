using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor;
using ServerMonitor_unittest.Mock;
using NUnit.Framework;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestMonitoringLoggerMgr
    {
        private readonly string TEST_SERVER_NAME = "TestServerName";
        private readonly int TEST_WORLD_ID1 = 1;
        private readonly int TEST_SERVER_ID1 = 2;
        private MonitoringLoggerMgr m_monitoringLoggerMgr = null;

        [SetUp]
        public void SetUpMonitoringLoggerMgr()
        {
            MockLogStringMgr mockLogStringMgr = new MockLogStringMgr();
            mockLogStringMgr.LoadStringTable();
            m_monitoringLoggerMgr = new MonitoringLoggerMgr(mockLogStringMgr);
        }

        [Test]
        public void TestAddLogger()
        {
            Assert.AreEqual(0, m_monitoringLoggerMgr.LoggerCount);

            m_monitoringLoggerMgr.AddLogger(new MockMonitoringLogger_Dummy());

            Assert.AreEqual(1, m_monitoringLoggerMgr.LoggerCount);

            m_monitoringLoggerMgr.AddLogger(new MockMonitoringLogger_Dummy());

            Assert.AreEqual(2, m_monitoringLoggerMgr.LoggerCount);
        }

        [Test]
        public void TestClearLogger()
        {
            m_monitoringLoggerMgr.AddLogger(new MockMonitoringLogger_Dummy());
            m_monitoringLoggerMgr.AddLogger(new MockMonitoringLogger_Dummy());
            m_monitoringLoggerMgr.AddLogger(new MockMonitoringLogger_Dummy());

            Assert.AreEqual(3, m_monitoringLoggerMgr.LoggerCount);

            m_monitoringLoggerMgr.ClearLogger();

            Assert.AreEqual(0, m_monitoringLoggerMgr.LoggerCount);
        }

        [Test]
        public void TestWrite()
        {
            List<string> m_listMessage = new List<string>();

            m_monitoringLoggerMgr.AddLogger(new MockMonitoringLogger_Test(m_listMessage));

            Assert.AreEqual(1, m_monitoringLoggerMgr.LoggerCount);
            Assert.AreEqual(0, m_listMessage.Count);

            m_monitoringLoggerMgr.Write(TEST_SERVER_NAME, TEST_WORLD_ID1, TEST_SERVER_ID1, MockLogStringMgr.TEST_STRING_ID1);

            Assert.AreEqual(1, m_listMessage.Count);
            Assert.AreEqual(MockLogStringMgr.TEST_STRING1, m_listMessage[0]);

            m_monitoringLoggerMgr.Write(TEST_SERVER_NAME, TEST_WORLD_ID1, TEST_SERVER_ID1, MockLogStringMgr.TEST_STRING_ID2);
            Assert.AreEqual(MockLogStringMgr.TEST_STRING2, m_listMessage[1]);
            m_monitoringLoggerMgr.Write(TEST_SERVER_NAME, TEST_WORLD_ID1, TEST_SERVER_ID1, MockLogStringMgr.TEST_STRING_ID3);
            Assert.AreEqual(MockLogStringMgr.TEST_STRING3, m_listMessage[2]);

            Assert.AreEqual(3, m_listMessage.Count);
        }

    }

    public class MockMonitoringLogger_Dummy : MonitoringLogger
    {
        public MockMonitoringLogger_Dummy()
            : base(LOGGER_TYPE.FILE)
        {}
        public override void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage) { }
    }

    public class MockMonitoringLogger_Test : MonitoringLogger
    {
        private List<string> m_listMessage = null;

        public MockMonitoringLogger_Test(List<string> refMessage)
            : base(LOGGER_TYPE.FILE)
        {
            m_listMessage = refMessage;
        }

        public override void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage)
        {
            m_listMessage.Add(strLogMessage);
        }
    }

}
