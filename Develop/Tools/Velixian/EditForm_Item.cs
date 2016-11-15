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
    public partial class EditForm_Item : EditForm
    {
        internal SHXmlCore xmlCore;
        protected ItemListViewController m_ListViewController;


        //////////////////////////////////////////////////////////////////////////

        public class ItemListViewController : BaseListViewController
        {
            public ItemListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList,
                System.Windows.Forms.ListView listView, System.Windows.Forms.ComboBox cbFilter)
                : base(xmlCore, xmlDataObject, arrayList, listView, cbFilter)
            {

            }

            public override void SetListText(ListViewItem lvi, SHDataObject selDataObject)
            {
                SHItem item = (SHItem)(selDataObject);

                String szCond = "";

                if (lvi == null) return;
                if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

                item.Compile();
                item.Build(m_XmlCore);

                lvi.Text = item.id.ToString();
                szCond = (String)GetConditionString(item);

                lvi.SubItems.AddRange(new String[]{
                (item.name!=null)?m_XmlCore.GetSafeString(item.name):"",
                (item.desc!=null)?m_XmlCore.GetSafeString(item.desc):"",
                szCond,
                (item.sex != null) ? item.sex.ToUpper() : ""
            });

                lvi.Tag = item;
                if (!item.Passed) lvi.ImageIndex = 7;

                lvi.ForeColor = (item.Passed) ? Color.Black : Color.Red;
                lvi.BackColor = (item.Passed) ? Color.White : Color.Yellow;

            }

            private string GetConditionString(SHItem item)
            {
                string ret = "";

                if (item.condition != 0 && item.conditionSpecified)
                {
                    try
                    {
                        ret = m_XmlCore.ItemConditions[item.condition].GetString(m_XmlCore);
                    }
                    catch
                    {
                        ret = item.condition.ToString();
                    }
                }
                return ret;
            }

            protected override int GetDataObjectID(object listViewItemTag)
            {
                if (listViewItemTag.GetType() != typeof(SHItem)) return 0;

                SHItem selItem = (SHItem)listViewItemTag;

                if (selItem != null)
                {
                    return selItem.id;
                }
                return 0;
            }

            protected override bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
            {
                SHItem item = (SHItem)(dataObject);

                if (strHead.ToLower() == "id") return ExistFilterString(strFilter, item.id.ToString());
                if (strHead.ToLower() == "name" || strHead == "이름") return ExistFilterString(strFilter, item.name);
                if (strHead.ToLower() == "desc" || strHead == "설명") return ExistFilterString(strFilter, item.desc);
                if (strHead.ToLower() == "condition" || strHead == "조건" || strHead == "사용조건") return ExistFilterString(strFilter, GetConditionString(item));
                if (strHead.ToLower() == "sex" || strHead == "성별") return ExistFilterString(strFilter, (item.sex != null) ? item.sex.ToString() : "");

                if (ExistFilterString(strFilter, item.id.ToString())) return true;
                if (ExistFilterString(strFilter, item.name)) return true;
                if (ExistFilterString(strFilter, item.desc)) return true;
                if (ExistFilterString(strFilter, (item.sex != null) ? item.sex.ToString() : "")) return true;


                return false;
            }

        }

        public EditForm_Item()
        {
            InitializeComponent();
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.ITEM, lvItems, "ITEM", xmlCore.Items.LoadingFailed, null, null, null);
            }

            m_ListViewController = new ItemListViewController(xmlCore, xmlCore.Items, xmlCore.Items.dataList, lvItems, cbItemFilter);
            m_ListViewController.InitListView(null);

            SHListViewUtil.SelectIndex(lvItems, 0);

            
        }

        private void lvItems_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvItems.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvItems.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHItem))
                {
                    SHItem item = (SHItem)lvi.Tag;

                    if (item != null)
                    {
                        pgItemDetail.SelectedObject = item;
                        Global._mainForm.SetStatusLabelText(item.CompiledMessage);
                    }
                }
            }
            m_ListViewController.OnSelectedIndexChanged();
        }

        private void EditForm_Item_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);

            if (e.KeyCode == Keys.F3)
            {
                cbItemFilter.Focus();
            }

        }

        private void EditForm_Item_Load(object sender, EventArgs e)
        {
            this.lvItems.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }

        private void lvItems_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnKeyDown(e, "Items");
        }

        private void cbItemFilter_KeyDown(object sender, KeyEventArgs e)
        {
            m_ListViewController.OnFilterCombobox_KeyDown(ref e);
        }

    }
}
