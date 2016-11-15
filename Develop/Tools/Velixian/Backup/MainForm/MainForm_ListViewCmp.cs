using System;
using System.Windows.Forms;
using System.Collections;

namespace Velixian
{
    public partial class MainForm : Form
    {
        public void ColumnClickHandler(object sender, ColumnClickEventArgs e)
        {
            if (Cursor == Cursors.WaitCursor) return; 
            ListView lvSender = sender as ListView;

            if (lvSender != null)
            {
                lvSender.SuspendLayout();

                Cursor csOld = Cursor;
                Cursor = Cursors.WaitCursor;

                bool bDigitColumn = false;

                if ((lvSender.Name != "lvStrings" && e.Column == 0) || (lvSender.Name == "lvNPCs" && e.Column == 3) || (lvSender.Name == "lvLootList" && e.Column != 3) || (lvSender.Name == "lvLootings" && e.Column != 1))
                {
                    bDigitColumn = true;
                }

                if (lvSender.Sorting == SortOrder.Ascending || lvSender.Sorting == SortOrder.None)
                {
                    lvSender.ListViewItemSorter = new MainForm_ListViewComparer(e.Column, false, bDigitColumn);
                    lvSender.Sorting = SortOrder.Descending;
                }
                else
                {
                    lvSender.ListViewItemSorter = new MainForm_ListViewComparer(e.Column, true, bDigitColumn);
                    lvSender.Sorting = SortOrder.Ascending;
                }

                Cursor = csOld;

                lvSender.ResumeLayout();
            }
        }
    }

    // 리스트뷰의 정렬을 위하여 사용함.
    class MainForm_ListViewComparer : IComparer
    {
        private int _nColumn = 0;
        private bool _bDesc = false;
        private bool _bDigit = false;

        public MainForm_ListViewComparer(int nColumn, bool bDesc, bool bDigit)
        {
            _nColumn = nColumn;
            _bDesc = bDesc;
            _bDigit = bDigit;
        }

        public int Compare(object x, object y)
        {
            try
            {
                if (!_bDigit)
                {
                    if (!_bDesc)
                    {
                        return String.Compare(((ListViewItem)x).SubItems[_nColumn].Text, ((ListViewItem)y).SubItems[_nColumn].Text);
                    }
                    else
                    {
                        return String.Compare(((ListViewItem)y).SubItems[_nColumn].Text, ((ListViewItem)x).SubItems[_nColumn].Text);
                    }
                }
                else
                {
                    try
                    {
                        int nX = Int32.Parse(((ListViewItem)x).SubItems[_nColumn].Text);
                        int nY = Int32.Parse(((ListViewItem)y).SubItems[_nColumn].Text);

                        if (!_bDesc)
                        {
                            return -1 * (nX - nY);
                        }
                        else
                        {
                            return (nX - nY);
                        }
                    }
                    catch (FormatException)
                    {
                        return 0;
                    }
                }
            }
            catch
            {
                return 0;
            }
        }
    }
}