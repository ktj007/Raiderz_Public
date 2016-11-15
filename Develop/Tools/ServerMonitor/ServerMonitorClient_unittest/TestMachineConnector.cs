using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitorClient;
using ServerMonitorClient_unittest.Mock;

namespace ServerMonitorClient_unittest
{
    [TestFixture]
    public class TestMachineConnector
    {
        class TestNetClient : MockNetClient
        {
            public int m_nCallCount_NewClientSession = 0;
            public int m_nCallCount_DeleteClientSession = 0;

            public TestNetClient() : base()
            {

            }

            public override bool NewClientSession(string strServerName, string strIP, int nPort, out UInt64 nSessionID)
            {
                nSessionID = 0;
                m_nCallCount_NewClientSession++;
                return true;
            }

            public override void DeleteClientSession(UInt64 nSessionID)
            {
                m_nCallCount_DeleteClientSession++;
            }
        }

        private readonly string TEST_MACHINE_NAME1 = "TestMachine1";
        private readonly string TEST_MACHINE_IP1 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT1 = 7500;
        private readonly UInt64 TEST_MACHINE_SESSION_ID1 = 1;

        private readonly string TEST_MACHINE_NAME2 = "TestMachine2";
        private readonly string TEST_MACHINE_IP2 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT2 = 7500;
        private readonly UInt64 TEST_MACHINE_SESSION_ID2 = 2;

        private MachineInfoMgr m_machineInfoMgr = null;
        private TestNetClient m_testNetClient = null;
        private MachineConnector m_machineConnector = null;

        private MachineInfo m_refMachineInfo1 = null;
        private MachineInfo m_refMachineInfo2 = null;

        private void AddTestMachineInfo1()
        {
            m_refMachineInfo1 = new MachineInfo();
            m_refMachineInfo1.MachineName = TEST_MACHINE_NAME1;
            m_refMachineInfo1.IP = TEST_MACHINE_IP1;
            m_refMachineInfo1.Port = TEST_MACHINE_PORT1;

            MachineInfo[] arrMachineInfo = { m_refMachineInfo1 };

            m_machineInfoMgr.AddMachineInfo(arrMachineInfo);
        }
        private void AddTestMachineInfo2()
        {
            m_refMachineInfo2 = new MachineInfo();
            m_refMachineInfo2.MachineName = TEST_MACHINE_NAME2;
            m_refMachineInfo2.IP = TEST_MACHINE_IP2;
            m_refMachineInfo2.Port = TEST_MACHINE_PORT2;

            MachineInfo[] arrMachineInfo = { m_refMachineInfo2 };

            m_machineInfoMgr.AddMachineInfo(arrMachineInfo);
        }

        [SetUp]
        public void SetUpTestMachineConnector()
        {
            m_machineInfoMgr = new MachineInfoMgr();
            m_testNetClient = new TestNetClient();
            m_machineConnector = new MachineConnector(m_machineInfoMgr, m_testNetClient);
        }

        [Test]
        public void TestUpdate_Failure()
        {
            AddTestMachineInfo1();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
            m_machineConnector.Update(0);
            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
            m_machineConnector.Update(1);
            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
            m_machineConnector.Update(3);
            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
        }

        [Test]
        public void TestUpdate()
        {
            AddTestMachineInfo1();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
            m_machineConnector.Update(MachineConnector.DELAY_TO_REQUEST);
            Assert.AreEqual(1, m_testNetClient.m_nCallCount_NewClientSession);
        }

        [Test]
        public void TestConnectToAllMachines()
        {
            AddTestMachineInfo1();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
            m_machineConnector.ConnectToAllMachines();
            Assert.AreEqual(1, m_testNetClient.m_nCallCount_NewClientSession);
        }

        [Test]
        public void TestConnectToAllMachines_TwoMachine()
        {
            AddTestMachineInfo1();
            AddTestMachineInfo2();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_NewClientSession);
            m_machineConnector.ConnectToAllMachines();
            Assert.AreEqual(2, m_testNetClient.m_nCallCount_NewClientSession);
        }

        [Test]
        public void TestDisconnectAllMachines_Failure()
        {
            AddTestMachineInfo1();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_DeleteClientSession);
            m_machineConnector.DisconnectAllMachines();
            Assert.AreEqual(0, m_testNetClient.m_nCallCount_DeleteClientSession);

            m_refMachineInfo1.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_machineConnector.DisconnectAllMachines();
            Assert.AreEqual(0, m_testNetClient.m_nCallCount_DeleteClientSession);
        }

        [Test]
        public void TestDisconnectAllMachines()
        {
            AddTestMachineInfo1();
            m_refMachineInfo1.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_refMachineInfo1.SetConnected();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_DeleteClientSession);
            m_machineConnector.DisconnectAllMachines();
            Assert.AreEqual(1, m_testNetClient.m_nCallCount_DeleteClientSession);
        }

        [Test]
        public void TestDisconnectAllMachines_TwoMachine()
        {
            AddTestMachineInfo1();
            m_refMachineInfo1.SetRequestConnect(TEST_MACHINE_SESSION_ID1);
            m_refMachineInfo1.SetConnected();

            AddTestMachineInfo2();
            m_refMachineInfo2.SetRequestConnect(TEST_MACHINE_SESSION_ID2);
            m_refMachineInfo2.SetConnected();

            Assert.AreEqual(0, m_testNetClient.m_nCallCount_DeleteClientSession);
            m_machineConnector.DisconnectAllMachines();
            Assert.AreEqual(2, m_testNetClient.m_nCallCount_DeleteClientSession);
        }

    }
}
