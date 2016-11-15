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
    public partial class EditForm_Looting : EditForm
    {
        internal SHXmlCore xmlCore;
        protected LootListViewController m_ListViewController;


        //////////////////////////////////////////////////////////////////////////

        public class LootListViewController : BaseListViewController
        {
            public LootListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
                System.Windows.Forms.ListView listView, System.Windows.Forms.ComboBox cbFilter)
                : base(xmlCore, xmlDataObject, arrayList, listView, cbFilter)
            {

            }

            public override void SetListText(ListViewItem lvi, SHDataObject selDataObject)
            {
                SHLoot loot = (SHLoot)(selDataObject);

                if (lvi == null) return;
                if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

                loot.Compile();
                loot.Build(m_XmlCore);

                lvi.Text = (loot.idSpecified) ? loot.id.ToString() : String.Empty;

                lvi.SubItems.AddRange(new String[] { 
                (loot.comment != null) ? loot.comment : String.Empty,
                (loot.maxSpecified) ? loot.max.ToString() : String.Empty,
                (loot.dataList.Count > 0) ? loot.dataList.Count.ToString() + "개" : "0개"
            });

                if (!loot.Passed) lvi.ImageIndex = 7;
                lvi.Tag = loot;
                lvi.ForeColor = (loot.Passed) ? Color.Black : Color.Red;
                lvi.BackColor = (loot.Passed) ? Color.White : Color.Yellow;
            }

            protected override int GetDataObjectID(object listViewItemTag)
            {
                if (listViewItemTag.GetType() != typeof(SHLoot)) return 0;

                SHLoot selLoot = (SHLoot)listViewItemTag;

                if (selLoot != null)
                {
                    return selLoot.id;
                }
                return 0;
            }

            protected override bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
            {
                SHLoot loot = (SHLoot)(dataObject);

                int nDataListCount = (loot.dataList != null) ? loot.dataList.Count : 0;

                if (strHead.ToLower() == "id") return ExistFilterString(strFilter, loot.id.ToString());
                if (strHead.ToLower() == "comment" || strHead == "주석") return ExistFilterString(strFilter, loot.comment);
                if (strHead.ToLower() == "max" || strHead == "최대갯수") return ExistFilterString(strFilter, loot.max.ToString());
                if (strHead == "종류") return ExistFilterString(strFilter, nDataListCount.ToString());


                if (ExistFilterString(strFilter, loot.id.ToString())) return true;
                if (ExistFilterString(strFilter, loot.comment)) return true;
                if (ExistFilterString(strFilter, loot.max.ToString())) return true;


                return false;
            }

        }

        //////////////////////////////////////////////////////////////////////////

        public EditForm_Looting()
        {
            InitializeComponent();
            m_ListViewController = null;
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.LOOTING, lvLooting, "LOOTING", xmlCore.Lootings.LoadingFailed, null, null, xmlCore.SaveLootings);
            }

            m_ListViewController = new LootListViewController(xmlCore, xmlCore.Lootings, xmlCore.Lootings.dataList, lvLooting, cbLootFilter);
            m_ListViewController.InitListView(null);

            SHListViewUtil.SelectIndex(lvLooting, 0);
        }

        private void ProcessLootSilver(ListViewItem lvi, SHLootSilver silver)
        {
            if (lvi == null || silver == null) return;

            lvi.SubItems.Clear();

            lvi.Text = "Silver";
            lvi.SubItems.AddRange(new String[] { silver.rate.ToString() + "%", (silver.minSpecified) ? silver.min.ToString() : "(Undefined)", (silver.maxSpecified) ? silver.max.ToString() : "(Undefined)" });
            lvi.Tag = silver;
            lvi.ImageIndex = 9;
            lvi.ForeColor = (silver.Passed) ? Color.Black : Color.Red;
            lvi.BackColor = (silver.Passed) ? Color.White : Color.Yellow;
        }

        private void ProcessLootItem(ListViewItem lvi, SHLootItem item)
        {
            if (lvi == null || item == null) return;

            lvi.SubItems.Clear();

            lvi.Text = (xmlCore.Items.IsValid(item.id) && xmlCore.Items[item.id].Passed) ? xmlCore.GetSafeString(xmlCore.Items[item.id].name) : "Item does not exist (ID:" + item.id.ToString() + ")";
            lvi.SubItems.AddRange(new String[] { item.rate.ToString() + "%", (item.minSpecified) ? item.min.ToString() : "(Undefined)", (item.maxSpecified) ? item.max.ToString() : "(Undefined)" });
            lvi.Tag = item;
            lvi.ImageIndex = 10;
            lvi.ForeColor = (item.Passed) ? Color.Black : Color.Red;
            lvi.BackColor = (item.Passed) ? Color.White : Color.Yellow;

            if (item.Passed && (!xmlCore.Items.IsValid(item.id) || !xmlCore.Items[item.id].Passed)) lvi.ForeColor = Color.Gray;
        }

        private void ProcessLootSoul(ListViewItem lvi, SHLootSoul soul)
        {
            if (lvi == null || soul == null) return;

            lvi.SubItems.Clear();

            lvi.Text = (soul.typeSpecified) ? "Soul " + soul.type.ToString() + "Rank" : "(Undefined)";
            lvi.SubItems.AddRange(new String[] { soul.rate.ToString() + "%", (soul.minSpecified) ? soul.min.ToString() : "(Undefined)", (soul.maxSpecified) ? soul.max.ToString() : "(Undefined)" });
            lvi.Tag = soul;
            lvi.ImageIndex = 11;
            lvi.ForeColor = (soul.Passed) ? Color.Black : Color.Red;
            lvi.BackColor = (soul.Passed) ? Color.White : Color.Yellow;
        }

        private void ProcessLoot(SHLoot loot, ListView lvTarget)
        {
            if (loot == null) return;

            int nSelectedIndex = (lvTarget.SelectedItems.Count > 0) ? lvTarget.SelectedIndices[0] : 0;

            lvTarget.Items.Clear();

            for (int i = 0; i < loot.dataList.Count; i++)
            {
                ListViewItem lvi = new ListViewItem();

                if (loot.dataList[i].GetType() == typeof(SHLootSilver))
                {
                    ProcessLootSilver(lvi, loot.dataList[i] as SHLootSilver);
                }
                else if (loot.dataList[i].GetType() == typeof(SHLootItem))
                {
                    ProcessLootItem(lvi, loot.dataList[i] as SHLootItem);
                }
                else if (loot.dataList[i].GetType() == typeof(SHLootSoul))
                {
                    ProcessLootSoul(lvi, loot.dataList[i] as SHLootSoul);
                }

                lvTarget.Items.Add(lvi);
            }
            lvTarget.Tag = loot;
        }

        private void subLootAddSilver_Click(object sender, EventArgs e)
        {
            SHLootSilver silver = new SHLootSilver();

            silver.min = 1;
            silver.max = 2;
            silver.rate = 50;

            if (lvLootList.Tag != null && lvLootList.Tag.GetType() == typeof(SHLoot))
            {
                silver.Compile();
                silver.Build(xmlCore);

                ((SHLoot)lvLootList.Tag).Add(silver);

                ListViewItem lvi = new ListViewItem();

                ProcessLootSilver(lvi, silver);

                lvLootList.Items.Add(lvi);

                SHListViewUtil.SelectIndex(lvLootList, lvi.Index);

                m_ListViewController.SetListText(pgLoot.Tag as ListViewItem, lvLootList.Tag as SHLoot);
            }

            Global._VelixianForms.FindForm("LOOTING").Touch();
        }

        private void subLootAddItem_Click(object sender, EventArgs e)
        {
            SHLootItem item = new SHLootItem();

            item.min = 0;
            item.max = 1;
            item.rate = 20;
            item.id = 0;

            if (lvLootList.Tag != null && lvLootList.Tag.GetType() == typeof(SHLoot))
            {
                item.Compile();
                item.Build(xmlCore);

                ((SHLoot)lvLootList.Tag).Add(item);

                ListViewItem lvi = new ListViewItem();

                ProcessLootItem(lvi, item);

                lvLootList.Items.Add(lvi);
                SHListViewUtil.SelectIndex(lvLootList, lvi.Index);

                m_ListViewController.SetListText(pgLoot.Tag as ListViewItem, lvLootList.Tag as SHLoot);
            }

            Global._VelixianForms.FindForm("LOOTING").Touch();
        }

        private void subLootAddSoul_Click(object sender, EventArgs e)
        {
            SHLootSoul soul = new SHLootSoul();

            soul.min = 0;
            soul.max = 1;
            soul.rate = 10;
            soul.type = SHLootSoulType.C;

            if (lvLootList.Tag != null && lvLootList.Tag.GetType() == typeof(SHLoot))
            {
                soul.Compile();
                soul.Build(xmlCore);

                ((SHLoot)lvLootList.Tag).Add(soul);

                ListViewItem lvi = new ListViewItem();

                ProcessLootSoul(lvi, soul);

                lvLootList.Items.Add(lvi);
                SHListViewUtil.SelectIndex(lvLootList, lvi.Index);

                m_ListViewController.SetListText(pgLoot.Tag as ListViewItem, lvLootList.Tag as SHLoot);
            }

            Global._VelixianForms.FindForm("LOOTING").Touch();
        }

        private void subLootDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                ListViewItem lvi = lvLootList.SelectedItems[0];
                int nSelected = lvi.Index;

                if (lvi != null && lvi.Tag != null && ((lvi.Tag.GetType() == typeof(SHLootSoul) || lvi.Tag.GetType() == typeof(SHLootSilver) || lvi.Tag.GetType() == typeof(SHLootItem))))
                {
                    SHLoot loot = lvLootList.Tag as SHLoot;
                    if (loot != null)
                    {
                        loot.dataList.Remove(lvi.Tag);

                        m_ListViewController.SetListText(pgLoot.Tag as ListViewItem, loot);

                        ProcessLoot(loot, lvLootList);

                        lvLootList.SelectedIndices.Clear();

                        SHListViewUtil.SelectIndex(lvLootList, nSelected);

                        Global._VelixianForms.FindForm("LOOTING").Touch();
                    }
                }
            }
        }

        private void subLooting_Opening(object sender, CancelEventArgs e)
        {
            subLootDel.Enabled = (lvLootList.SelectedItems.Count > 0);
        }

        private void subLootings_Opening(object sender, CancelEventArgs e)
        {
            subLootingsDel.Enabled = (lvLooting.SelectedItems.Count > 0);
        }


        private void subLootingsAdd_Click(object sender, EventArgs e)
        {
            SHLoot newLoot = new SHLoot();

            int nNewLootID = m_ListViewController.MakeNewID();

            newLoot.id = nNewLootID;
            newLoot.max = 1;

            xmlCore.Lootings.Add(newLoot);

            m_ListViewController.AddItem(newLoot);

            Global._VelixianForms.FindForm("LOOTING").Touch();
        }

        private void subLootingsDel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to delete?", Application.ProductName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                object tagObject = m_ListViewController.DeleteSelectedItem();
                if (tagObject != null)
                {
                    xmlCore.Lootings.dataList.Remove(tagObject);
                    Global._VelixianForms.FindForm("LOOTING").Touch();
                }

            }
        }



        private void pgLoot_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (pgLoot.Tag.GetType() == typeof(ListViewItem))
            {
                SHLoot loot = (SHLoot)(pgLoot.SelectedObject as SHLoot);
                if (loot != null)
                {
                    if (e.ChangedItem.Label == "(Username)" && loot.id != (int)e.OldValue)
                    {
                        m_ListViewController.SortListView(loot);
                    }
                }

                m_ListViewController.SetListText(pgLoot.Tag as ListViewItem, pgLoot.SelectedObject as SHLoot);
            }

            Global._VelixianForms.FindForm("LOOTING").Touch();
        }

        private void pgLootableDetail_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (pgLootableDetail.Tag != null)
            {
                ListViewItem lvi = pgLootableDetail.Tag as ListViewItem;
                LootableDetail_ValueChanged(lvi);
            }
        }

        private void LootableDetail_ValueChanged(ListViewItem lvi)
        {
            if (lvi != null)
            {
                if (lvi.Tag.GetType() == typeof(SHLootSilver))
                {
                    SHLootSilver silver = (SHLootSilver)lvi.Tag;

                    silver.Compile();
                    silver.Build(xmlCore);

                    ProcessLootSilver(lvi, silver);
                }
                else if (lvi.Tag.GetType() == typeof(SHLootItem))
                {
                    SHLootItem item = (SHLootItem)lvi.Tag;

                    item.Compile();
                    item.Build(xmlCore);

                    ProcessLootItem(lvi, item);
                }
                else if (lvi.Tag.GetType() == typeof(SHLootSoul))
                {
                    SHLootSoul soul = (SHLootSoul)lvi.Tag;

                    soul.Compile();
                    soul.Build(xmlCore);

                    ProcessLootSoul(lvi, soul);
                }
            }

            Global._VelixianForms.FindForm("LOOTING").Touch();
        }

        private void lvLooting_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvLooting.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvLooting.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHLoot))
                {
                    SHLoot loot = (SHLoot)lvi.Tag;

                    if (loot != null)
                    {
                        pgLoot.SelectedObject = loot;
                        pgLoot.Tag = lvi;

                        ProcessLoot(loot, lvLootList);

                        SHListViewUtil.SelectIndex(lvLootList, 0);

                        if (lvLootList.Items.Count == 0)
                        {
                            pgLootableDetail.SelectedObject = null;
                            pgLootableDetail.Tag = null;
                        }

                        Global._mainForm.SetStatusLabelText(loot.CompiledMessage);
                    }
                }
            }
            m_ListViewController.OnSelectedIndexChanged();
        }

        private void lvLootList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvLootList.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvLootList.SelectedItems[0];
                if (lvi != null && lvi.Tag != null)
                {
                    pgLootableDetail.SelectedObject = lvi.Tag;
                    pgLootableDetail.Tag = lvi;
                }
            }
        }

        private void EditForm_Looting_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);

            if (e.KeyCode == Keys.F3)
            {
                cbLootFilter.Focus();
            }

        }

        private void EditForm_Looting_Load(object sender, EventArgs e)
        {
            this.lvLooting.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
            this.lvLootList.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }

        private void lvLootList_DoubleClick(object sender, EventArgs e)
        {
            pgLootableDetail.Focus();

            SHPropertyGridUtil.SelectGridItem(pgLootableDetail, "(Probability)");

            /*
            if (lvLootList.SelectedItems.Count <= 0) return;

            ListViewItem lvi = lvLootList.SelectedItems[0];
            if (lvi == null || lvi.Tag == null) return;

            SHLootable lootable = (SHLootable)lvi.Tag;

            string strDesc = "[" + lvi.Text + "]의 확률:";

            EditDialogBox.EditDialogBoxResult result = EditDialogBox.ShowDialog("전리품 확률 변경", strDesc, (int)lootable.rate);

            if (result.DialogResult == DialogResult.OK)
            {
                lootable.rate = result.ValueInt;

                LootableDetail_ValueChanged(lvi);

                pgLootableDetail.SelectedObject = lootable;
                pgLootableDetail.Tag = lvi;

            }
            */
        }

        private void lvLootList_KeyDown(object sender, KeyEventArgs e)
        {
        }

        private void lvLooting_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "Loot");
        }

        private void cbLootFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }


    }
}
