using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SoulHunt;
using WeifenLuo.WinFormsUI.Docking;


namespace Velixian
{
    public class ConditionHelper
    {
        public ConditionHelper()
        {

        }

        public bool CondAddClick(object sender, EventArgs e, SHXmlCore xmlCore, TreeView tvList)
        {
            if (tvList.SelectedNode == null || tvList.SelectedNode.Tag == null) return false;

            Type nodeType = tvList.SelectedNode.Tag.GetType();

            SHAnd newAnd = new SHAnd();

            if (nodeType == typeof(SHConditions))
            {
                SHConditions conds = (SHConditions)tvList.SelectedNode.Tag;
                conds.and = newAnd;
            }
            else if (nodeType == typeof(SHAnd))
            {
                SHAnd and = (SHAnd)tvList.SelectedNode.Tag;
                and.and = newAnd;
            }
            else if (nodeType == typeof(SHOr))
            {
                SHOr or = (SHOr)tvList.SelectedNode.Tag;
                or.and = newAnd;
            }
            else if (nodeType == typeof(SHCondition))
            {
                SHCondition cond = (SHCondition)tvList.SelectedNode.Tag;
                cond.and = newAnd;
            }
            else
            {
                return false;
            }

            TreeNode newNode = ProcessAnd(xmlCore, tvList.SelectedNode, newAnd);
            tvList.SelectedNode = newNode;

            return true;
        }

        public bool CondORClick(object sender, EventArgs e, SHXmlCore xmlCore, TreeView tvList)
        {
            if (tvList.SelectedNode == null || tvList.SelectedNode.Tag == null) return false;

            Type nodeType = tvList.SelectedNode.Tag.GetType();

            SHOr newOr = new SHOr();

            if (nodeType == typeof(SHConditions))
            {
                SHConditions conds = (SHConditions)tvList.SelectedNode.Tag;
                conds.or = newOr;
            }
            else if (nodeType == typeof(SHAnd))
            {
                SHAnd and = (SHAnd)tvList.SelectedNode.Tag;
                and.or = newOr;
            }
            else if (nodeType == typeof(SHOr))
            {
                SHOr or = (SHOr)tvList.SelectedNode.Tag;
                or.or = newOr;
            }
            else if (nodeType == typeof(SHCondition))
            {
                SHCondition cond = (SHCondition)tvList.SelectedNode.Tag;
                cond.or = newOr;
            }
            else
            {
                return false;
            }

            TreeNode newNode = ProcessOr(xmlCore, tvList.SelectedNode, newOr);
            tvList.SelectedNode = newNode;

            return true;
        }

        public bool CondAddCondClick(object sender, EventArgs e, SHXmlCore xmlCore, TreeView tvList)
        {
            if (tvList.SelectedNode == null || tvList.SelectedNode.Tag == null) return false;

            Type nodeType = tvList.SelectedNode.Tag.GetType();

            SHCondition newCondition = new SHCondition();
            newCondition.Compile();
            newCondition.Build(xmlCore);

            if (nodeType == typeof(SHConditions))
            {
                SHConditions conds = (SHConditions)tvList.SelectedNode.Tag;
                conds.dataList.Add(newCondition);
            }
            else if (nodeType == typeof(SHAnd))
            {
                SHAnd and = (SHAnd)tvList.SelectedNode.Tag;
                and.dataList.Add(newCondition);
            }
            else if (nodeType == typeof(SHOr))
            {
                SHOr or = (SHOr)tvList.SelectedNode.Tag;
                or.dataList.Add(newCondition);
            }
            else
            {
                return true;
            }

            TreeNode newNode = ProcessCondition(xmlCore, tvList.SelectedNode, newCondition);
            tvList.SelectedNode = newNode;


            return true;
        }

        public bool CondDelClick(object sender, EventArgs e, SHXmlCore xmlCore, TreeView tvList, ListView lvMainList)
        {
            SHConditions conds = tvList.Tag as SHConditions;


            if (tvList == null || conds == null || lvMainList == null || tvList.SelectedNode == null) return false;

            TreeNode selectedNode = tvList.SelectedNode;

            if (selectedNode == null || selectedNode.Tag == null) return false;

            if (selectedNode.Tag.GetType() == typeof(SHAnd))
            {
                SHAnd and = selectedNode.Tag as SHAnd;

                if (selectedNode.Parent.Tag.GetType() == typeof(SHConditions))
                {
                    SHConditions parentConditions = (SHConditions)selectedNode.Parent.Tag;
                    parentConditions.and = null;
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHAnd))
                {
                    SHAnd parentAnd = (SHAnd)selectedNode.Parent.Tag;
                    parentAnd.and = null;
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHOr))
                {
                    SHOr parentOr = (SHOr)selectedNode.Parent.Tag;
                    parentOr.and = null;
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHCondition))
                {
                    SHCondition parentCondition = (SHCondition)selectedNode.Parent.Tag;
                    parentCondition.and = null;
                }
                else
                {
                    return false;
                }
            }
            else if (selectedNode.Tag.GetType() == typeof(SHOr))
            {
                SHOr or = selectedNode.Tag as SHOr;

                if (selectedNode.Parent.Tag.GetType() == typeof(SHConditions))
                {
                    SHConditions parentConditions = (SHConditions)selectedNode.Parent.Tag;
                    parentConditions.or = null;
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHAnd))
                {
                    SHAnd parentAnd = (SHAnd)selectedNode.Parent.Tag;
                    parentAnd.or = null;
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHOr))
                {
                    SHOr parentOr = (SHOr)selectedNode.Parent.Tag;
                    parentOr.or = null;
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHCondition))
                {
                    SHCondition parentCondition = (SHCondition)selectedNode.Parent.Tag;
                    parentCondition.or = null;
                }
                else
                {
                    return false;
                }
            }
            else if (selectedNode.Tag.GetType() == typeof(SHCondition))
            {
                SHCondition cond = selectedNode.Tag as SHCondition;

                if (selectedNode.Parent.Tag.GetType() == typeof(SHConditions))
                {
                    SHConditions parentConditions = (SHConditions)selectedNode.Parent.Tag;
                    parentConditions.dataList.Remove(cond);
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHAnd))
                {
                    SHAnd parentAnd = (SHAnd)selectedNode.Parent.Tag;
                    parentAnd.dataList.Remove(cond);
                }
                else if (selectedNode.Parent.Tag.GetType() == typeof(SHOr))
                {
                    SHOr parentOr = (SHOr)selectedNode.Parent.Tag;
                    parentOr.dataList.Remove(cond);
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            tvList.Nodes.Remove(selectedNode);
            selectedNode = selectedNode.Parent;

            return true;
        }

        public TreeNode ProcessAnd(SHXmlCore xmlCore, TreeNode nodeParent, SHAnd and)
        {
            if (and == null || nodeParent == null) return null;

            TreeNode nodeThis = new TreeNode(and.GetString(xmlCore));
            nodeThis.ImageIndex = 5;
            nodeThis.SelectedImageIndex = 5;
            nodeThis.Tag = and;
            nodeParent.Nodes.Add(nodeThis);

            if (and.and != null)
            {
                ProcessAnd(xmlCore, nodeThis, and.and);
            }

            if (and.or != null)
            {
                ProcessOr(xmlCore, nodeThis, and.or);
            }

            for (int i = 0; i < and.dataList.Count; i++)
            {
                SHCondition cond = and.dataList[i] as SHCondition;
                ListViewItem lvi = new ListViewItem();

                ProcessCondition(xmlCore, nodeThis, cond);
            }

            return nodeThis;
        }

        private TreeNode ProcessOr(SHXmlCore xmlCore, TreeNode nodeParent, SHOr or)
        {
            if (or == null || nodeParent == null) return null;

            TreeNode nodeThis = new TreeNode(or.GetString(xmlCore));
            nodeThis.ImageIndex = 5;
            nodeThis.SelectedImageIndex = 5;
            nodeThis.Tag = or;
            nodeParent.Nodes.Add(nodeThis);

            if (or.and != null)
            {
                ProcessAnd(xmlCore, nodeThis, or.and);
            }

            if (or.or != null)
            {
                ProcessOr(xmlCore, nodeThis, or.or);
            }

            for (int i = 0; i < or.dataList.Count; i++)
            {
                SHCondition cond = or.dataList[i] as SHCondition;
                ListViewItem lvi = new ListViewItem();

                ProcessCondition(xmlCore, nodeThis, cond);
            }

            return nodeThis;
        }

        public TreeNode ProcessCondition(SHXmlCore xmlCore, TreeNode nodeParent, SHCondition cond)
        {
            if (nodeParent == null || cond == null) return null;

            TreeNode nodeThis = new TreeNode(cond.GetString(xmlCore));
            nodeThis.ImageIndex = 5;
            nodeThis.SelectedImageIndex = 5;
            nodeThis.Tag = cond;
            nodeParent.Nodes.Add(nodeThis);


            if (cond.and != null)
            {
                ProcessAnd(xmlCore, nodeThis, cond.and);
            }

            if (cond.or != null)
            {
                ProcessOr(xmlCore, nodeThis, cond.or);
            }

            nodeThis.ForeColor = (cond.Passed) ? Color.Black : Color.Red;
            nodeThis.BackColor = (cond.Passed) ? Color.White : Color.Yellow;

            return nodeThis;
        }

        /// <param name="lvTarget">구체적인 조건들이 명시될 리스트뷰를 지정한다.</param>
        /// <param name="conds">조건들의 목록</param>
        public TreeNode ProcessConditions(SHXmlCore xmlCore, TreeView tvList, SHConditions conds)
        {
            if (conds == null) return null;

            TreeNode nodeRoot = new TreeNode(conds.GetString(xmlCore));
            nodeRoot.ImageIndex = 4;
            nodeRoot.SelectedImageIndex = 4;
            nodeRoot.Tag = conds;

            if (conds.and != null)
            {
                ProcessAnd(xmlCore, nodeRoot, conds.and);
            }

            if (conds.or != null)
            {
                ProcessOr(xmlCore, nodeRoot, conds.or);
            }

            for (int i = 0; i < conds.dataList.Count; i++)
            {
                SHCondition cond = conds.dataList[i] as SHCondition;
                ProcessCondition(xmlCore, nodeRoot, cond);
            }

            tvList.Tag = conds;

            tvList.Nodes.Clear();
            tvList.Nodes.Add(nodeRoot);
            tvList.ExpandAll();
            tvList.SelectedNode = nodeRoot;

            return nodeRoot;
        }
    }
}