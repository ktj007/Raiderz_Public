using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitor;
using SMCommon;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestServerInfoMgr
    {
        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;

        private readonly string TEST_SERVER_NAME1 = "TestServerName1";
        private readonly string TEST_SERVER_NAME2 = "TestServerName2";

        private readonly string TEST_SERVER_EXE_PATH1 = "C:\\TestServerExePath1.exe";
        private readonly string TEST_SERVER_EXE_PATH2 = "C:\\TestServerExePath2.exe";

        private readonly int WORLD_ID1 = 1;
        private readonly int WORLD_ID2 = 2;

        private readonly int SERVER_ID1 = 1;
        private readonly int SERVER_ID2 = 2;

        private readonly int TEST_PROCESS_ID = 1000;

        private ServerInfoMgr m_serverInfoMgr = null;

        public bool MockDelegatorAddServer(SERVER_TYPE eServerType, string strServerExePath, string strProcessName) { return true; }
        public bool MockDelegatorDeleteServer(string strServerExePath) { return true; }

        [SetUp]
        public void SetUpTestServerInfoMgr()
        {
            Config cConfig = new Config(new ConfigFileController());
            m_serverInfoMgr = new ServerInfoMgr();
            m_serverInfoMgr.Init(new List<ServerInfo>(), MockDelegatorAddServer, MockDelegatorDeleteServer);
        }

        [Test]
        public void TestAddServerInfo()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            Assert.AreEqual(1, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME2, TEST_SERVER_EXE_PATH2, WORLD_ID2, SERVER_ID2);
            Assert.AreEqual(2, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestAddServerInfo_Failure_NoServerName()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, "", TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestAddServerInfo_Failure_NoServerExePath()
        {
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, "", WORLD_ID1, SERVER_ID1);
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestAddServerInfo_Failure_NoWorldID()
        {
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, 0, SERVER_ID1);
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestAddServerInfo_Failure_NoServerID()
        {
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, 0);
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestAddServerInfo_Failure_AlreadyExistServerExePath()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            Assert.AreEqual(1, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME2, TEST_SERVER_EXE_PATH1, WORLD_ID2, SERVER_ID2);
            Assert.AreEqual(1, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestDeleteServerInfo()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME2, TEST_SERVER_EXE_PATH2, WORLD_ID2, SERVER_ID2);
            Assert.AreEqual(2, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.DeleteServerInfo(TEST_SERVER_NAME2);
            Assert.AreEqual(1, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.DeleteServerInfo(TEST_SERVER_NAME1);
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestDeleteServerInfo_Failure_InvalidServerName()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME2, TEST_SERVER_EXE_PATH2, WORLD_ID2, SERVER_ID2);
            Assert.AreEqual(2, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.DeleteServerInfo("");
            Assert.AreEqual(2, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.DeleteServerInfo("NotExistServerName");
            Assert.AreEqual(2, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestIsExistServerInfo()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);

            Assert.AreEqual(true, m_serverInfoMgr.IsExistServerInfo(TEST_SERVER_EXE_PATH1));
        }

        [Test]
        public void TestIsRevivableServer()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo1 = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(false, refServerInfo1.IsRevivable);

            refServerInfo1.SetProcessDied();
            refServerInfo1.SetProcessRevivable(true);

            Assert.AreEqual(true, refServerInfo1.IsRevivable);
        }

        [Test]
        public void TestGetTRANS_SERVERINFO()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);

            TRANS_SERVERINFONODE serverInfo = new TRANS_SERVERINFONODE();
            m_serverInfoMgr.GetTRANS_SERVERINFONODE(TEST_SERVER_NAME1, ref serverInfo);

            Assert.AreEqual(false, serverInfo.bRevivable);
            Assert.AreEqual(PROCESS_STATE.TERMINATED, (PROCESS_STATE)serverInfo.nProcessState);
            Assert.AreEqual(TEST_SERVER_NAME1, serverInfo.strServerName);
        }

        [Test]
        public void TestGetProcessInfo()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            refServerInfo.SetProcessStarting(TEST_PROCESS_ID);
            refServerInfo.SetProcessRunning();

            string strOriginProcessName = System.IO.Path.GetFileNameWithoutExtension(TEST_SERVER_EXE_PATH1);

            Assert.AreEqual(PROCESS_STATE.RUNNING, refServerInfo.ProcessState);
            Assert.AreEqual(strOriginProcessName, refServerInfo.ProcessName);
            Assert.AreEqual(TEST_PROCESS_ID, refServerInfo.ProcessID);
        }

        [Test]
        public void TestGetExePath()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(TEST_SERVER_EXE_PATH1, refServerInfo.ExePath);
        }

        [Test]
        public void TestGetServerProcessState()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.ProcessState);

            refServerInfo.SetProcessStarting(TEST_PROCESS_ID);
            refServerInfo.SetProcessRunning();

            Assert.AreEqual(PROCESS_STATE.RUNNING, refServerInfo.ProcessState);
        }

        [Test]
        public void TestSetServerRevivable()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(false, refServerInfo.IsRevivable);

            refServerInfo.SetProcessDied();
            Assert.AreEqual(false, refServerInfo.IsRevivable);

            refServerInfo.SetProcessRevivable(true);
            Assert.AreEqual(true, refServerInfo.IsRevivable);
        }

        [Test]
        public void TestSetServerRunning()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.ProcessState);

            refServerInfo.SetProcessStarting(TEST_PROCESS_ID);
            refServerInfo.SetProcessRunning();

            Assert.AreEqual(PROCESS_STATE.RUNNING, refServerInfo.ProcessState);
        }

        [Test]
        public void TestSetServerTerminate()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            refServerInfo.SetProcessStarting(TEST_PROCESS_ID);
            refServerInfo.SetProcessRunning();
            Assert.AreEqual(PROCESS_STATE.RUNNING, refServerInfo.ProcessState);

            refServerInfo.SetProcessTerminated();
            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.ProcessState);
        }

        [Test]
        public void TestGetLatestStartTime()
        {
            m_serverInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME1, TEST_SERVER_EXE_PATH1, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(DateTime.MinValue, refServerInfo.LatestStartTime);

            refServerInfo.SetProcessStarting(TEST_PROCESS_ID);

            Assert.AreNotEqual(DateTime.MinValue, refServerInfo.LatestStartTime);
        }

    }
}
