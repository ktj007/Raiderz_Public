using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ServerMonitorClient;
using ServerMonitorClient_unittest.Mock;
using System.Windows.Forms;
using SMCommon;


namespace ServerMonitorClient_unittest
{
    [TestFixture]
    public class TestTreeViewController
    {
        private readonly string TEST_MACHINE_NAME1 = "TestMachine1";
        private readonly string TEST_MACHINE_NAME2 = "TestMachine2";

        private const string TEST_SERVER_NAME1 = "TestServer1";
        private const string TEST_SERVER_NAME2 = "TestServer2";

        private TreeView m_treeview = null;
        private TreeViewController m_treeViewController = null;

        [SetUp]
        public void SetUpTestTreeViewController()
        {
            m_treeview = new TreeView();
            m_treeViewController = new TreeViewController(m_treeview);
        }

        [Test]
        public void TestAddMachineNode()
        {
            Assert.AreEqual(0, m_treeview.Nodes.Count);

            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);

            Assert.AreEqual(1, m_treeview.Nodes.Count);
        }

        [Test]
        public void TestAddServerNode()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);

            Assert.AreEqual(0, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);

            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);
        }

        [Test]
        public void TestClear()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            Assert.AreEqual(1, m_treeview.Nodes.Count);
            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);

            m_treeViewController.Clear();

            Assert.AreEqual(0, m_treeview.Nodes.Count);
        }

        [Test]
        public void TestClearAllServerNodes()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME2, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME2, TEST_SERVER_NAME2);

            Assert.AreEqual(2, m_treeview.Nodes.Count);
            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);
            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME2].Nodes.Count);

            m_treeViewController.ClearAllServerNodes();

            Assert.AreEqual(2, m_treeview.Nodes.Count);
            Assert.AreEqual(0, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);
            Assert.AreEqual(0, m_treeview.Nodes[TEST_MACHINE_NAME2].Nodes.Count);
        }

        [Test]
        public void TestClearServerNodes()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME2, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME2, TEST_SERVER_NAME2);

            Assert.AreEqual(2, m_treeview.Nodes.Count);
            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);
            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME2].Nodes.Count);

            m_treeViewController.ClearServerNodes(TEST_MACHINE_NAME1);

            Assert.AreEqual(2, m_treeview.Nodes.Count);
            Assert.AreEqual(0, m_treeview.Nodes[TEST_MACHINE_NAME1].Nodes.Count);
            Assert.AreEqual(1, m_treeview.Nodes[TEST_MACHINE_NAME2].Nodes.Count);

            m_treeViewController.ClearServerNodes(TEST_MACHINE_NAME2);

            Assert.AreEqual(0, m_treeview.Nodes[TEST_MACHINE_NAME2].Nodes.Count);
        }

        [Test]
        public void TestIsSelectedServerNode()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Server Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0].Nodes[0];

            Assert.AreEqual(true, m_treeViewController.IsSelectedServerNode());
        }

        [Test]
        public void TestIsSelectedServerNode_Failure()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Machine Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0];

            Assert.AreEqual(false, m_treeViewController.IsSelectedServerNode());
        }

        [Test]
        public void TestIsSelectedMachineNode()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Machine Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0];

            Assert.AreEqual(true, m_treeViewController.IsSelectedMachineNode());
        }

        [Test]
        public void TestIsSelectedMachineNode_Failure()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Server Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0].Nodes[0];

            Assert.AreEqual(false, m_treeViewController.IsSelectedMachineNode());
        }

        [Test]
        public void TestGetSelectedMachineName_SelectMachineNode()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Machine Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0];

            string strMachineName;
            Assert.AreEqual(true, m_treeViewController.GetSelectedMachineName(out strMachineName));
            Assert.AreEqual(TEST_MACHINE_NAME1, strMachineName);
        }

        [Test]
        public void TestGetSelectedMachineName_SelectServerNode()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Server Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0].Nodes[0];

            string strMachineName;
            Assert.AreEqual(true, m_treeViewController.GetSelectedMachineName(out strMachineName));
            Assert.AreEqual(TEST_MACHINE_NAME1, strMachineName);
        }

        [Test]
        public void TestGetSelectedServerName()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Server Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0].Nodes[0];

            string strServerName;
            Assert.AreEqual(true, m_treeViewController.GetSelectedServerName(out strServerName));
            Assert.AreEqual(TEST_SERVER_NAME1, strServerName);
        }

        [Test]
        public void TestGetSelectedServerName_Failure_SelectedMachineNode()
        {
            m_treeViewController.AddMachineNode(TEST_MACHINE_NAME1, false);
            m_treeViewController.AddServerNode(TEST_MACHINE_NAME1, TEST_SERVER_NAME1);

            // Machine Node를 선택한 상태로 설정
            m_treeview.SelectedNode = m_treeview.Nodes[0];

            string strServerName;
            Assert.AreEqual(false, m_treeViewController.GetSelectedServerName(out strServerName));
            Assert.AreEqual(0, strServerName.Length);
        }

    }
}
