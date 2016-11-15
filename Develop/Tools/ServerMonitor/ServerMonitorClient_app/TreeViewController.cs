using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SMCommon;

namespace ServerMonitorClient
{
    public class TreeViewController
    {
        private TreeIconSelector m_treeIconSelector = new TreeIconSelector();
        private TreeView m_refTreeView;

        public TreeViewController(TreeView refTreeview)
        {
            m_refTreeView = refTreeview;
        }
        public void AddMachineNode(string strMachineName, bool bConnected)
        {
            NODE_ICON eIconIndex = m_treeIconSelector.GetMachineIcon(bConnected);
            m_refTreeView.Nodes.Add(strMachineName, strMachineName, (int)eIconIndex, (int)eIconIndex);
        }
        public bool AddServerNode(string strMachineName, string strServerName)
        {
            TreeNode tnodeMachine = FindMachineNode(strMachineName);
            if (tnodeMachine == null) return false;

            NODE_ICON eServerIcon = NODE_ICON.ICON_INDEX_SERVER_NOT_DETECTED;
            tnodeMachine.Nodes.Add(strServerName, strServerName, (int)eServerIcon, (int)eServerIcon);
            tnodeMachine.Expand();
            return true;
        }
        public void Clear()
        {
            m_refTreeView.Nodes.Clear();
        }
        public void ClearAllServerNodes()
        {
            for (int i = 0; i < m_refTreeView.Nodes.Count; ++i)
            {
                m_refTreeView.Nodes[i].Nodes.Clear();
            }
        }
        public void ClearServerNodes(string strMachineName)
        {
            for (int i = 0; i < m_refTreeView.Nodes.Count; ++i)
            {
                if (strMachineName.Equals(m_refTreeView.Nodes[i].Name) == true)
                {
                    m_refTreeView.Nodes[i].Nodes.Clear();
                    break;
                }
            }
        }
        public bool IsSelectedServerNode()
        {
            if (m_refTreeView.SelectedNode == null) return false;
            if (IsSelectedMachineNode() == true) return false;
            return true;
        }
        public bool IsSelectedMachineNode()
        {
            if (m_refTreeView.SelectedNode == null) return false;
            return m_refTreeView.Nodes.Contains(m_refTreeView.SelectedNode);
        }
        public NODE_ICON GetSelectedNodeIcon()
        {
            if (m_refTreeView.SelectedNode == null) return NODE_ICON.ICON_INDEX_NONE;
            return (NODE_ICON)m_refTreeView.SelectedNode.ImageIndex;
        }
        public bool GetSelectedMachineName(out string strMachineName)
        {
            strMachineName = "";

            if (m_refTreeView.SelectedNode == null) return false;

            if (IsSelectedServerNode() == true)
            {
                strMachineName = m_refTreeView.SelectedNode.Parent.Name;
            }
            else if (IsSelectedMachineNode() == true)
            {
                strMachineName = m_refTreeView.SelectedNode.Name;
            }
            return true;
        }
        public bool GetSelectedServerName(out string strServerName)
        {
            strServerName = "";

            if (IsSelectedServerNode() == true)
                strServerName = m_refTreeView.SelectedNode.Name;
            else
                return false;
            return true;
        }

        public void SetMachineIcon(string strMachineName, bool bConnected)
        {
            TreeNode tnodeMachine = FindMachineNode(strMachineName);
            if (tnodeMachine == null) return;

            NODE_ICON eMachineIcon = m_treeIconSelector.GetMachineIcon(bConnected);

            tnodeMachine.Text = tnodeMachine.Name;
            tnodeMachine.ImageIndex = (int)eMachineIcon;
            tnodeMachine.SelectedImageIndex = (int)eMachineIcon;
        }
        public void SetServerIcon(string strMachineName, string strServerName, PROCESS_STATE eProcessState, SERVABLE_STATE eServableState, bool bRevivable)
        {
            TreeNode tnodeServer = FindServerNode(strMachineName, strServerName);
            if (tnodeServer == null) return;

            NODE_ICON eServerIcon = m_treeIconSelector.GetServerIcon(eProcessState, eServableState, bRevivable);

            tnodeServer.ImageIndex = (int)eServerIcon;
            tnodeServer.SelectedImageIndex = (int)eServerIcon;
        }
        public void UpdateMachineNodeExpansionInfo(string strMachineName, int nCurPlayerCount)
        {
            TreeNode tnodeMachine = FindMachineNode(strMachineName);
            if (tnodeMachine == null) return;

            string strKeyName = tnodeMachine.Name;
            tnodeMachine.Text = strKeyName + " [" + nCurPlayerCount + "]";
        }
        public void UpdateServerNodeExpansionInfo(string strMachineName, string strServerName, int nCurServerPlayerCount)
        {
            TreeNode tnodeServer = FindServerNode(strMachineName, strServerName);
            if (tnodeServer == null) return;

            string strKeyName = tnodeServer.Name;
            tnodeServer.Text =  strKeyName + " [" + nCurServerPlayerCount + "]";
        }

        #region Private
        private TreeNode FindMachineNode(string strMachineName)
        {
            TreeNode[] treeNodes;
            treeNodes = m_refTreeView.Nodes.Find(strMachineName, false);
            if (treeNodes.Length == 0) return null;

            return treeNodes[0];
        }
        private TreeNode FindServerNode(string strMachineName, string strServerName)
        {
            TreeNode tnodeMachine = FindMachineNode(strMachineName);
            if (tnodeMachine == null) return null;

            TreeNode[] tnodeServers = tnodeMachine.Nodes.Find(strServerName, false);
            if (tnodeServers.Length == 0) return null;

            return tnodeServers[0];
        }
        #endregion

    }

}
