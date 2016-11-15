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
    public class BaseListViewController
    {
        protected SHXmlCore m_XmlCore;
        protected SHXmlDataObject m_XmlDataObject;
        protected ArrayList m_ArrayList;
        protected System.Windows.Forms.ListView m_ListView;
        protected System.Windows.Forms.ComboBox m_cbFilter;

        protected int m_nCopyObjectID;

        public BaseListViewController(SHXmlCore xmlCore, SHXmlDataObject xmlDataObject, ArrayList arrayList, 
            System.Windows.Forms.ListView listView,
            System.Windows.Forms.ComboBox cbFilter)
        {
            m_XmlCore = xmlCore;
            m_XmlDataObject = xmlDataObject;
            m_ArrayList = arrayList;
            m_ListView = listView;
            m_cbFilter = cbFilter;

            m_nCopyObjectID = 0;
        }

        public void InitListView(SHDataObject selDataObject)
        {
            m_ListView.SuspendLayout();

            m_ListView.Items.Clear();

            foreach (SHDataObject dataObject in m_ArrayList)
            {
                ListViewItem lvi = new ListViewItem();
                SetListText(lvi, dataObject);
                m_ListView.Items.Add(lvi);

                if (selDataObject != null && selDataObject == dataObject)
                {
                    lvi.Focused = true;
                    lvi.Selected = true;
                }
            }

            m_ListView.ResumeLayout();

            Global._mainForm.SetListViewStatusLabelText(0, m_ListView.Items.Count);
            m_nCopyObjectID = 0;
        }

        public void SortListView(SHDataObject selDataObject)
        {
            m_XmlDataObject.Sort();

            InitListView(selDataObject);
        }

        public int MakeNewID()
        {
            int nSelLootID = 0;

            if (m_ListView.SelectedItems.Count > 0)
            {
                ListViewItem lvi = m_ListView.SelectedItems[0];
                if (lvi != null && lvi.Tag != null)
                {
                    nSelLootID = GetDataObjectID(lvi.Tag);
                }
            }

            do
            {
                ++nSelLootID;
            }
            while (m_XmlDataObject.IsValid(nSelLootID) == true);

            return nSelLootID;
        }

        protected virtual int GetDataObjectID(object listViewItemTag)
        {
            return 0;
        }

        public virtual void SetListText(ListViewItem lvi, SHDataObject selDataObject)      {}

        public void AddItem(SHDataObject newDataObject)
        {
            ListViewItem lviNew = new ListViewItem();
            SetListText(lviNew, newDataObject);

            m_ListView.Items.Add(lviNew);
            SHListViewUtil.SelectIndex(m_ListView, lviNew.Index);

            SortListView(newDataObject);

            if (m_ListView.SelectedItems.Count > 0)
                m_ListView.SelectedItems[0].EnsureVisible();

        }

        public object DeleteSelectedItem()
        {
            ListViewItem lvi = m_ListView.SelectedItems[0];

            if (lvi != null)
            {
                object tagObject = lvi.Tag;

                if (tagObject != null)
                {
                    int nSelected = lvi.Index;
                    m_ListView.Items.Remove(lvi);
                    SHListViewUtil.SelectIndex(m_ListView, nSelected);

                    return tagObject;
                }
            }
            return null;
        }
        public int GetSelectedItemDataObjectID()
        {
            if (m_ListView.SelectedItems.Count <= 0) return 0;

            ListViewItem lvi = m_ListView.SelectedItems[0];

            return GetDataObjectID(lvi.Tag);
        }
        public void OnKeyDown(KeyEventArgs e, string strDataName)
        {
            int nID = GetSelectedItemDataObjectID();
            if (nID == 0) return;

            if (e.Control && e.KeyCode == Keys.C)
            {
                Clipboard.SetText(String.Format("{0}", nID));
                Global.Output(String.Format("{0} ID 복사 : {1}\n", strDataName, nID));
            }
        }

        public void OnFilterCombobox_KeyDown(ref KeyEventArgs e)
        {
            if (m_cbFilter == null) return;

            if (e.KeyCode == Keys.Enter)
            {
                string strFilter = m_cbFilter.Text.Trim();

                if (strFilter.Length > 0) m_cbFilter.Items.Add(m_cbFilter.Text);

                InitListView_Filter(null, strFilter);

                e.SuppressKeyPress = true;
            }
        }
        
        public void InitListView_Filter(SHDataObject selDataObject, string strFilters)
        {
            m_ListView.SuspendLayout();

            m_ListView.Items.Clear();

            string strHead = "";
            SplitFilter(ref strHead, ref strFilters);

            foreach (SHDataObject dataObject in m_ArrayList)
            {

                int n = strFilters.IndexOf(";");
                if (n > 0)
                {
                    bool bCheck = false;
                    string[] split = strFilters.Split(new Char[] { ';' });
                    foreach (string strFilter in split)
                    {
                        if (strFilter.Length > 0 && CheckFilter(dataObject, strHead, strFilter) == true)
                        {
                            bCheck = true;
                            break;
                        }
                    }
                    if (bCheck == false) continue;
                }
                else
                {
                    if (strFilters.Length > 0 && CheckFilter(dataObject, strHead, strFilters) == false) continue;
                }


                ListViewItem lvi = new ListViewItem();
                SetListText(lvi, dataObject);
                m_ListView.Items.Add(lvi);

                if (selDataObject != null && selDataObject == dataObject)
                {
                    lvi.Focused = true;
                    lvi.Selected = true;
                }
            }

            m_ListView.ResumeLayout();

            Global._mainForm.SetListViewStatusLabelText(0, m_ListView.Items.Count);
            m_nCopyObjectID = 0;
        }

        private void SplitFilter(ref string strHead, ref string strFilter)
        {
            strHead = "";
            int n = strFilter.IndexOf(":");
            if (n > 0)
            {
                string[] split = strFilter.Split(new Char[] { ':' });
                if (split.Length >= 2)
                {
                    strHead = split[0];
                    strFilter = split[1];
                }
            }
        }

        protected bool ExistFilterString(string strFilter, string strString)
        {
            if (strString != null && strString.ToLower().IndexOf(strFilter.ToLower()) >= 0) return true;

            return false;
        }

        protected virtual bool CheckFilter(SHDataObject dataObject, string strHead, string strFilter)
        {
            return true;
        }

        public void OnSelectedIndexChanged()
        {
            if (m_ListView.SelectedItems.Count > 0)
            {
                ListViewItem lvi = m_ListView.SelectedItems[0];
                if (lvi != null && lvi.Tag != null)
                {
                    Global._mainForm.SetListViewStatusLabelText(lvi.Index + 1, m_ListView.Items.Count);
                }
            }
        }

        public void CopyDataObject()
        {
            if (m_ListView.SelectedItems.Count <= 0) return;

            ListViewItem lvi = m_ListView.SelectedItems[0];
            if (lvi == null || lvi.Tag == null)
            {
                return;
            }

            m_nCopyObjectID = GetDataObjectID(lvi.Tag);
        }
        public bool IsCopied()
        {
            return (m_nCopyObjectID != 0);
        }
        public void PasteDataObject()
        {
            if (m_nCopyObjectID <= 0) return;

            if (m_ListView.SelectedItems.Count <= 0) return;

            ListViewItem lvi = m_ListView.SelectedItems[0];
            if (lvi == null || lvi.Tag == null)
            {
                return;
            }

            SHDataObject tarDataObject = (SHDataObject)lvi.Tag;

            if (_PasteDataObject(tarDataObject) == false)
            {
                m_nCopyObjectID = 0;
                return;
            }

            SetListText(lvi, tarDataObject);
            SHListViewUtil.SelectIndex(m_ListView, lvi.Index);

            if (m_ListView.SelectedItems.Count > 0)
                m_ListView.SelectedItems[0].EnsureVisible();



        }
        protected virtual bool _PasteDataObject(SHDataObject tarDataObject)
        {
            return false;
        }
    }


    //////////////////////////////////////////////////////////////////////////



}