using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitorClient;
using SMCommon;

namespace ServerMonitorClient_unittest
{
    [TestFixture]
    public class TestServerInfoMgr
    {
        private const string TEST_SERVER_NAME1 = "TestServer1";
        private const PROCESS_STATE TEST_SERVER_STATE1 = PROCESS_STATE.TERMINATED;
        private const bool TEST_SERVER_REVIVABLE1 = false;
        private const int TEST_WORLD_ID1 = 100;
        private const int TEST_SERVER_ID1 = 101;

        private const string TEST_SERVER_NAME2 = "TestServer2";
        private const PROCESS_STATE TEST_SERVER_STATE2 = PROCESS_STATE.TERMINATED;
        private const bool TEST_SERVER_REVIVABLE2 = false;
        private const int TEST_WORLD_ID2 = 100;
        private const int TEST_SERVER_ID2 = 102;

        private TRANS_SERVERLISTNODE m_ServerListNode1 = new TRANS_SERVERLISTNODE();
        private TRANS_SERVERLISTNODE m_ServerListNode2 = new TRANS_SERVERLISTNODE();

        private ServerInfoMgr m_serverInfoMgr = null;

        [SetUp]
        public void SetUpTestServerInfoMgr()
        {
            m_ServerListNode1.strServerName = TEST_SERVER_NAME1;
            m_ServerListNode1.nWorldID = TEST_WORLD_ID1;
            m_ServerListNode1.nServerID = TEST_SERVER_ID1;

            m_ServerListNode2.strServerName = TEST_SERVER_NAME2;
            m_ServerListNode2.nWorldID = TEST_WORLD_ID2;
            m_ServerListNode2.nServerID = TEST_SERVER_ID2;

            m_serverInfoMgr = new ServerInfoMgr();
        }

        [Test]
        public void TestAddServerInfo()
        {
            Assert.AreEqual(0, m_serverInfoMgr.ServerInfoCount);

            m_serverInfoMgr.AddServerInfo(ref m_ServerListNode1);

            Assert.AreEqual(1, m_serverInfoMgr.ServerInfoCount);
        }

        [Test]
        public void TestGetServerNames()
        {
            m_serverInfoMgr.AddServerInfo(ref m_ServerListNode1);
            ServerInfo refServerInfo1 = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            m_serverInfoMgr.AddServerInfo(ref m_ServerListNode2);
            ServerInfo refServerInfo2 = m_serverInfoMgr.Find(TEST_SERVER_NAME2);

            Assert.AreEqual(TEST_SERVER_NAME1, refServerInfo1.GetName());
            Assert.AreEqual(TEST_SERVER_NAME2, refServerInfo2.GetName());
        }

        [Test]
        public void TestGetSetServerState()
        {
            m_serverInfoMgr.AddServerInfo(ref m_ServerListNode1);
            ServerInfo refServerInfo = m_serverInfoMgr.Find(TEST_SERVER_NAME1);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.GetProcessState());
        }

    }
}
