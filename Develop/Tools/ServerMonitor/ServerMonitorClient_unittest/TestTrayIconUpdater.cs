using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using NUnit.Framework;
using ServerMonitorClient;
using SMCommon;

namespace ServerMonitorClient_unittest
{
    [TestFixture]
    public class TestTrayIconUpdater
    {
        private readonly string TEST_MACHINE_NAME1 = "TestMachine1";
        private readonly string TEST_MACHINE_IP1 = "127.0.0.1";
        private readonly int TEST_MACHINE_PORT1 = 7500;

        private readonly string TEST_SERVER_NAME1 = "TestServer1";


        private NotifyIcon m_iconOrigin = new NotifyIcon();
        private NotifyIcon m_iconFine = new NotifyIcon();
        private NotifyIcon m_iconBad = new NotifyIcon();
        private NotifyIcon m_iconChanging = new NotifyIcon();

        private MachineInfoMgr m_machineInfoMgr = null;
        private MachineInfo m_refMachineInfo = null;
        private ServerInfo m_refServerInfo = null;

        private TrayIconUpdater m_TrayIconUpdater = null;

        private void InitTestMachineInfo()
        {
            m_refMachineInfo = new MachineInfo();
            m_refMachineInfo.MachineName = TEST_MACHINE_NAME1;
            m_refMachineInfo.IP = TEST_MACHINE_IP1;
            m_refMachineInfo.Port = TEST_MACHINE_PORT1;

            MachineInfo[] arrMachineInfo = { m_refMachineInfo };

            m_machineInfoMgr.AddMachineInfo(arrMachineInfo);
            m_refMachineInfo.SetConnected();


            TRANS_SERVERLISTNODE m_ServerListNode;

            m_ServerListNode = new TRANS_SERVERLISTNODE();
            m_ServerListNode.strServerName = TEST_SERVER_NAME1;
            m_refMachineInfo.ServerInfoMgr.AddServerInfo(ref m_ServerListNode);
            m_refServerInfo = m_refMachineInfo.ServerInfoMgr.Find(TEST_SERVER_NAME1);
        }

        [SetUp]
        public void SetUpTestTrayIconUpdater()
        {
            m_machineInfoMgr = new MachineInfoMgr();
            InitTestMachineInfo();

            m_iconOrigin.Text = "Origin";
            m_iconFine.Text = "Fine";
            m_iconBad.Text = "Bad";
            m_iconChanging.Text = "Changing";

            m_TrayIconUpdater = new TrayIconUpdater(m_iconOrigin, m_machineInfoMgr);
            m_TrayIconUpdater.RegIcon(TRAY_ICON_TYPE.FINE, m_iconFine);
            m_TrayIconUpdater.RegIcon(TRAY_ICON_TYPE.BAD, m_iconBad);
            m_TrayIconUpdater.RegIcon(TRAY_ICON_TYPE.CHANGING, m_iconChanging);
        }

        [Test]
        public void TestTrayIconUpdater_Update()
        {
            m_TrayIconUpdater.Update(0.0f);

            Assert.AreNotEqual(m_iconFine.Text, m_iconOrigin.Text);
            Assert.AreNotEqual(m_iconBad.Text, m_iconOrigin.Text);
            Assert.AreNotEqual(m_iconChanging.Text, m_iconOrigin.Text);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, m_refServerInfo.GetProcessState());

            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);

            Assert.AreEqual(m_iconBad.Text, m_iconOrigin.Text);
        }

        [Test]
        public void TestTrayIconUpdater_Bad()
        {
            m_refServerInfo.SetProcessState(PROCESS_STATE.TERMINATED);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconBad.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.TERMINATING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconBad.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.DIED);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconBad.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.NOUPDATE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconBad.Text, m_iconOrigin.Text);


            m_refServerInfo.SetProcessState(PROCESS_STATE.STARTING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconBad.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.RUNNING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconBad.Text, m_iconOrigin.Text);
        }

        [Test]
        public void TestTrayIconUpdater_Changine()
        {
            m_refServerInfo.SetProcessState(PROCESS_STATE.RUNNING);
            m_refServerInfo.SetServableState(SERVABLE_STATE.INACTIVE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconChanging.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.STARTING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconChanging.Text, m_iconOrigin.Text);



            m_refServerInfo.SetProcessState(PROCESS_STATE.RUNNING);
            m_refServerInfo.SetServableState(SERVABLE_STATE.ACTIVE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconChanging.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.DIED);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconChanging.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.NOUPDATE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconChanging.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.TERMINATING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconChanging.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.TERMINATED);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconChanging.Text, m_iconOrigin.Text);
        }

        [Test]
        public void TestTrayIconUpdater_Fine()
        {
            m_refServerInfo.SetProcessState(PROCESS_STATE.RUNNING);
            m_refServerInfo.SetServableState(SERVABLE_STATE.INACTIVE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconChanging.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.RUNNING);
            m_refServerInfo.SetServableState(SERVABLE_STATE.ACTIVE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreEqual(m_iconFine.Text, m_iconOrigin.Text);


            m_refServerInfo.SetProcessState(PROCESS_STATE.STARTING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconFine.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.DIED);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconFine.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.NOUPDATE);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconFine.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.TERMINATED);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconFine.Text, m_iconOrigin.Text);

            m_refServerInfo.SetProcessState(PROCESS_STATE.TERMINATING);
            m_TrayIconUpdater.Update(TrayIconUpdater.TIME_DELAY_TO_UPDATE);
            Assert.AreNotEqual(m_iconFine.Text, m_iconOrigin.Text);
        }

    }
}
