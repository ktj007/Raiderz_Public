using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitorClient;
using ServerMonitorClient_unittest.Mock;
using SMCommon;

namespace ServerMonitorClient_unittest
{
    [TestFixture]
    public class TestServerInfoRequester
    {
        private class TestCommandSender : MockCommandSender
        {
            public int m_nPostCount = 0;

            public override bool SendRequestServerInfo(UInt64 nSessionID, string[] arrServerNames)
            {
                m_nPostCount++;
                return true;
            }
        }

        private readonly float DELAY_TO_REQUEST = 10.0f;

        private readonly string TEST_MACHINE_NAME1 = "TestMachine1";
        private readonly string TEST_MACHINE_IP1 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT1 = 7500;
        private readonly UInt64 TEST_MACHINE_SESSION_ID1 = 1;

        private readonly string TEST_MACHINE_NAME2 = "TestMachine2";
        private readonly string TEST_MACHINE_IP2 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT2 = 7500;
        private readonly UInt64 TEST_MACHINE_SESSION_ID2 = 2;

        private readonly string TEST_SERVER_NAME1 = "TestServer1";
        private const int TEST_WORLD_ID1 = 100;
        private const int TEST_SERVER_ID1 = 101;

        private const string TEST_SERVER_NAME2 = "TestServer2";
        private const int TEST_WORLD_ID2 = 100;
        private const int TEST_SERVER_ID2 = 102;

        private TRANS_SERVERLISTNODE m_ServerListNode1 = new TRANS_SERVERLISTNODE();
        private TRANS_SERVERLISTNODE m_ServerListNode2 = new TRANS_SERVERLISTNODE();

        private MachineInfoMgr m_machineInfoMgr = null;
        private TestCommandSender m_testCommandSender = null;
        private ServerInfoRequester m_serverInfoRequester = null;

        private MachineInfo m_refMachineInfo1 = null;
        private MachineInfo m_refMachineInfo2 = null;

        private void AddTestMachineInfo1(MachineInfoMgr refMachineInfoMgr)
        {
            m_refMachineInfo1 = new MachineInfo();
            m_refMachineInfo1.MachineName = TEST_MACHINE_NAME1;
            m_refMachineInfo1.IP = TEST_MACHINE_IP1;
            m_refMachineInfo1.Port = TEST_MACHINE_PORT1;

            MachineInfo[] arrMachineInfo = { m_refMachineInfo1 };
            refMachineInfoMgr.AddMachineInfo(arrMachineInfo);
        }

        private void AddTestMachineInfo2(MachineInfoMgr refMachineInfoMgr)
        {
            m_refMachineInfo2 = new MachineInfo();
            m_refMachineInfo2.MachineName = TEST_MACHINE_NAME2;
            m_refMachineInfo2.IP = TEST_MACHINE_IP2;
            m_refMachineInfo2.Port = TEST_MACHINE_PORT2;

            MachineInfo[] arrMachineInfo = { m_refMachineInfo2 };
            refMachineInfoMgr.AddMachineInfo(arrMachineInfo);
        }

        [SetUp]
        public void SetUpTestServerInfoMgr()
        {
            m_ServerListNode1.strServerName = TEST_SERVER_NAME1;
            m_ServerListNode1.nWorldID = TEST_WORLD_ID1;
            m_ServerListNode1.nServerID = TEST_SERVER_ID1;

            m_ServerListNode2.strServerName = TEST_SERVER_NAME2;
            m_ServerListNode2.nWorldID = TEST_WORLD_ID2;
            m_ServerListNode2.nServerID = TEST_SERVER_ID2;

            m_machineInfoMgr = new MachineInfoMgr();
            m_testCommandSender = new TestCommandSender();

            AddTestMachineInfo1(m_machineInfoMgr);

            m_serverInfoRequester = new ServerInfoRequester(m_machineInfoMgr, m_testCommandSender);
        }

        [Test]
        public void TestUpdate()
        {
            m_refMachineInfo1.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_refMachineInfo1.SetConnected();

            ServerInfoMgr refServerInfoMgr = m_refMachineInfo1.ServerInfoMgr;
            refServerInfoMgr.AddServerInfo(ref m_ServerListNode1);
            Assert.AreEqual(1, refServerInfoMgr.ServerInfoCount);

            m_serverInfoRequester.Update(DELAY_TO_REQUEST);

            Assert.AreEqual(1, m_testCommandSender.m_nPostCount);
        }

        [Test]
        public void TestUpdate_Machine2()
        {
            AddTestMachineInfo2(m_machineInfoMgr);
            Assert.AreEqual(2, m_machineInfoMgr.MachineInfoCount);

            m_refMachineInfo1.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_refMachineInfo1.SetConnected();

            m_refMachineInfo2.SetRequestConnect(TEST_MACHINE_SESSION_ID2);
            m_refMachineInfo2.SetConnected();

            ServerInfoMgr refServerInfoMgr = null;
            refServerInfoMgr = m_refMachineInfo1.ServerInfoMgr;
            refServerInfoMgr.AddServerInfo(ref m_ServerListNode1);
            Assert.AreEqual(1, refServerInfoMgr.ServerInfoCount);

            refServerInfoMgr = m_refMachineInfo2.ServerInfoMgr;
            refServerInfoMgr.AddServerInfo(ref m_ServerListNode2);
            Assert.AreEqual(1, refServerInfoMgr.ServerInfoCount);

            m_serverInfoRequester.Update(DELAY_TO_REQUEST);

            Assert.AreEqual(2, m_testCommandSender.m_nPostCount);
        }

        [Test]
        public void TestUpdate_Failure()
        {
            ServerInfoMgr refServerInfoMgr = m_refMachineInfo1.ServerInfoMgr;
            Assert.AreEqual(0, refServerInfoMgr.ServerInfoCount);

            m_serverInfoRequester.Update(0);

            Assert.AreEqual(0, m_testCommandSender.m_nPostCount);

            m_serverInfoRequester.Update(DELAY_TO_REQUEST);

            Assert.AreEqual(0, m_testCommandSender.m_nPostCount);

            m_serverInfoRequester.Update(60);

            Assert.AreEqual(0, m_testCommandSender.m_nPostCount);
        }

    }
}
