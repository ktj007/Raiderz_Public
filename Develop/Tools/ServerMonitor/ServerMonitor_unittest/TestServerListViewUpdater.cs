using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ServerMonitor;
using ServerMonitor_unittest.Mock;
using SMCommon;
using NUnit.Framework;

namespace ServerMonitor_unittest
{
    [TestFixture]
    public class TestServerListUpdater
    {
        private readonly SERVER_TYPE TEST_SERVER_TYPE = SERVER_TYPE.SERVER_GAME;

        private readonly string TEST_SERVER_NAME = "TestServerName";
        private readonly string TEST_EXE_PATH = "C:\\TestProcessName.exe";
        private readonly int WORLD_ID1 = 1;
        private readonly int SERVER_ID1 = 10;
        private readonly int TEST_PROCESS_ID = 1000;

        private readonly string TEST_SERVER_NAME2 = "TestServerName2";
        private readonly string TEST_EXE_PATH2 = "C:\\TestProcessName2.exe";
        private readonly int WORLD_ID2 = 2;
        private readonly int SERVER_ID2 = 20;

        private readonly int COL_INDEX_SERVER_NAME = 3;

        ListView m_lviewServerList = null;
        private MockServerInfoMgr m_mockServerInfoMgr = null;
        private ServerListViewUpdater m_serverListViewUpdater = null;

        [SetUp]
        public void SetUpTestServerListUpdater()
        {
            m_lviewServerList = new ListView();

            m_mockServerInfoMgr = new MockServerInfoMgr();
            m_mockServerInfoMgr.Init(new List<ServerInfo>());
            m_serverListViewUpdater = new ServerListViewUpdater(m_lviewServerList, COL_INDEX_SERVER_NAME, m_mockServerInfoMgr);
        }

        [Test]
        public void TestRebuildServerListView_AddItem()
        {
            Assert.AreEqual(0, m_lviewServerList.Items.Count);
            Assert.AreEqual(0, m_mockServerInfoMgr.ServerInfoCount);

            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_EXE_PATH, WORLD_ID1, SERVER_ID1);
            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(1, m_lviewServerList.Items.Count);

            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME2, TEST_EXE_PATH2, WORLD_ID2, SERVER_ID2);
            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(2, m_lviewServerList.Items.Count);
        }

        [Test]
        public void TestRebuildServerListView_ModifyItem()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_EXE_PATH, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(1, m_lviewServerList.Items.Count);

            Assert.AreEqual(PROCESS_STATE.TERMINATED, refServerInfo.ProcessState);
            Assert.AreEqual(PROCESS_STATE.TERMINATED.ToString(), m_lviewServerList.Items[0].SubItems[1].Text);

            refServerInfo.SetProcessStarting(TEST_PROCESS_ID);
            refServerInfo.SetProcessRunning();
            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(PROCESS_STATE.RUNNING, refServerInfo.ProcessState);
            Assert.AreEqual(PROCESS_STATE.RUNNING.ToString(), m_lviewServerList.Items[0].SubItems[1].Text);


            Assert.AreEqual(false, m_lviewServerList.Items[0].Checked);

            refServerInfo.SetProcessRevivable(true);
            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(true, m_lviewServerList.Items[0].Checked);
        }

        [Test]
        public void TestRebuildServerListView_CheckListViewInfoOriginalInfo()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_EXE_PATH, WORLD_ID1, SERVER_ID1);
            m_serverListViewUpdater.BuildServerListView();

            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(refServerInfo.ProcessState.ToString(), m_lviewServerList.Items[0].SubItems[1].Text);
            Assert.AreEqual(TEST_SERVER_NAME,                       m_lviewServerList.Items[0].SubItems[3].Text);
            Assert.AreEqual(TEST_SERVER_TYPE.ToString(),            m_lviewServerList.Items[0].SubItems[4].Text);
            Assert.AreEqual(refServerInfo.WorldID.ToString(),       m_lviewServerList.Items[0].SubItems[5].Text);
            Assert.AreEqual(refServerInfo.ServerID.ToString(),      m_lviewServerList.Items[0].SubItems[6].Text);
        }

        [Test]
        public void TestRebuildServerListView_NoServerInfo()
        {
            Assert.AreEqual(0, m_lviewServerList.Items.Count);
            Assert.AreEqual(0, m_mockServerInfoMgr.ServerInfoCount);

            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(0, m_lviewServerList.Items.Count);
        }

        [Test]
        public void TestRebuildServerListView()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_EXE_PATH, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo1 = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME2, TEST_EXE_PATH2, WORLD_ID2, SERVER_ID2);
            ServerInfo refServerInfo2 = m_mockServerInfoMgr.Find(TEST_SERVER_NAME2);

            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(TEST_SERVER_NAME,   m_lviewServerList.Items[0].SubItems[3].Text);
            Assert.AreEqual(false,              m_lviewServerList.Items[0].Checked);

            Assert.AreEqual(TEST_SERVER_NAME2,  m_lviewServerList.Items[1].SubItems[3].Text);
            Assert.AreEqual(false,              m_lviewServerList.Items[1].Checked);

            refServerInfo1.SetProcessRevivable(true);
            m_serverListViewUpdater.UpdateServerListInfo(TEST_SERVER_NAME);

            Assert.AreEqual(true,   m_lviewServerList.Items[0].Checked);
            Assert.AreEqual(false,  m_lviewServerList.Items[1].Checked);

            refServerInfo2.SetProcessRevivable(true);
            m_serverListViewUpdater.UpdateServerListInfo(TEST_SERVER_NAME2);

            Assert.AreEqual(true, m_lviewServerList.Items[0].Checked);
            Assert.AreEqual(true, m_lviewServerList.Items[1].Checked);
        }

        [Test]
        public void TestUpdateServerListInfo_CheckListViewInfoOriginalInfo()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_EXE_PATH, WORLD_ID1, SERVER_ID1);
            m_serverListViewUpdater.BuildServerListView();
            m_serverListViewUpdater.UpdateServerListInfo(TEST_SERVER_NAME);


            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            Assert.AreEqual(refServerInfo.ProcessState.ToString(), m_lviewServerList.Items[0].SubItems[1].Text);
            Assert.AreEqual(TEST_SERVER_NAME,                       m_lviewServerList.Items[0].SubItems[3].Text);
            Assert.AreEqual(TEST_SERVER_TYPE.ToString(),            m_lviewServerList.Items[0].SubItems[4].Text);
            Assert.AreEqual(refServerInfo.WorldID.ToString(),       m_lviewServerList.Items[0].SubItems[5].Text);
            Assert.AreEqual(refServerInfo.ServerID.ToString(),      m_lviewServerList.Items[0].SubItems[6].Text);
        }

        [Test]
        public void TestUpdateServerListInfo_InvalidServerName()
        {
            m_mockServerInfoMgr.AddServerInfo(TEST_SERVER_TYPE, TEST_SERVER_NAME, TEST_EXE_PATH, WORLD_ID1, SERVER_ID1);
            ServerInfo refServerInfo = m_mockServerInfoMgr.Find(TEST_SERVER_NAME);

            m_serverListViewUpdater.BuildServerListView();

            Assert.AreEqual(TEST_SERVER_NAME,   m_lviewServerList.Items[0].SubItems[3].Text);
            Assert.AreEqual(false,              m_lviewServerList.Items[0].Checked);

            refServerInfo.SetProcessRevivable(true);
            m_serverListViewUpdater.UpdateServerListInfo("InvalidServerName");

            Assert.AreEqual(false, m_lviewServerList.Items[0].Checked);

            m_serverListViewUpdater.UpdateServerListInfo(TEST_SERVER_NAME);

            Assert.AreEqual(true, m_lviewServerList.Items[0].Checked);
        }

    }
}
