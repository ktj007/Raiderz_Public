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
    public partial class EditForm_Condition : EditForm
    {
        internal SHXmlCore xmlCore;
        protected ConditionsListViewController m_ListViewController;

        //////////////////////////////////////////////////////////////////////////

        public class ConditionsListViewController : BaseListViewController
        {
            public ConditionsListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
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
                if (m_XmlCore.Conditions.IsValid(m_nCopyObjectID) == false)
                {
                    return false;
                }

                SHConditions srcConds = m_XmlCore.Conditions[m_nCopyObjectID];
                SHConditions tarConds = (SHConditions)(tarDataObject);

                tarConds.Assign(srcConds);

                Global._VelixianForms.FindForm("CONDITION").Touch();

                return true;
            }

        }

        //////////////////////////////////////////////////////////////////////////

        public EditForm_Condition()
        {
            InitializeComponent();
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.CONDITION, lvConditions, "CONDITION", xmlCore.Conditions.LoadingFailed, null, null, xmlCore.SaveConditions);
            }

            m_ListViewController = new ConditionsListViewController(xmlCore, xmlCore.Conditions, xmlCore.Conditions.dataList, lvConditions, cbConditionFilter);
            m_ListViewController.InitListView(null);


            lvConditions.Tag = xmlCore.Conditions;
            SHListViewUtil.SelectIndex(lvConditions, 0);
        }

        private void lvConditions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvConditions.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvConditions.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHConditions))
                {
                    SHConditions conds = (SHConditions)lvi.Tag;
                    if (conds != null)
                    {
                        pgConditionElement.SelectedObject = conds;
                        pgConditionElement.Tag = lvi;

                        ConditionHelper conditionHelper = new ConditionHelper();
                        conditionHelper.ProcessConditions(xmlCore, tvConditionDetail, conds);

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

            node = tvConditionDetail.SelectedNode;
            lv = lvConditions;
            tv = tvConditionDetail;
            tab = Global._VelixianForms.FindForm("CONDITION");

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

        /// <summary>
        /// 조건을 추가하거나 지울때 쓰는 메뉴가 열릴때 호출
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void subCond_Opening(object sender, CancelEventArgs e)
        {

        }

        /// <summary>
        /// 조건문을 추가하거나 지울때 쓰는 메뉴가 열릴때 호출된다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void subCondDetail_Opening(object sender, CancelEventArgs e)
        {

        }

        private void subCondsAdd_Click(object sender, EventArgs e)
        {
            int nSelCondsID = m_ListViewController.MakeNewID();

            SHConditions newConds = new SHConditions();
            newConds.id = nSelCondsID;

            xmlCore.Conditions.Add(newConds);

            m_ListViewController.AddItem(newConds);

            Global._VelixianForms.FindForm("CONDITION").Touch();
        }

        private void subCondsDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                object tagObject = m_ListViewController.DeleteSelectedItem();
                if (tagObject != null)
                {
                    xmlCore.Conditions.dataList.Remove(tagObject);
                    Global._VelixianForms.FindForm("CONDITION").Touch();
                }

            }
        }

        /// <summary>
        /// 새로운 조건문 Or를 추가한다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void subCondDetailAdd_Or_Click(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// 새로운 조건문 Condition를 추가한다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void subCondDetailAdd_Cond_Click(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// 선택된 조건문을 삭제한다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void subCondDetailDel_Click(object sender, EventArgs e)
        {

        }

        private void tvCondDetail_AfterSelect(object sender, TreeViewEventArgs e)
        {
            PropertyGrid pgElement = null;

            pgElement = pgConditionElement;

            if (e.Node.Tag != null)
            {
                pgConditionElement.SelectedObject = e.Node.Tag;
            }
        }

        private void tvConditionDetail_MouseUp(object sender, MouseEventArgs e)
        {
            TreeView tvList = null;

            tvList = tvConditionDetail;

            if (e.Button == MouseButtons.Right)
            {
                tvList.SelectedNode = tvList.GetNodeAt(e.X, e.Y);
                if (tvList.SelectedNode != null) subCondition.Show(tvList, e.Location);
            }
        }

        private void subCondition_Opening(object sender, CancelEventArgs e)
        {
            ContextMenuStrip cms = sender as ContextMenuStrip;

            if (cms == null)
            {
                e.Cancel = true;
            }
            else
            {
                cms.Tag = tvConditionDetail;
                subCondsDel.Enabled = (tvConditionDetail.SelectedNode != null) ? true : false;
            }
        }

        private void subCondAdd_And_Click(object sender, EventArgs e)
        {
            TreeView tvList = subCondition.Tag as TreeView;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondAddClick(sender, e, xmlCore, tvList) == false) return;

            Global._VelixianForms.FindForm("CONDITION").Touch();
        }

        private void subCondAdd_Or_Click(object sender, EventArgs e)
        {
            TreeView tvList = subCondition.Tag as TreeView;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondAddClick(sender, e, xmlCore, tvList) == false) return;

            Global._VelixianForms.FindForm("CONDITION").Touch();

        }

        private void subCondAdd_Cond_Click(object sender, EventArgs e)
        {
            TreeView tvList = subCondition.Tag as TreeView;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondAddCondClick(sender, e, xmlCore, tvList) == false) return;

            Global._VelixianForms.FindForm("CONDITION").Touch();

        }

        private void subCondDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No) return;

            if (subCondition.Tag == null || subCondition.Tag.GetType() != typeof(TreeView)) return;

            TreeView tvList = subCondition.Tag as TreeView;
            ListView lvMainList = lvMainList = lvConditions;

            ConditionHelper conditionHelper = new ConditionHelper();
            if (conditionHelper.CondDelClick(sender, e, xmlCore, tvList, lvMainList) == false) return;


            Global._VelixianForms.FindForm("CONDITION").Touch();

        }

        private void EditForm_Condition_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);

            if (e.KeyCode == Keys.F3)
            {
                cbConditionFilter.Focus();
            }

        }

        private void EditForm_Condition_Load(object sender, EventArgs e)
        {
            this.lvConditions.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }

        private void lvConditions_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "Conditions");
        }

        private void cbConditionFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }

        private void subConditions_Opening(object sender, CancelEventArgs e)
        {
            if (m_ListViewController.IsCopied())
            {
                subCondPaste.Enabled = true;
            }
            else
            {
                subCondPaste.Enabled = false;
            }
        }

        private void subCondCopy_Click(object sender, EventArgs e)
        {
            m_ListViewController.CopyDataObject();
        }

        private void subCondPaste_Click(object sender, EventArgs e)
        {
            m_ListViewController.PasteDataObject();
        }


    }
}
