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
    public class TestServerReviver
    {
        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;
        private readonly string TEST_SERVER_NAME = "TestServerName";
        private readonly string TEST_SERVER_EXE_PATH = "C:\\TestServerExePath.exe";
        private readonly int WORLD_ID = 1;
        private readonly int SERVER_ID = 1;

        private MockServerInfoMgr m_mockServerInfoMgr = null;
        private MockProcessSystem m_mockProcessSystem = null;
        private ServerController m_serverController = null;
        private ServerReviver m_serverReviver = null;

        [SetUp]
        public void SetUpTestServerReviver()
        {
            m_mockServerInfoMgr = new MockServerInfoMgr();
            m_mockServerInfoMgr.Init(new List<ServerInfo>());
            m_mockProcessSystem = new MockProcessSystem();
            MockMonitoringLoggerMgr mockMonitoringLoggerMgr = new MockMonitoringLoggerMgr();
            m_serverController = new ServerController(m_mockServerInfoMgr, m_mockProcessSystem, mockMonitoringLoggerMgr);

            m_serverReviver = new ServerReviver(m_mockServerInfoMgr, m_serverController);

            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_SERVER_EXE_PATH, WORLD_ID, SERVER_ID);
        }

        [Test]
        public void TestReviveAllRevivableServer()
        {
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            refServerInfo.SetProcessDied();
            refServerInfo.SetProcessRevivable(true);

            Assert.AreEqual(false, m_mockProcessSystem.CallStartProcess);

            m_serverReviver.ReviveAllRevivableServer();

            Assert.AreEqual(PROCESS_STATE.STARTING, refServerInfo.ProcessState);
            Assert.AreEqual(true,                   m_mockProcessSystem.CallStartProcess);
        }
    }

}
