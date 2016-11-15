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
    public partial class EditForm_Dialog : EditForm
    {
        internal SHXmlCore xmlCore;
        protected DialogListViewController m_ListViewController;

        //////////////////////////////////////////////////////////////////////////

        public class DialogListViewController : BaseListViewController
        {
            public DialogListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
                System.Windows.Forms.ListView listView, System.Windows.Forms.ComboBox cbFilter)
                : base(xmlCore, xmlDataObject, arrayList, listView, cbFilter)
            {

            }

            public override void SetListText(ListViewItem lvi, SHDataObject selDataObject)
            {
                SHDialog dialog = (SHDialog)(selDataObject);

                if (lvi == null) return;
                if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

                dialog.Compile();
                dialog.Build(m_XmlCore);

                lvi.Text = dialog.id.ToString();
                String strType = m_XmlCore.GetSafeString(dialog.ToString());
                lvi.SubItems.AddRange(new String[]{
                m_XmlCore.GetSafeString(dialog.text),
                ((strType==null)?"":strType)});

                lvi.Tag = dialog;
                if (!dialog.Passed) lvi.ImageIndex = 7;

                lvi.ForeColor = (dialog.Passed) ? Color.Black : Color.Red;
                lvi.BackColor = (dialog.Passed) ? Color.White : Color.Yellow;

            }
            protected override int GetDataObjectID(object listViewItemTag)
            {
                if (listViewItemTag.GetType() != typeof(SHDialog)) return 0;

                SHDialog selDialog = (SHDialog)listViewItemTag;

                if (selDialog != null)
                {
                    return selDialog.id;
                }
                return 0;
            }

            protected override bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
            {
                SHDialog dialog = (SHDialog)(dataObject);

                string strType = m_XmlCore.GetSafeString(dialog.ToString());
                if (strType == null) strType = "";
                string strText = m_XmlCore.GetSafeString(dialog.text);

                if (strHead.ToLower() == "id") return ExistFilterString(strFilter, dialog.id.ToString());
                if (strHead.ToLower() == "text" || strHead == "시작대화") return ExistFilterString(strFilter, strText);
                if (strHead.ToLower() == "type" || strHead == "타입") return ExistFilterString(strFilter, strType);


                if (ExistFilterString(strFilter, dialog.id.ToString())) return true;
                if (ExistFilterString(strFilter, strText)) return true;
                if (ExistFilterString(strFilter, strType)) return true;


                return false;
            }

        }


        public EditForm_Dialog()
        {
            InitializeComponent();
            wbDialogDetail.ScriptErrorsSuppressed = true;
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (wbDialogDetail.ObjectForScripting == null) wbDialogDetail.ObjectForScripting = mainForm;

            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.DIALOG, lvDialogs, "DIALOG", xmlCore.Dialogs.LoadingFailed, null, null, xmlCore.SaveDialogs);
            }

            m_ListViewController = new DialogListViewController(xmlCore, xmlCore.Dialogs, xmlCore.Dialogs.dataList, lvDialogs, cbDialogFilter);
            m_ListViewController.InitListView(null);

            SHListViewUtil.SelectIndex(lvDialogs, 0);

            
        }

        private void subDialogAdd_Click(object sender, EventArgs e)
        {
            int nNewDlgID = m_ListViewController.MakeNewID();

            SHDialog newDlg = new SHDialog();

            newDlg.id = nNewDlgID;
            newDlg.Say = null;
            newDlg.exit = 1;
            newDlg.text = "[New String]";

            xmlCore.Dialogs.Add(newDlg);

            m_ListViewController.AddItem(newDlg);

            Global._VelixianForms.FindForm("DIALOG").Touch();

        }

        private void subDialogDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                if (lvDialogs.SelectedItems.Count > 0)
                {
                    ListViewItem lvi = lvDialogs.SelectedItems[0];
                    if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHDialog))
                    {
                        xmlCore.Dialogs.dataList.Remove(lvi.Tag);
                        lvi.Remove();
                    }
                }
            }

            Global._VelixianForms.FindForm("DIALOG").Touch();
        }

        private void subDialogDetailAdd_Click(object sender, EventArgs e)
        {
            if (tvDialogDetail.SelectedNode == null || tvDialogDetail.SelectedNode.Tag == null) return;

            Type nodeType = tvDialogDetail.SelectedNode.Tag.GetType();

            if (nodeType == typeof(SHDialog))
            {
                SHDialog dlg = (SHDialog)tvDialogDetail.SelectedNode.Tag;
                if (dlg.Say == null)
                {
                    SHDialogSay say = new SHDialogSay();
                    say.text = "[New SAY Statement]";

                    dlg.exit = 0;
                    dlg.Say = say;

                    SHDialogSelect newSel = new SHDialogSelect();
                    newSel.text = "[New SELECT Statement]";
                    newSel.exit = 1;
                    say.Add(newSel);

                    ProcessDialog(dlg);
                    m_ListViewController.SetListText(lvDialogs.SelectedItems[0], dlg);
                }
                else
                {
                    MessageBox.Show("Only one SAY to each dialogue.", Application.ProductName);
                }
            }
            else if (nodeType == typeof(SHDialogSay))
            {
                SHDialogSay say = (SHDialogSay)tvDialogDetail.SelectedNode.Tag;
                SHDialogSelect newSel = new SHDialogSelect();

                newSel.text = "[New SELECT Statement]";
                newSel.exit = 1;

                say.Add(newSel);

                TreeNode newNode = ProcessSelect(tvDialogDetail.SelectedNode, newSel);
                tvDialogDetail.SelectedNode = newNode;
            }
            else if (nodeType == typeof(SHDialogSelect))
            {
                // 항상 SELECT로 끝나야 하기 때문에 새로운 SAY문을 넣으면 하위 SELECT까지 자동으로 추가한다.
                SHDialogSelect sel = (SHDialogSelect)tvDialogDetail.SelectedNode.Tag;
                if (sel.Say == null)
                {
                    SHDialogSay newSay = new SHDialogSay();
                    newSay.text = "[New SAY Statement]";

                    sel.exit = 0;

                    SHDialogSelect newSel = new SHDialogSelect();
                    newSel.text = "[New SELECT Statement]";
                    newSel.exit = 1;

                    newSay.Add(newSel);

                    sel.Say = newSay;   // 부모에 새로 만든 노드들을 연결시켜준다.

                    SetTreeNodeFromSel(tvDialogDetail.SelectedNode, sel.text, sel.exit, sel.exitSpecified);

                    TreeNode newNode = ProcessSay(tvDialogDetail.SelectedNode, newSay);
                    newNode.Expand();
                    tvDialogDetail.SelectedNode = newNode;
                }
                else
                {
                    MessageBox.Show("SELECT에 연결된 SAY문은 한개만 올수 있습니다.", Application.ProductName);
                }
            }

            Global._VelixianForms.FindForm("DIALOG").Touch();
        }

        private void subDialogDetailDel_Click(object sender, EventArgs e)
        {
            if (tvDialogDetail.SelectedNode == null || tvDialogDetail.SelectedNode.Tag == null) return;

            SHDialog dlg = (SHDialog)tvDialogDetail.TopNode.Tag;
            Type nodeType = tvDialogDetail.SelectedNode.Tag.GetType();

            if (nodeType == typeof(SHDialog))
            {
                MessageBox.Show("For safety you can not delete the conversation.", Application.ProductName);
                return;
            }

            if (nodeType == typeof(SHDialogSay))
            {
                SHDialogSay say = (SHDialogSay)tvDialogDetail.SelectedNode.Tag;

                if ((say.dataList.Count > 0 && MessageBox.Show("All subnodes will be deleted, Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No)
                    || (say.dataList.Count == 0 && MessageBox.Show("Are you 100% sure?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No)) return;

                if (tvDialogDetail.SelectedNode.Parent.Tag.GetType() == typeof(SHDialog))
                {
                    dlg.Say = null;
                    dlg.exit = 1;

                    TreeNode pNode = ProcessDialog(dlg);
                    tvDialogDetail.SelectedNode = pNode;
                }
                else if (tvDialogDetail.SelectedNode.Parent.Tag.GetType() == typeof(SHDialogSelect))
                {
                    SHDialogSelect sel = (SHDialogSelect)tvDialogDetail.SelectedNode.Parent.Tag;

                    sel.Say = null;
                    sel.exit = 1;

                    SetTreeNodeFromSel(tvDialogDetail.SelectedNode.Parent, sel.text, sel.exit, sel.exitSpecified);
                    tvDialogDetail.Nodes.Remove(tvDialogDetail.SelectedNode);

                    tvDialogDetail.SelectedNode = tvDialogDetail.SelectedNode.Parent;
                }
            }
            else if (nodeType == typeof(SHDialogSelect))
            {
                SHDialogSelect sel = (SHDialogSelect)tvDialogDetail.SelectedNode.Tag;

                if (tvDialogDetail.SelectedNode.Parent == null || tvDialogDetail.SelectedNode.Parent.Tag.GetType() != typeof(SHDialogSay)) return;

                // Parent
                SHDialogSay say = (SHDialogSay)tvDialogDetail.SelectedNode.Parent.Tag;

                if ((sel.Say != null && MessageBox.Show("이 작업으로 하위 노드까지 전부 삭제됩니다. 계속하시겠습니까?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No)
                    || (say.dataList.Count < 2 && sel.Say == null && MessageBox.Show("대화문은 DIALOG 또는 SELECT로 반드시 종료되어야 합니다.\n이 작업으로 인하여 논리적 오류가 존재하는 대화문이 생성될 수 있습니다. 계속하시겠습니까?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No)
                    || (say.dataList.Count >= 2 && sel.Say == null && MessageBox.Show("정말 삭제하시겠습니까?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No)) return;

                tvDialogDetail.Nodes.Remove(tvDialogDetail.SelectedNode);
                say.dataList.Remove(sel);
            }
            else if (nodeType == typeof(SHItemAdd) || nodeType == typeof(SHItemRemove) || nodeType == typeof(SHItemFill))
            {
                SHItemAddRemoveBase itemAdd = (SHItemAddRemoveBase)tvDialogDetail.SelectedNode.Tag;

                if (MessageBox.Show("정말 삭제하시겠습니까?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.No) return;

                if (tvDialogDetail.SelectedNode.Parent.Tag.GetType() == typeof(SHDialogSelect))
                {
                    SHDialogSelect sel = (SHDialogSelect)tvDialogDetail.SelectedNode.Parent.Tag;

                    for (int i = 0; i < sel.Item.dataList.Count; i++)
                    {
                        if (sel.Item.dataList[i] == tvDialogDetail.SelectedNode.Tag)
                        {
                            sel.Item.dataList.Remove(tvDialogDetail.SelectedNode.Tag);
                            break;
                        }
                    }

                    if (sel.Item.dataList.Count == 0)
                    {
                        sel.Item = null;
                    }

                    tvDialogDetail.Nodes.Remove(tvDialogDetail.SelectedNode);
                    tvDialogDetail.SelectedNode = tvDialogDetail.SelectedNode.Parent;
                }
            }

            // Update
            m_ListViewController.SetListText(lvDialogs.SelectedItems[0], dlg);

            Global._VelixianForms.FindForm("DIALOG").Touch();
        }

        private void subDialog_Opening(object sender, CancelEventArgs e)
        {
            subDialogDel.Enabled = (lvDialogs.SelectedItems.Count > 0);
        }

        private void subDialogDetail_Opening(object sender, CancelEventArgs e)
        {
            subDialogDetailDel.Enabled = (tvDialogDetail.SelectedNode != null);
        }

        private void RefreshDialogDetail(SHDialog dlg)
        {
            if (dlg != null)
            {
                wbDialogDetail.DocumentText = dlg.GetHTML(xmlCore);
            }
        }

        private void RefreshDialogDetail()
        {
            if (lvDialogs.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvDialogs.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHDialog))
                {
                    SHDialog dlg = (SHDialog)lvi.Tag;

                    if (dlg != null)
                    {
                        RefreshDialogDetail(dlg);
                    }
                }
            }
        }

        private void lvDialogs_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvDialogs.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvDialogs.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHDialog))
                {
                    SHDialog dlg = (SHDialog)lvi.Tag;

                    if (dlg != null)
                    {
                        RefreshDialogDetail(dlg);

                        ProcessDialog(dlg);
                        Global._mainForm.SetStatusLabelText(dlg.CompiledMessage);
                    }
                }
            }
            m_ListViewController.OnSelectedIndexChanged();

        }

        private void tvDialogDetail_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag != null)
            {
                pgDialogElement.SelectedObject = e.Node.Tag;
            }

        }

        private void tvDialogDetail_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                tvDialogDetail.SelectedNode = tvDialogDetail.GetNodeAt(e.X, e.Y);
                if (tvDialogDetail.SelectedNode != null) subDialogDetail.Show(tvDialogDetail, e.Location);
            }

        }

        private void tvDialogDetail_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete) subDialogDetailDel_Click(sender, e);
        }


        private void Refresh_WhenPropertyValueChanged(TreeNode node)
        {
            if (node.Tag != null)
            {
                if (lvDialogs.SelectedItems[0].Tag == tvDialogDetail.TopNode.Tag)
                {
                    SHDialog dlg = (SHDialog)tvDialogDetail.Nodes[0].Tag;
                    m_ListViewController.SetListText(lvDialogs.SelectedItems[0], dlg);
                }
            }

            RefreshDialogDetail();

            Global._VelixianForms.FindForm("DIALOG").Touch();
        }

        private void pgDialogElement_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            TreeNode node = tvDialogDetail.SelectedNode;
            if (node.Tag.GetType() == typeof(SHDialog))
            {
                SHDialog dlg = (SHDialog)node.Tag;
                node.Text = xmlCore.GetSafeString(dlg.text);

                if (e.ChangedItem.Label == "(아이디)" && dlg.id != (int)e.OldValue)
                {
                    m_ListViewController.SortListView(dlg);
                }
            }
            else if (node.Tag.GetType() == typeof(SHDialogSay))
            {
                SHDialogSay say = (SHDialogSay)node.Tag;
                node.Text = xmlCore.GetSafeString(say.text);
            }
            else if (node.Tag.GetType() == typeof(SHDialogSelect))
            {
                SHDialogSelect sel = (SHDialogSelect)node.Tag;
                SetTreeNodeFromSel(node, sel.text, sel.exit, sel.exitSpecified);
            }
            else if (node.Tag.GetType() == typeof(SHItemAdd) || node.Tag.GetType() == typeof(SHItemRemove) || node.Tag.GetType() == typeof(SHItemFill))
            {
                SHItemAddRemoveBase itemBase = (SHItemAddRemoveBase)node.Tag;
                SetTreeNodeFromItem(node, itemBase);
            }

            Refresh_WhenPropertyValueChanged(node);
        }

        private void tvDialogDetail_DoubleClick(object sender, EventArgs e)
        {
            tvDialogDetail.ExpandAll();

            if (tvDialogDetail.SelectedNode == null || tvDialogDetail.SelectedNode.Tag == null) return;

            Type nodeType = tvDialogDetail.SelectedNode.Tag.GetType();
            if (nodeType == typeof(SHDialogSay))
            {
                SHDialogSay say = (SHDialogSay)tvDialogDetail.SelectedNode.Tag;

                SHCE_TextEditor editor = new SHCE_TextEditor();

                editor.SetText((String)say.text);

                if (editor.ShowDialog() == DialogResult.OK)
                {
                    say.text = editor.GetText();

                    tvDialogDetail.SelectedNode.Text = xmlCore.GetSafeString(say.text);


                    pgDialogElement.SelectedObject = tvDialogDetail.SelectedNode.Tag;

                    Refresh_WhenPropertyValueChanged(tvDialogDetail.SelectedNode);
                }
            }
        }

        /// <summary>
        /// 주어진 text, exit code로 이루어진 트리노드를 만든다. 이렇게 처리하는 이유는 Select문은 Exit코드 때문에 문자열을 별도로 생성해야 하기 때문
        /// </summary>
        /// <param name="text">본문의 문자열</param>
        /// <param name="nExit">EXIT 코드</param>
        /// <param name="bExit">EXIT 코드가 명시되었는지 여부(SHDialog의 exitSpecified를 그냥 패스하면 된다.)</param>
        /// <returns>생성된 트리노드를 반환한다.</returns>
        private TreeNode CreateTreeNodeFromSel(String text, Int32 nExit, Boolean bExit)
        {
            TreeNode node = new TreeNode();
            SetTreeNodeFromSel(node, text, nExit, bExit);
            return node;
        }

        /// <summary>
        /// SELECT문의 속성들을 기반으로 트리뷰용 문자열을 생성하여 트리노드에 셋팅한다.
        /// </summary>
        /// <param name="node">설정되어야 할 트리노드</param>
        /// <param name="text">본문의 문자열</param>
        /// <param name="nExit">EXIT 코드</param>
        /// <param name="bExit">EXIT 코드를 명시했는지 여부(SHDialog의 exitSpecified를 그냥 패스한다.)</param>
        public void SetTreeNodeFromSel(TreeNode node, String text, Int32 nExit, Boolean bExit)
        {
            node.Text = (bExit) ? xmlCore.GetSafeString(text) + " => " + nExit.ToString() : xmlCore.GetSafeString(text);
        }

        /// <summary>
        /// nodeParent하위에 파라미터로 지정된 SELECT문을 자동으로 붙인다.
        /// </summary>
        /// <param name="nodeParent">부모노드(SAY문이 되어야한다. 별도로 체크하지는 않는다.)</param>
        /// <param name="selectNode">생성된 SELECT 데이터</param>
        /// <returns>만들어진 노드를 반환</returns>
        private TreeNode ProcessSelect(TreeNode nodeParent, SHDialogSelect selectNode)
        {
            TreeNode nodeThis = CreateTreeNodeFromSel(selectNode.text, selectNode.exit, selectNode.exitSpecified);

            nodeThis.ImageIndex = 0;
            nodeThis.SelectedImageIndex = 0;
            nodeThis.Tag = selectNode;
            nodeThis.ForeColor = Color.CornflowerBlue;

            nodeParent.Nodes.Add(nodeThis);

            if (selectNode.Say != null) ProcessSay(nodeThis, selectNode.Say);

            if (selectNode.Item != null)
            {
                for (int i = 0; i < selectNode.Item.dataList.Count; i++)
                {
                    ProcessItem(nodeThis, (SHItemAddRemoveBase)selectNode.Item.dataList[i]);
                }
            }


            return nodeThis;
        }

        private void SetTreeNodeFromItem(TreeNode node, SHItemAddRemoveBase itemBase)
        {
            if (itemBase.GetType() == typeof(SHItemAdd))
            {
                node.Text = "[ITEM-ADD]";
            }
            else if (itemBase.GetType() == typeof(SHItemRemove))
            {
                node.Text = "[ITEM-REMOVE]";
            }
            else if (itemBase.GetType() == typeof(SHItemFill))
            {
                node.Text = "[ITEM-FILL]";
            }
            else
            {
                return;
            }

            node.Text = node.Text + " (ID: " + itemBase.id.ToString() + ") , (Amount: " + itemBase.amount.ToString() + ")";

            node.ImageIndex = 0;
            node.SelectedImageIndex = 0;
            node.Tag = itemBase;
            node.ForeColor = Color.DeepSkyBlue;
        }

        private TreeNode ProcessItem(TreeNode nodeParent, SHItemAddRemoveBase selectNode)
        {
            TreeNode nodeNew = new TreeNode();

            SetTreeNodeFromItem(nodeNew, selectNode);

            nodeParent.Nodes.Add(nodeNew);

            return nodeNew;
        }

        /// <summary>
        /// nodeParent하위에 파라미터로 지정된 SAY문을 자동으로 붙인다.
        /// </summary>
        /// <param name="nodeParent">부모노드(SELECT문이나 DIALOG문이 된다.)</param>
        /// <param name="sayNode">생성된 SAY 데이터</param>
        /// <returns>만들어진 SAY문의 노드데이터</returns>
        private TreeNode ProcessSay(TreeNode nodeParent, SHDialogSay sayNode)
        {
            TreeNode nodeThis = new TreeNode(xmlCore.GetSafeString(sayNode.text));

            nodeThis.ImageIndex = 5;
            nodeThis.SelectedImageIndex = 5;
            nodeThis.Tag = sayNode;

            nodeParent.Nodes.Add(nodeThis);

            foreach (SHDialogSelect sel in sayNode.dataList) ProcessSelect(nodeThis, sel);

            return nodeThis;
        }

        /// <summary>
        /// 다이얼로그 데이터를 기반으로 트리노드를 생성한다.
        /// </summary>
        /// <param name="dlg">SHDialog 데이터</param>
        /// <returns>트리뷰의 루트 노드</returns>
        private TreeNode ProcessDialog(SHDialog dlg)
        {
            if (dlg != null)
            {
                TreeNode nodeRoot = CreateTreeNodeFromSel(dlg.text, dlg.exit, dlg.exitSpecified);

                nodeRoot.ImageIndex = 4;
                nodeRoot.SelectedImageIndex = 4;
                nodeRoot.Tag = dlg;

                if (dlg.Say != null) ProcessSay(nodeRoot, dlg.Say);

                tvDialogDetail.SuspendLayout();

                tvDialogDetail.Nodes.Clear();
                tvDialogDetail.Nodes.Add(nodeRoot);
                tvDialogDetail.ExpandAll();
                tvDialogDetail.SelectedNode = nodeRoot;

                tvDialogDetail.ResumeLayout();

                return nodeRoot;
            }

            return null;
        }

        private void EditForm_Dialog_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);

            if (e.KeyCode == Keys.F3)
            {
                cbDialogFilter.Focus();
            }

        }


        private void EditForm_Dialog_Load(object sender, EventArgs e)
        {
            this.lvDialogs.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }


        private void AddSelect_Item(SHItemAddRemoveType nItemAddRemoveType)
        {
            if (tvDialogDetail.SelectedNode == null || tvDialogDetail.SelectedNode.Tag == null) return;

            Type nodeType = tvDialogDetail.SelectedNode.Tag.GetType();

            if (nodeType == typeof(SHDialogSelect))
            {
                SHDialogSelect dialogSelect = (SHDialogSelect)tvDialogDetail.SelectedNode.Tag;
                SHItemAddRemoveBase newItem = SHItemAddRemove.NewItem(nItemAddRemoveType);

                newItem.id = 0;
                newItem.amount = 0;

                if (dialogSelect.Item == null)
                {
                    dialogSelect.Item = new SHItemAddRemove();
                }
                dialogSelect.Item.dataList.Add(newItem);

                TreeNode newNode = ProcessItem(tvDialogDetail.SelectedNode, newItem);

                tvDialogDetail.SelectedNode = newNode;
            }

            Global._VelixianForms.FindForm("DIALOG").Touch();
        }

        private void subDialogDetailAddItemAdd_Click(object sender, EventArgs e)
        {
            AddSelect_Item(SHItemAddRemoveType.ADD);
        }

        private void subDialogDetailAddItemRemove_Click(object sender, EventArgs e)
        {
            AddSelect_Item(SHItemAddRemoveType.REMOVE);
        }

        private void subDialogDetailAddItemFill_Click(object sender, EventArgs e)
        {
            AddSelect_Item(SHItemAddRemoveType.FILL);
        }

        private void lvDialogs_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "Dialogs");
        }

        private void cbDialogFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }
    }
}
