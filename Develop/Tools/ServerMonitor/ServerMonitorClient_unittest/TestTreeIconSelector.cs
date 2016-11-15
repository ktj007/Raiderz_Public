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
    public class TestTreeIconSelector
    {
        private TreeIconSelector m_treeIconSelector = null;
        NODE_ICON m_eIcon;

        [SetUp]
        public void SetUpTestTreeIconSelector()
        {
            m_treeIconSelector = new TreeIconSelector();
            m_eIcon = NODE_ICON.ICON_INDEX_NONE;
        }

        [Test]
        public void TestSetMachineIcon_CONNECTED()
        {
            m_eIcon = m_treeIconSelector.GetMachineIcon(false);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_MACHINE_DISCONN, m_eIcon);

            m_eIcon = m_treeIconSelector.GetMachineIcon(true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_MACHINE_CONN, m_eIcon);
        }

        [Test]
        public void TestSetServerIcon_Error()
        {
            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.DIED, SERVABLE_STATE.ACTIVE, true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_ERROR, m_eIcon);

            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.NOUPDATE, SERVABLE_STATE.ACTIVE, true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_ERROR, m_eIcon);
        }

        [Test]
        public void TestSetServerIcon_Terminate()
        {
            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.TERMINATING, SERVABLE_STATE.ACTIVE, true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_PROCESS_CHANGING, m_eIcon);

            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.TERMINATED, SERVABLE_STATE.ACTIVE, true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_BAD, m_eIcon);
        }

        [Test]
        public void TestSetServerIcon_Fine()
        {
            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.RUNNING, SERVABLE_STATE.ACTIVE, true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_FINE, m_eIcon);

            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.RUNNING, SERVABLE_STATE.ACTIVE, false);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_FINE_NOT_REVIVE, m_eIcon);
        }

        [Test]
        public void TestSetServerIcon_Inactive()
        {
            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.RUNNING, SERVABLE_STATE.INACTIVE, true);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_INACTIVE, m_eIcon);

            m_eIcon = m_treeIconSelector.GetServerIcon(PROCESS_STATE.RUNNING, SERVABLE_STATE.INACTIVE, false);
            Assert.AreEqual(NODE_ICON.ICON_INDEX_SERVER_INACTIVE, m_eIcon);
        }

    }
}
