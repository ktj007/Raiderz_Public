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
    public partial class SHCE_TextEditor : Form
    {
        public SHCE_TextEditor()
        {
            InitializeComponent();
        }

        public void SetText(String text)
        {
            if (text == null) text = "";
            SHTextUtil.SetTextToTextBox(tbEditor, text);
        }

        public String GetText()
        {
            return SHTextUtil.GetTextFromTextBox(tbEditor);
        }

        private void VelixianTextEditor_Load(object sender, EventArgs e)
        {
            for (int i = 0; i < SHXmlCore.CurrentCore.Strings.dataList.Count; i++)
            {
                SHString str = (SHString)SHXmlCore.CurrentCore.Strings.dataList[i];
                ListViewItem lvi = new ListViewItem(new string[] { str.Key, str.String });
                lvi.Tag = "$$" + str.Key;
                lvList.Items.Add(lvi);
            }
        }

        private void lvList_DoubleClick(object sender, EventArgs e)
        {
            if (lvList.SelectedItems.Count > 0 && lvList.SelectedItems[0].Tag != null && lvList.SelectedItems[0].Tag.GetType() == typeof(String))
            {
                SetText((String)lvList.SelectedItems[0].Tag);
            }
        }

        private void cbSearchText_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                int nSearchIndex = (lvList.SelectedIndices.Count > 0 && lvList.SelectedIndices[0] != lvList.Items.Count) ? lvList.SelectedIndices[0] + 1 : 0;

                ListViewItem lvi = lvList.FindItemWithText(cbSearchText.Text, (cbIDOnly.Checked != true), nSearchIndex, true);
                if (lvi == null) lvi = lvList.FindItemWithText(cbSearchText.Text, (cbIDOnly.Checked != true), 0, true);

                if (lvi != null)
                {
                    SHListViewUtil.SelectIndex(lvList, lvi.Index);                    
                    cbSearchText.AutoCompleteCustomSource.Add(cbSearchText.Text);
                }
            }
        }

        public string GetDialogHTML(string szText)
        {
            szText = SHWebUtil.ReplaceDialogTextForWeb(szText);

            szText = Properties.Resources.dialog_preview_element_template.Replace("[[DIALOG_TEXT]]", szText);
            szText = Properties.Resources.dialog_preview_template.Replace("[[DIALOG_TEXT_ELEMENT]]", szText);
            szText = Properties.Resources.xhtml_template.Replace("[[CONTENT_BODY]]", szText);

            return szText;
        }

        private void ReplaceEditorTextTag(string szTag)
        {
            if (tbEditor.SelectionLength > 0)
            {
                int nSelStart = tbEditor.SelectionStart;

                string szSelText = "{" + szTag + "}" + tbEditor.SelectedText + "{/" + szTag + "}";

                string szReplaceText = tbEditor.Text.Remove(tbEditor.SelectionStart, tbEditor.SelectionLength);
                tbEditor.Text = szReplaceText.Insert(nSelStart, szSelText);

                tbEditor.Select(nSelStart, szSelText.Length);
            }
        }

        private void tsbtnReplaceCharacterTag_Click(object sender, EventArgs e)
        {
            ReplaceEditorTextTag("character");
        }

        private void tsbtnReplaceItemTag_Click(object sender, EventArgs e)
        {
            ReplaceEditorTextTag("item");
        }

        private void tsbtnReplacePlaceTag_Click(object sender, EventArgs e)
        {
            ReplaceEditorTextTag("place");
        }

        private void SHCE_TextEditor_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F1)
            {
                ReplaceEditorTextTag("character");
            }
            if (e.KeyCode == Keys.F2)
            {
                ReplaceEditorTextTag("item");
            }
            if (e.KeyCode == Keys.F3)
            {
                ReplaceEditorTextTag("place");
            }
            if (e.KeyCode == Keys.F4)
            {
                tsbtnAniTag_Click(sender, e);
            }

        }

        private void tsbtnAniTag_Click(object sender, EventArgs e)
        {
            string szAniText = "{ani=}";

            string szReplaceText = tbEditor.Text;
            tbEditor.Text = szReplaceText.Insert(0, szAniText);

            tbEditor.Focus();
            tbEditor.Select(szAniText.Length - 1, 0);

        }

        private void SHCE_TextEditor_Shown(object sender, EventArgs e)
        {
            tbEditor.Focus();
        }

        private void tbEditor_TextChanged(object sender, EventArgs e)
        {
            SetViewTextFromEditor();
        }

        private void SetViewTextFromEditor()
        {
            wbViewText.DocumentText = GetDialogHTML(tbEditor.Text);

            string strPlaneText = SHWebUtil.RemoveTag(tbEditor.Text);

            lbTextLen.Text = "글자수 : " + Convert.ToString(strPlaneText.Length);
        }

    }
}
