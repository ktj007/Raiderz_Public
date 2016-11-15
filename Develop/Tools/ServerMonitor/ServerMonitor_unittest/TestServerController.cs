using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using SMCommon;
using ServerMonitor;
using ServerMonitor_unittest.Mock;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestServerController
    {
        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;
        private readonly string TEST_SERVER_NAME = "TestServerName";
        private readonly string TEST_SERVER_EXE_PATH = "C:\\TestServerExePath.exe";
        private readonly int WORLD_ID = 1;
        private readonly int SERVER_ID = 1;

        private ServerController m_ServerController = null;
        private MockServerInfoMgr m_mockServerInfoMgr = null;
        private MockProcessSystem m_mockProcessSystem = null;
        private MockMonitoringLoggerMgr m_mockMonitoringLoggerMgr = null;

        [SetUp]
        public void SetUpTestServerController()
        {
            m_mockServerInfoMgr = new MockServerInfoMgr();
            m_mockServerInfoMgr.Init(new List<ServerInfo>());
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_SERVER_EXE_PATH, WORLD_ID, SERVER_ID);

            m_mockProcessSystem = new MockProcessSystem();
            m_mockMonitoringLoggerMgr = new MockMonitoringLoggerMgr();
            m_ServerController = new ServerController(m_mockServerInfoMgr, m_mockProcessSystem, m_mockMonitoringLoggerMgr);
        }

        [Test]
        public void TestStartServer()
        {
            PROCESS_STATE eProcessState;

            m_mockServerInfoMgr.GetProcessState(TEST_SERVER_NAME, out eProcessState);
            Assert.AreEqual(PROCESS_STATE.TERMINATED, eProcessState);
            Assert.AreEqual(false, m_mockProcessSystem.CallStartProcess);
            Assert.AreEqual(0, m_mockMonitoringLoggerMgr.m_nLatestLogID);

            Assert.AreEqual(true, m_ServerController.StartServer(TEST_SERVER_NAME));

            m_mockServerInfoMgr.GetProcessState(TEST_SERVER_NAME, out eProcessState);
            Assert.AreEqual(PROCESS_STATE.STARTING, eProcessState);
            Assert.AreEqual(true, m_mockProcessSystem.CallStartProcess);
            Assert.AreEqual(LogID.STARTING_SERVER, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        [Test]
        public void TestStartServer_Failure_NoServerName()
        {
            PROCESS_STATE eProcessState;

            Assert.AreEqual(false, m_ServerController.StartServer(""));

            m_mockServerInfoMgr.GetProcessState(TEST_SERVER_NAME, out eProcessState);
            Assert.AreEqual(PROCESS_STATE.TERMINATED, eProcessState);
            Assert.AreEqual(false, m_mockProcessSystem.CallStartProcess);
            Assert.AreEqual(0, m_mockMonitoringLoggerMgr.LoggerCount);
        }

        [Test]
        public void TestStartServer_Failure_InvalidServerName()
        {
            PROCESS_STATE eProcessState;

            Assert.AreEqual(false, m_ServerController.StartServer("InvalidServerName"));

            m_mockServerInfoMgr.GetProcessState(TEST_SERVER_NAME, out eProcessState);
            Assert.AreEqual(PROCESS_STATE.TERMINATED, eProcessState);
            Assert.AreEqual(false, m_mockProcessSystem.CallStartProcess);
            Assert.AreEqual(0, m_mockMonitoringLoggerMgr.LoggerCount);
        }

        [Test]
        public void TestStartServer_Failure_AlreadyStarted()
        {
            PROCESS_STATE eProcessState;

            Assert.AreEqual(true, m_ServerController.StartServer(TEST_SERVER_NAME));

            m_mockServerInfoMgr.GetProcessState(TEST_SERVER_NAME, out eProcessState);
            Assert.AreEqual(PROCESS_STATE.STARTING, eProcessState);

            Assert.AreEqual(false, m_ServerController.StartServer(TEST_SERVER_NAME));

            Assert.AreEqual(LogID.FAILED_START_SERVER_ALREADY_START, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        [Test]
        public void TestTerminateServer()
        {
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(true, m_ServerController.StartServer(TEST_SERVER_NAME));
            Assert.AreEqual(true, m_mockProcessSystem.CallStartProcess);
            Assert.AreEqual(false, m_mockProcessSystem.CallTerminateProcess);

            refServerInfo.SetProcessRunning();

            Assert.AreEqual(true, m_ServerController.TerminateServer(TEST_SERVER_NAME));
            Assert.AreEqual(PROCESS_STATE.TERMINATING, refServerInfo.ProcessState);
            Assert.AreEqual(true, m_mockProcessSystem.CallTerminateProcess);
            Assert.AreEqual(LogID.TERMINATING_SERVER, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        [Test]
        public void TestTerminateServer_Failure_NoServerName()
        {
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(true, m_ServerController.StartServer(TEST_SERVER_NAME));

            Assert.AreEqual(false, m_ServerController.TerminateServer(""));

            Assert.AreEqual(PROCESS_STATE.STARTING, refServerInfo.ProcessState);
            Assert.AreEqual(false, m_mockProcessSystem.CallTerminateProcess);
            Assert.AreEqual(0, m_mockMonitoringLoggerMgr.LoggerCount);
        }

        [Test]
        public void TestTerminateServer_Failure_InvalidServerName()
        {
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(true, m_ServerController.StartServer(TEST_SERVER_NAME));

            Assert.AreEqual(false, m_ServerController.TerminateServer("InvalidServerName"));

            Assert.AreEqual(PROCESS_STATE.STARTING, refServerInfo.ProcessState);
            Assert.AreEqual(false, m_mockProcessSystem.CallTerminateProcess);
            Assert.AreEqual(0, m_mockMonitoringLoggerMgr.LoggerCount);
        }

        [Test]
        public void TestTerminateServer_Failure_AlreadyStoped()
        {
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.ProcessState);
            Assert.AreEqual(false, m_ServerController.TerminateServer(TEST_SERVER_NAME));
            Assert.AreEqual(LogID.FAILED_TERMINATE_SERVER_ALREADY_TERMINATED, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        [Test]
        public void TestTerminateServer_NoUpdateServer()
        {
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            m_ServerController.StartServer(TEST_SERVER_NAME);
            refServerInfo.SetProcessNoUpdate();

            m_mockMonitoringLoggerMgr.ClearLogIDStack();

            Assert.AreEqual(PROCESS_STATE.NOUPDATE, refServerInfo.ProcessState);
            Assert.AreEqual(true, m_ServerController.TerminateServer(TEST_SERVER_NAME));
            Assert.AreEqual(2, m_mockMonitoringLoggerMgr.m_queueLogID.Count);
            
            int[] arrLogID = m_mockMonitoringLoggerMgr.m_queueLogID.ToArray();
            
            Assert.AreEqual(LogID.WARN_TERMINATE_SERVER_NO_UPDATE, m_mockMonitoringLoggerMgr.m_queueLogID.Dequeue());
            Assert.AreEqual(LogID.TERMINATING_SERVER, m_mockMonitoringLoggerMgr.m_queueLogID.Dequeue());
            Assert.AreEqual(PROCESS_STATE.NOUPDATE, refServerInfo.ProcessState);
        }

    }

}
