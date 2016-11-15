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
    public class TestMachineInfoMgr
    {
        private readonly string TEST_MACHINE_NAME1 = "TestMachine1";
        private readonly string TEST_MACHINE_IP1 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT1 = 7500;
        private readonly UInt64 TEST_MACHINE_SESSION_ID1 = 1;

        private readonly string TEST_MACHINE_NAME2 = "TestMachine2";
        private readonly string TEST_MACHINE_IP2 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT2 = 7500;

        private readonly string TEST_SERVER_NAME1 = "TestServer1";
        private readonly string TEST_SERVER_NAME2 = "TestServer2";

        private MachineInfoMgr m_machineInfoMgr = null;
        private MachineInfo m_refMachineInfo = null;

        private void AddTestMachineInfo()
        {
            m_refMachineInfo = new MachineInfo();
            m_refMachineInfo.MachineName = TEST_MACHINE_NAME1;
            m_refMachineInfo.IP = TEST_MACHINE_IP1;
            m_refMachineInfo.Port = TEST_MACHINE_PORT1;

            MachineInfo[] arrMachineInfo = { m_refMachineInfo };

            m_machineInfoMgr.AddMachineInfo(arrMachineInfo);
        }
        
        [SetUp]
        public void SetUpTestMachineInfoMgr()
        {
            m_machineInfoMgr = new MachineInfoMgr();
        }

        [Test]
        public void TestAddMachineInfo()
        {
            Assert.AreEqual(0, m_machineInfoMgr.MachineInfoCount);

            MachineInfo machineInfo = new MachineInfo();
            machineInfo.MachineName = TEST_MACHINE_NAME1;
            machineInfo.IP = TEST_MACHINE_IP1;
            machineInfo.Port = TEST_MACHINE_PORT1;
            MachineInfo[] arrMachineInfo1 = { machineInfo };
            m_machineInfoMgr.AddMachineInfo(arrMachineInfo1);

            Assert.AreEqual(1, m_machineInfoMgr.MachineInfoCount);


            machineInfo = new MachineInfo();
            machineInfo.MachineName = TEST_MACHINE_NAME2;
            machineInfo.IP = TEST_MACHINE_IP2;
            machineInfo.Port = TEST_MACHINE_PORT2;
            MachineInfo[] arrMachineInfo2 = { machineInfo };
            m_machineInfoMgr.AddMachineInfo(arrMachineInfo2);

            Assert.AreEqual(2, m_machineInfoMgr.MachineInfoCount);
        }

        [Test]
        public void TestIsConnectable()
        {
            AddTestMachineInfo();

            Assert.AreEqual(false, m_refMachineInfo.IsRequestConnect);
            Assert.AreEqual(false, m_refMachineInfo.IsConnected);
            Assert.AreEqual(0, m_refMachineInfo.GetSessionID());


            Assert.AreEqual(true, m_refMachineInfo.IsConnectable);
        }

        [Test]
        public void TestIsConnectable_False1()
        {
            AddTestMachineInfo();
            Assert.AreEqual(true, m_refMachineInfo.IsConnectable);

            m_refMachineInfo.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            Assert.AreEqual(true, m_refMachineInfo.IsRequestConnect);
            Assert.AreEqual(TEST_MACHINE_SESSION_ID1, m_refMachineInfo.GetSessionID());


            Assert.AreEqual(false, m_refMachineInfo.IsConnectable);
        }

        [Test]
        public void TestIsConnectable_SetDisconnected()
        {
            AddTestMachineInfo();

            m_refMachineInfo.SetRequestConnect(TEST_MACHINE_SESSION_ID1);

            m_refMachineInfo.SetDisconnected();
            Assert.AreEqual(true, m_refMachineInfo.IsConnectable);


            m_refMachineInfo.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_refMachineInfo.SetConnected();
            Assert.AreEqual(false, m_refMachineInfo.IsConnectable);


            m_refMachineInfo.SetDisconnected();
            Assert.AreEqual(true, m_refMachineInfo.IsConnectable);
        }
        
        [Test]
        public void TestGetMachinePlayerCount_WithMasterServer()
        {
            AddTestMachineInfo();

            int nPlayerCount = m_machineInfoMgr.GetMachinePlayerCount(TEST_MACHINE_NAME1);
            Assert.AreEqual(0, nPlayerCount);

            ServerInfoMgr serverInfoMgr = m_machineInfoMgr.GetServerInfoMgr(TEST_MACHINE_NAME1);

            TRANS_SERVERLISTNODE serverListNode1 = new TRANS_SERVERLISTNODE();
            serverListNode1.strServerName = TEST_SERVER_NAME1;
            serverListNode1.nWorldID = 5000;
            serverListNode1.nServerID = 1;
            serverListNode1.nServerType = (byte)SERVER_TYPE.SERVER_MASTER;

            TRANS_SERVERLISTNODE serverListNode2 = new TRANS_SERVERLISTNODE();
            serverListNode2.strServerName = TEST_SERVER_NAME2;
            serverListNode2.nWorldID = 5000;
            serverListNode2.nServerID = 2;
            serverListNode2.nServerType = (byte)SERVER_TYPE.SERVER_GAME;

            serverInfoMgr.AddServerInfo(ref serverListNode1);
            serverInfoMgr.AddServerInfo(ref serverListNode2);

            serverInfoMgr.SetServerCurServerPlayerCount(TEST_SERVER_NAME1, 1);
            serverInfoMgr.SetServerCurServerPlayerCount(TEST_SERVER_NAME2, 1);

            nPlayerCount = m_machineInfoMgr.GetMachinePlayerCount(TEST_MACHINE_NAME1);
            Assert.AreEqual(1, nPlayerCount);
        }

        [Test]
        public void TestGetMachinePlayerCount_NoMasterServer()
        {
            AddTestMachineInfo();

            int nPlayerCount = m_machineInfoMgr.GetMachinePlayerCount(TEST_MACHINE_NAME1);
            Assert.AreEqual(0, nPlayerCount);

            ServerInfoMgr serverInfoMgr = m_machineInfoMgr.GetServerInfoMgr(TEST_MACHINE_NAME1);

            TRANS_SERVERLISTNODE serverListNode1 = new TRANS_SERVERLISTNODE();
            serverListNode1.strServerName = TEST_SERVER_NAME1;
            serverListNode1.nWorldID = 5000;
            serverListNode1.nServerID = 1;
            serverListNode1.nServerType = (byte)SERVER_TYPE.SERVER_GAME;

            TRANS_SERVERLISTNODE serverListNode2 = new TRANS_SERVERLISTNODE();
            serverListNode2.strServerName = TEST_SERVER_NAME2;
            serverListNode2.nWorldID = 5000;
            serverListNode2.nServerID = 2;
            serverListNode2.nServerType = (byte)SERVER_TYPE.SERVER_GAME;

            serverInfoMgr.AddServerInfo(ref serverListNode1);
            serverInfoMgr.AddServerInfo(ref serverListNode2);

            serverInfoMgr.SetServerCurServerPlayerCount(TEST_SERVER_NAME1, 1);
            serverInfoMgr.SetServerCurServerPlayerCount(TEST_SERVER_NAME2, 1);

            nPlayerCount = m_machineInfoMgr.GetMachinePlayerCount(TEST_MACHINE_NAME1);
            Assert.AreEqual(2, nPlayerCount);
        }

        [Test]
        public void TestSetRequestConnect()
        {
            AddTestMachineInfo();

            Assert.AreEqual(false, m_refMachineInfo.IsRequestConnect);
            Assert.AreEqual(0, m_refMachineInfo.GetSessionID());

            m_refMachineInfo.SetRequestConnect(TEST_MACHINE_SESSION_ID1);

            Assert.AreEqual(true, m_refMachineInfo.IsRequestConnect);
            Assert.AreEqual(TEST_MACHINE_SESSION_ID1, m_refMachineInfo.GetSessionID());
        }

        [Test]
        public void TestSetConnected()
        {
            AddTestMachineInfo();
            m_refMachineInfo.SetRequestConnect(TEST_MACHINE_SESSION_ID1);

            Assert.AreEqual(true, m_refMachineInfo.IsRequestConnect);
            Assert.AreEqual(false, m_refMachineInfo.IsConnected);
            Assert.AreEqual(MACHINE_STATE.OFFLINE, m_refMachineInfo.GetState());

            m_refMachineInfo.SetConnected();

            Assert.AreEqual(false, m_refMachineInfo.IsRequestConnect);
            Assert.AreEqual(true, m_refMachineInfo.IsConnected);
            Assert.AreEqual(MACHINE_STATE.ONLINE, m_refMachineInfo.GetState());
        }

        [Test]
        public void TestSetDisconnected()
        {
            AddTestMachineInfo();
            m_refMachineInfo.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_refMachineInfo.SetConnected();

            Assert.AreEqual(true, m_refMachineInfo.IsConnected);
            Assert.AreEqual(TEST_MACHINE_SESSION_ID1, m_refMachineInfo.GetSessionID());
            Assert.AreEqual(MACHINE_STATE.ONLINE, m_refMachineInfo.GetState());

            m_refMachineInfo.SetDisconnected();

            Assert.AreEqual(false, m_refMachineInfo.IsConnected);
            Assert.AreEqual(0, m_refMachineInfo.GetSessionID());
            Assert.AreEqual(MACHINE_STATE.OFFLINE, m_refMachineInfo.GetState());
        }


    }
}
