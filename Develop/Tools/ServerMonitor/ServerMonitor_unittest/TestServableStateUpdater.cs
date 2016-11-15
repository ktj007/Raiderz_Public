using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitor;
using ServerMonitor_unittest.Mock;
using SMCommon;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestServableStateUpdater
    {
        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;
        private readonly string TEST_SERVER_NAME = "TestServerName";
        private readonly string TEST_SERVER_EXE_PATH = "C:\\TestServerExePath.exe";
        private readonly int WORLD_ID = 1;
        private readonly int SERVER_ID = 1;

        private MockServerInfoMgr m_mockServerInfoMgr = null;
        private MockMonitoringLoggerMgr m_mockMonitoringLoggerMgr = null;
        private ServableStateUpdater m_servableStateUpdater = null;

        [SetUp]
        public void SetUpTestProcessStateChecker()
        {
            m_mockServerInfoMgr = new MockServerInfoMgr();
            m_mockServerInfoMgr.Init(new List<ServerInfo>());
            m_mockMonitoringLoggerMgr = new MockMonitoringLoggerMgr();

            m_servableStateUpdater = new ServableStateUpdater(m_mockServerInfoMgr, m_mockMonitoringLoggerMgr);
        }

        [Test]
        public void TestUpdate_Failure_ServerIsNotRunning()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_SERVER_EXE_PATH, WORLD_ID, SERVER_ID);
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.ProcessState);
            Assert.AreEqual(SERVABLE_STATE.INACTIVE, refServerInfo.ServableState);

            m_servableStateUpdater.Update(ServableStateUpdater.TIME_CHECK_DELAY);

            Assert.AreEqual(SERVABLE_STATE.INACTIVE, refServerInfo.ServableState);
        }

        [Test]
        public void TestUpdate_Success()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_SERVER_EXE_PATH, WORLD_ID, SERVER_ID);
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            refServerInfo.SetProcessRunning();

            Assert.AreEqual(PROCESS_STATE.RUNNING, refServerInfo.ProcessState);

            Assert.AreEqual(false, refServerInfo.IsServable);
            Assert.AreEqual(SERVABLE_STATE.INACTIVE, refServerInfo.ServableState);

            refServerInfo.SetDBInfo(true, true, 0);

            Assert.AreEqual(true, refServerInfo.IsServable);
            Assert.AreEqual(SERVABLE_STATE.INACTIVE, refServerInfo.ServableState);

            m_servableStateUpdater.Update(ServableStateUpdater.TIME_CHECK_DELAY);

            Assert.AreEqual(true, refServerInfo.IsServable);
            Assert.AreEqual(SERVABLE_STATE.ACTIVE, refServerInfo.ServableState);
        }

    }
}
