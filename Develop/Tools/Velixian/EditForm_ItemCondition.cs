using System;
using System.Collections.Generic;
using System.Collections;
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
    public partial class EditForm_ItemCondition : EditForm
    {
        internal SHXmlCore xmlCore;
        protected ItemConditionsListViewController m_ListViewController;

        //////////////////////////////////////////////////////////////////////////

        public class ItemConditionsListViewController : BaseListViewController
        {
            public ItemConditionsListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
                System.Windows.Forms.ListView listView, System.Windows.Forms.ComboBox cbFilter)
                : base(xmlCore, xmlDataObject, arrayList, listView, cbFilter)
            {

            }

            public override void SetListText(ListViewItem lvi, SHDataObject selDataObject)
            {
                SHConditions conds = (SHConditions)(selDataObject);

                if (lvi == null) return;
                if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

                conds.Compile();
                conds.Build(m_XmlCore);

                lvi.Text = (conds.idSpecified) ? conds.id.ToString() : String.Empty;

                lvi.SubItems.AddRange(new String[] { (conds.comment != null) ? conds.comment : String.Empty });

                if (!conds.Passed) lvi.ImageIndex = 7; else lvi.ImageIndex = -1;

                lvi.Tag = conds;
                lvi.ForeColor = (conds.Passed) ? Color.Black : Color.Red;
                lvi.BackColor = (conds.Passed) ? Color.White : Color.Yellow;

            }

            protected override int GetDataObjectID(object listViewItemTag)
            {
                if (listViewItemTag.GetType() != typeof(SHConditions)) return 0;

                SHConditions selConds = (SHConditions)listViewItemTag;

                if (selConds != null)
                {
                    return selConds.id;
                }
                return 0;
            }
            protected override bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
            {
                SHConditions conds = (SHConditions)(dataObject);

                if (strHead.ToLower() == "id") return ExistFilterString(strFilter, conds.id.ToString());
                if (strHead.ToLower() == "comment" || strHead == "주석") return ExistFilterString(strFilter, conds.comment);

                if (ExistFilterString(strFilter, conds.id.ToString())) return true;
                if (ExistFilterString(strFilter, conds.comment)) return true;


                return false;
            }

            protected override bool _PasteDataObject(SHDataObject tarDataObject)
            {
                if (m_XmlCore.ItemConditions.IsValid(m_nCopyObjectID) == false)
                {
                    return false;
                }

                SHConditions srcConds = m_XmlCore.ItemConditions[m_nCopyObjectID];
                SHConditions tarConds = (SHConditions)(tarDataObject);

                tarConds.Assign(srcConds);

                Global._VelixianForms.FindForm("ITEMCONDITION").Touch();

                return true;
            }

        }

        //////////////////////////////////////////////////////////////////////////

        public EditForm_ItemCondition()
        {
            InitializeComponent();
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.ITEMCONDITION, lvIConditions, "ITEMCONDITION", xmlCore.ItemConditions.LoadingFailed, null, null, xmlCore.SaveItemConditions);
            }

            m_ListViewController = new ItemConditionsListViewController(xmlCore, xmlCore.ItemConditions, xmlCore.ItemConditions.dataList, lvIConditions, cbIConditionFilter);
            m_ListViewController.InitListView(null);

            lvIConditions.Tag = xmlCore.ItemConditions;
            SHListViewUtil.SelectIndex(lvIConditions, 0);

            
        }

        private void lvIConditions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvIConditions.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvIConditions.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHConditions))
                {
                    SHConditions conds = (SHConditions)lvi.Tag;
                    if (conds != null)
                    {
                        pgIConditionElement.SelectedObject = conds;
                        pgIConditionElement.Tag = lvi;

                        ConditionHelper conditionHelper = new ConditionHelper();
                        conditionHelper.ProcessConditions(xmlCore, tvIConditionDetail, conds);
                        Global._mainForm.SetStatusLabelText(conds.CompiledMessage);
                    }
                }
            }
            m_ListViewController.OnSelectedIndexChanged();
        }


        private void pgConditionElement_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            TreeNode node = null;
            ListView lv = null;
            TreeView tv = null;
            VelixianForm tab = null;

            node = tvIConditionDetail.SelectedNode;
            lv = lvIConditions;
            tv = tvIConditionDetail;
            tab = Global._VelixianForms.FindForm("ITEMCONDITION");

            if (node.Tag.GetType() == typeof(SHConditions))
            {
                SHConditions conds = (SHConditions)node.Tag;
                node.Text = conds.GetString(xmlCore);
            }
            else if (node.Tag.GetType() == typeof(SHCondition))
            {
                SHCondition cond = (SHCondition)node.Tag;
                node.Text = cond.GetString(xmlCore);
            }
            else if (node.Tag.GetType() == typeof(SHAnd))
            {
                SHAnd and = (SHAnd)node.Tag;
                node.Text = and.GetString(xmlCore);
            }
            else if (node.Tag.GetType() == typeof(SHOr))
            {
                SHOr or = (SHOr)node.Tag;
                node.Text = or.GetString(xmlCore);
            }

            if (node.Tag != null)
            {
                if (lv.SelectedItems[0].Tag == tv.TopNode.Tag)
                {
                    SHConditions conds = (SHConditions)tv.Nodes[0].Tag;
                    m_ListViewController.SetListText(lv.SelectedItems[0], conds);
                }
            }

            tab.Touch();
        }


        private void tvCondDetail_AfterSelect(object sender, TreeViewEventArgs e)
        {
            PropertyGrid pgElement = null;

            pgElement = pgIConditionElement;

            if (e.Node.Tag != null)
            {
                pgIConditionElement.SelectedObject = e.Node.Tag;
            }
        }

        private void tvConditionDetail_MouseUp(object sender, MouseEventArgs e)
        {
            TreeView tvList = null;

            tvList = tvIConditionDetail;


            if (e.Button == MouseButtons.Right)
            {
                tvList.SelectedNode = tvList.GetNodeAt(e.X, e.Y);
                if (tvList.SelectedNode != null) subItemCondition.Show(tvList, e.Location);
            }
        }

        private void EditForm_ItemCondition_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);

            if (e.KeyCode == Keys.F3)
            {
                cbIConditionFilter.Focus();
            }

        }

        private void EditForm_ItemCondition_Load(object sender, EventArgs e)
        {
            this.lvIConditions.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }

        private void subItemCondsAdd_Click(object sender, EventArgs e)
        {
            int nSelCondsID = m_ListViewController.MakeNewID();

            SHConditions newConds = new SHConditions();
            newConds.id = nSelCondsID;

            xmlCore.ItemConditions.Add(newConds);

            m_ListViewController.AddItem(newConds);

            Global._VelixianForms.FindForm("ITEMCONDITION").Touch();

        }

        private void subItemCondsDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                object tagObject = m_ListViewController.DeleteSelectedItem();
                if (tagObject != null)
                {
                    xmlCore.ItemConditions.dataList.Remove(tagObject);
                    Global._VelixianForms.FindForm("ITEMCONDITION").Touch();
                }

            }
        }

        private void subItemCondAdd_And_Click(object sender, EventArgs e)
        {
            TreeView tvList = subItemCondition.Tag as TreeView;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondAddClick(sender, e, xmlCore, tvList) == false) return;

            Global._VelixianForms.FindForm("ITEMCONDITION").Touch();
        }

        private void subItemCondition_Opening(object sender, CancelEventArgs e)
        {
            ContextMenuStrip cms = sender as ContextMenuStrip;

            if (cms == null)
            {
                e.Cancel = true;
            }
            else
            {
                cms.Tag = tvIConditionDetail;
                subItemCondsDel.Enabled = (tvIConditionDetail.SelectedNode != null) ? true : false;
            }
        }

        private void subItemCondAdd_Or_Click(object sender, EventArgs e)
        {
            TreeView tvList = subItemCondition.Tag as TreeView;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondAddClick(sender, e, xmlCore, tvList) == false) return;

            Global._VelixianForms.FindForm("ITEMCONDITION").Touch();
        }

        private void subItemCondAdd_Cond_Click(object sender, EventArgs e)
        {
            TreeView tvList = subItemCondition.Tag as TreeView;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondAddCondClick(sender, e, xmlCore, tvList) == false) return;

            Global._VelixianForms.FindForm("ITEMCONDITION").Touch();

        }

        private void subItemCondDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No) return;

            if (subItemCondition.Tag == null || subItemCondition.Tag.GetType() != typeof(TreeView)) return;

            TreeView tvList = subItemCondition.Tag as TreeView;
            ListView lvMainList = lvMainList = lvIConditions;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondDelClick(sender, e, xmlCore, tvList, lvMainList) == false) return;


            Global._VelixianForms.FindForm("ITEMCONDITION").Touch();
        }

        private void lvIConditions_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "Item Conditions");
        }

        private void cbIConditionFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }

        private void mnuICondsCopy_Click(object sender, EventArgs e)
        {
            m_ListViewController.CopyDataObject();
        }

        private void mnuICondsPaste_Click(object sender, EventArgs e)
        {
            m_ListViewController.PasteDataObject();
        }

        private void subItemConditions_Opening(object sender, CancelEventArgs e)
        {
            if (m_ListViewController.IsCopied())
            {
                mnuICondsPaste.Enabled = true;
            }
            else
            {
                mnuICondsPaste.Enabled = false;
            }

        }

    }
}
