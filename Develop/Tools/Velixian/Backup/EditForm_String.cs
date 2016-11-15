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
    public partial class EditForm_String : EditForm
    {
        internal SHXmlCore xmlCore;

        public EditForm_String()
        {
            InitializeComponent();
        }

        public override void InitEditForm(MainForm mainForm)
        {
            if (xmlCore == null)
            {
                xmlCore = Global._xmlCore;
                mainForm.AddForm(this, ValixianFormType.STRING, lvStrings, "STRING", xmlCore.Strings.LoadingFailed, null, null, null);
            }

            lvStrings.Items.Clear();
            foreach (SHString str in xmlCore.Strings.dataList)
            {
                ListViewItem lvi = new ListViewItem();
                String_SetListText(lvi, str);
                lvStrings.Items.Add(lvi);
            }

            SHListViewUtil.SelectIndex(lvStrings, 0);


            

        }

        private void String_SetListText(ListViewItem lvi, SHString str)
        {
            if (lvi == null) return;
            if (lvi.SubItems.Count > 1) lvi.SubItems.Clear();

            str.Compile();
            str.Build(xmlCore);

            lvi.Text = str.Key;
            lvi.SubItems.AddRange(new String[]{
                str.String
            });

            lvi.Tag = str;
            if (!str.Passed) lvi.ImageIndex = 7;

            lvi.ForeColor = (str.Passed) ? Color.Black : Color.Red;
            lvi.BackColor = (str.Passed) ? Color.White : Color.Yellow;
        }

        private void lvStrings_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvStrings.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvStrings.SelectedItems[0];

                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHString))
                {
                    SHString str = (SHString)lvi.Tag;
                    if (str != null)
                    {
                        tbStringKey.Text = str.Key;
                        SHTextUtil.SetTextToTextBox(tbStringValue, str.String);
                    }
                }
            }
        }

        private void Key_Set(object sender, EventArgs e)
        {
            if (lvStrings.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvStrings.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHString))
                {
                    SHString str = (SHString)lvi.Tag;
                    if (str != null && str.Key != tbStringKey.Text)
                    {
                        if (xmlCore.Strings.IsValid(tbStringKey.Text) && xmlCore.Strings[tbStringKey.Text] != str)
                        {
                            MessageBox.Show("Duplicate Key Value.", Application.ProductName);

                            tbStringKey.Text = str.Key;
                            tbStringKey.SelectAll();
                            tbStringKey.Focus();

                            return;
                        }

                        str.Key = tbStringKey.Text;
                        String_SetListText(lvi, str);

                        //FindTab("STRING").Touch();
                    }
                }
            }
        }

        private void Value_Set(object sender, EventArgs e)
        {
            if (lvStrings.SelectedItems.Count > 0)
            {
                ListViewItem lvi = lvStrings.SelectedItems[0];
                if (lvi != null && lvi.Tag != null && lvi.Tag.GetType() == typeof(SHString))
                {
                    SHString str = (SHString)lvi.Tag;
                    if (str != null && str.String != tbStringValue.Text)
                    {
                        str.String = SHTextUtil.GetTextFromTextBox(tbStringValue);
                        String_SetListText(lvi, str);

                        //FindTab("STRING").Touch();
                    }
                }
            }
        }

        private void tbStringKey_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Key_Set(sender, e);
            }
        }

        private void EditForm_String_Load(object sender, EventArgs e)
        {
            this.lvStrings.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(Global._mainForm.ColumnClickHandler);
        }

        public System.Windows.Forms.ListView GetStringListView() { return lvStrings;  }

        private void EditForm_String_KeyDown(object sender, KeyEventArgs e)
        {
            OnEditFormKeyDown(sender, e);
        }
    }
}
