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
    public class TestProcessStateChecker
    {
        private class TestProcessSystem : MockProcessSystem
        {
            public bool m_bProcessRunning = true;

            public override bool IsRunningProcess(string strProcessName, int nProcessID)
            {
                return m_bProcessRunning;
            }
        }

        private readonly float TEST_TIME_CHECK_DELAY = 10.0f;

        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;
        private readonly string TEST_SERVER_NAME = "TestServerName";
        private readonly string TEST_SERVER_EXE_PATH = "C:\\TestServerExePath.exe";
        private readonly int WORLD_ID = 1;
        private readonly int SERVER_ID = 1;

        private MockServerInfoMgr m_mockServerInfoMgr = null;
        private TestProcessSystem m_testProcessSystem = null;
        private MockMonitoringLoggerMgr m_mockMonitoringLoggerMgr = null;
        private ProcessStateChecker m_processStateChecker = null;

        private ServerInfo m_refServerInfo = null;

        [SetUp]
        public void SetUpTestProcessStateChecker()
        {
            m_mockServerInfoMgr = new MockServerInfoMgr();
            m_mockServerInfoMgr.Init(new List<ServerInfo>());
            m_testProcessSystem = new TestProcessSystem();
            m_mockMonitoringLoggerMgr = new MockMonitoringLoggerMgr();

            m_processStateChecker = new ProcessStateChecker(m_mockServerInfoMgr, m_testProcessSystem, m_mockMonitoringLoggerMgr);

            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_SERVER_EXE_PATH, WORLD_ID, SERVER_ID);
            m_refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);
        }


        ///////////////////////////////////////////////////////////////////////////
        // Running
        [Test]
        public void TestUpdate_ServerIsRunning()
        {
            m_refServerInfo.SetProcessStarting(10);

            Assert.AreEqual(PROCESS_STATE.STARTING, m_refServerInfo.ProcessState);

            m_refServerInfo.SetDBInfo(true, false, 0);
            m_testProcessSystem.m_bProcessRunning = true;

            bool bExistStateChanging = false;
            bExistStateChanging = m_processStateChecker.Update(TEST_TIME_CHECK_DELAY);

            Assert.AreEqual(true, bExistStateChanging);
            Assert.AreEqual(PROCESS_STATE.RUNNING, m_refServerInfo.ProcessState);
            Assert.AreEqual(LogID.SUCCESS_START_SERVER, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        ///////////////////////////////////////////////////////////////////////////
        // Terminated
        [Test]
        public void TestUpdate_ServerIsTerminated()
        {
            m_refServerInfo.SetProcessTerminating();

            Assert.AreEqual(PROCESS_STATE.TERMINATING, m_refServerInfo.ProcessState);

            m_refServerInfo.SetDBInfo(false, false, 0);
            m_testProcessSystem.m_bProcessRunning = false;

            bool bExistStateChanging = false;
            bExistStateChanging = m_processStateChecker.Update(TEST_TIME_CHECK_DELAY);

            Assert.AreEqual(true, bExistStateChanging);
            Assert.AreEqual(PROCESS_STATE.TERMINATED, m_refServerInfo.ProcessState);
            Assert.AreEqual(LogID.SUCCESS_TERMINATE_SERVER, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        ///////////////////////////////////////////////////////////////////////////
        // Died
        [Test]
        public void TestUpdate_ServerDied()
        {
            m_refServerInfo.SetProcessStarting(10);
            m_refServerInfo.SetProcessRunning();

            Assert.AreEqual(PROCESS_STATE.RUNNING, m_refServerInfo.ProcessState);

            m_testProcessSystem.m_bProcessRunning = false;

            bool bExistStateChanging = false;
            bExistStateChanging = m_processStateChecker.Update(TEST_TIME_CHECK_DELAY);

            Assert.AreEqual(true, bExistStateChanging);
            Assert.AreEqual(PROCESS_STATE.DIED, m_refServerInfo.ProcessState);
            Assert.AreEqual(LogID.SERVER_DIED, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        ///////////////////////////////////////////////////////////////////////////
        // NoUpdate
        [Test]
        public void TestUpdate_ServerIsNoUpdate()
        {
            m_refServerInfo.SetProcessStarting(10);
            m_refServerInfo.SetProcessRunning();

            Assert.AreEqual(PROCESS_STATE.RUNNING, m_refServerInfo.ProcessState);

            m_refServerInfo.SetDBInfo(false, false, 0);

            bool bExistStateChanging = false;
            bExistStateChanging = m_processStateChecker.Update(TEST_TIME_CHECK_DELAY);

            Assert.AreEqual(true, bExistStateChanging);
            Assert.AreEqual(PROCESS_STATE.NOUPDATE, m_refServerInfo.ProcessState);
            Assert.AreEqual(LogID.SERVER_NOUPDATE, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

        ///////////////////////////////////////////////////////////////////////////
        // Recorvery
        [Test]
        public void TestUpdate_ServerIsRecorvery()
        {
            m_refServerInfo.SetProcessNoUpdate();

            Assert.AreEqual(PROCESS_STATE.NOUPDATE, m_refServerInfo.ProcessState);

            m_refServerInfo.SetDBInfo(true, false, 0);
            m_testProcessSystem.m_bProcessRunning = true;

            bool bExistStateChanging = false;
            bExistStateChanging = m_processStateChecker.Update(TEST_TIME_CHECK_DELAY);

            Assert.AreEqual(true, bExistStateChanging);
            Assert.AreEqual(PROCESS_STATE.RUNNING, m_refServerInfo.ProcessState);
            Assert.AreEqual(LogID.SERVER_RECOVERY_NOUPDATE, m_mockMonitoringLoggerMgr.m_nLatestLogID);
        }

    }
}
