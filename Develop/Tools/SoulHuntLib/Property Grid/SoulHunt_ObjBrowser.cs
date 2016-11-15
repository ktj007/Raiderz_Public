using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SoulHunt;

namespace SoulHunt
{
    public partial class SHCE_ObjBrowser : Form
    {
        public SHCE_ObjBrowser()
        {
            DataType = VOBType.String;
            InitializeComponent();
        }

        private VOBType DataType;
        private String SelectedTag;

        private void VelixianObjBrowser_Load(object sender, EventArgs e)
        {
            ListViewItem lviSelected = null;

            switch (DataType)
            {
                case VOBType.Quest:
                    for (int i = 0; i < SHXmlCore.CurrentCore.Quests.dataList.Count; i++)
                    {
                        SHQuest quest = (SHQuest)SHXmlCore.CurrentCore.Talents.dataList[i];
                        ListViewItem lvi = new ListViewItem(new string[] { quest.id.ToString(), quest.title });
                        if (SelectedTag == quest.id.ToString()) lviSelected = lvi;
                        lvi.Tag = quest.title.ToString();
                        lvList.Items.Add(lvi);
                    }
                    break;
                case VOBType.Item:
                    for (int i = 0; i < SHXmlCore.CurrentCore.Items.dataList.Count; i++)
                    {
                        SHItem item = (SHItem)SHXmlCore.CurrentCore.Items.dataList[i];
                        ListViewItem lvi = new ListViewItem(new string[] { item.id.ToString(), item.name });
                        if (SelectedTag == item.id.ToString()) lviSelected = lvi;
                        lvi.Tag = item.id.ToString();
                        lvList.Items.Add(lvi);
                    }
                    break;
                case VOBType.Talent:
                    for (int i = 0; i < SHXmlCore.CurrentCore.Talents.dataList.Count; i++)
                    {
                        SHTalent talent = (SHTalent)SHXmlCore.CurrentCore.Talents.dataList[i];
                        ListViewItem lvi = new ListViewItem(new string[] { talent.id.ToString(), talent.name });
                        if (SelectedTag == talent.id.ToString()) lviSelected = lvi;
                        lvi.Tag = talent.id.ToString();
                        lvList.Items.Add(lvi);
                    }
                    break;
                case VOBType.String:
                    for (int i = 0; i < SHXmlCore.CurrentCore.Strings.dataList.Count; i++)
                    {
                        SHString str = (SHString)SHXmlCore.CurrentCore.Strings.dataList[i];
                        ListViewItem lvi = new ListViewItem(new string[] { str.Key, str.String });
                        if (SelectedTag == str.Key) lviSelected = lvi;
                        lvi.Tag = str.Key;
                        lvList.Items.Add(lvi);
                    }
                    break;
            }

            if (lviSelected != null) SHListViewUtil.SelectIndex(lvList, lviSelected.Index); else SHListViewUtil.SelectIndex(lvList, 0);

            lvList.Select();
        }

        private void VelixianObjBrowser_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (DialogResult != DialogResult.OK)
            {
                SelectedTag = null;
            }
        }

        public void SetDataType(VOBType type)
        {
            DataType = type;
        }

        public void SetTag(String str)
        {
            if(str != null) SelectedTag = str;
        }

        public bool GetTag(out Int32 Ref)
        {
            if (!Int32.TryParse(SelectedTag, out Ref))
            {
                Ref = 0;
                return false;
            }
            else
            {
                return true;
            }
        }

        public String GetTag()
        {
            return SelectedTag;
        }

        private void lvList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvList.SelectedItems.Count > 0 && lvList.SelectedItems[0].Tag.GetType() == typeof(String)) SelectedTag = (String)lvList.SelectedItems[0].Tag;
        }

        private void lvList_DoubleClick(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }

        private void cbSearch_TextUpdate(object sender, EventArgs e)
        {
            ListViewItem lvi = lvList.FindItemWithText(cbSearch.Text, !ckIDOnly.Checked, (lvList.SelectedItems.Count > 0)?lvList.SelectedItems[0].Index:0);

            if (lvi != null) SHListViewUtil.SelectIndex(lvList, lvi.Index);
        }
    }

    public enum VOBType
    {
        NPC,
        Quest,
        Item,
        Condition,
        ItemCondition,
        Dialog,
        String,
        Talent,
        Looting
    }
}
