using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using CombatMonitor.XML;

namespace CombatMonitor.Util
{
    /// <summary>
    /// This class is an implementation of the 'IComparer' interface.
    /// </summary>
    public class ListViewColumnSorter : IComparer
    {
        /// <summary>
        /// Specifies the column to be sorted
        /// </summary>
        private int ColumnToSort;
        /// <summary>
        /// Specifies the order in which to sort (i.e. 'Ascending').
        /// </summary>
        private SortOrder OrderOfSort;
        /// <summary>
        /// Case insensitive comparer object
        /// </summary>
        private CaseInsensitiveComparer ObjectCompare;

        /// <summary>
        /// Class constructor.  Initializes various elements
        /// </summary>
        public ListViewColumnSorter()
        {
            // Initialize the column to '0'
            ColumnToSort = 0;

            // Initialize the sort order to 'none'
            OrderOfSort = SortOrder.None;

            // Initialize the CaseInsensitiveComparer object
            ObjectCompare = new CaseInsensitiveComparer();
        }

        /// <summary>
        /// This method is inherited from the IComparer interface.  It compares the two objects passed using a case insensitive comparison.
        /// </summary>
        /// <param name="x">First object to be compared</param>
        /// <param name="y">Second object to be compared</param>
        /// <returns>The result of the comparison. "0" if equal, negative if 'x' is less than 'y' and positive if 'x' is greater than 'y'</returns>
        public int Compare(object x, object y)
        {
            int compareResult;
            ListViewItem listviewX, listviewY;

            // Cast the objects to be compared to ListViewItem objects
            listviewX = (ListViewItem)x;
            listviewY = (ListViewItem)y;

            // Compare the two items
            string strItem1 = listviewX.SubItems[ColumnToSort].Text;
            string strItem2 = listviewY.SubItems[ColumnToSort].Text;

            double dummy;
            if (double.TryParse(strItem1, out dummy))
            {
                double fItem1 = Convert.ToDouble(strItem1);
                double fItem2 = Convert.ToDouble(strItem2);
                compareResult = ObjectCompare.Compare(fItem1, fItem2);
            }
            else
            {
                compareResult = ObjectCompare.Compare(strItem1, strItem2);
            }


            // Calculate correct return value based on object comparison
            if (OrderOfSort == SortOrder.Ascending)
            {
                // Ascending sort is selected, return normal result of compare operation
                return compareResult;
            }
            else if (OrderOfSort == SortOrder.Descending)
            {
                // Descending sort is selected, return negative result of compare operation
                return (-compareResult);
            }
            else
            {
                // Return '0' to indicate they are equal
                return 0;
            }
        }

        /// <summary>
        /// Gets or sets the number of the column to which to apply the sorting operation (Defaults to '0').
        /// </summary>
        public int SortColumn
        {
            set
            {
                ColumnToSort = value;
            }
            get
            {
                return ColumnToSort;
            }
        }

        /// <summary>
        /// Gets or sets the order of sorting to apply (for example, 'Ascending' or 'Descending').
        /// </summary>
        public SortOrder Order
        {
            set
            {
                OrderOfSort = value;
            }
            get
            {
                return OrderOfSort;
            }
        }
    }

    class ListView
    {
        public static void ReflectionStructToColumn<STRUCT_TYPE>(ref System.Windows.Forms.ListView listview) 
            where STRUCT_TYPE : new()
        {
            listview.Columns.Clear();
            Type t = (new STRUCT_TYPE()).GetType();
            System.Reflection.FieldInfo[] fields = t.GetFields();
            foreach (System.Reflection.FieldInfo field in fields)
                listview.Columns.Add(field.Name, 80, HorizontalAlignment.Right);
        }

        public static void ReflectionStructToItems<STRUCT_TYPE>(ref System.Windows.Forms.ListView listview, ref ArrayList dataList)
        {
            foreach (STRUCT_TYPE e in dataList)
            {
                Type t = e.GetType();
                System.Reflection.FieldInfo[] fields = t.GetFields();
                ListViewItem item = null;
                foreach (System.Reflection.FieldInfo field in fields)
                {
                    object value = field.GetValue(e);
                    string strText = value != null ? value.ToString() : "";

                    if (item == null)
                        item = new ListViewItem(strText);
                    else
                        item.SubItems.Add(strText);
                }
                listview.Items.Add(item);
            }
        }

        public static void ColumnClickToSort(ref System.Windows.Forms.ListView listview, ref ListViewColumnSorter sorter, ColumnClickEventArgs e)
        {
            if (e.Column == sorter.SortColumn)
            {
                // Reverse the current sort direction for this column.
                if (sorter.Order == SortOrder.Ascending)
                {
                    sorter.Order = SortOrder.Descending;
                }
                else
                {
                    sorter.Order = SortOrder.Ascending;
                }
            }
            else
            {
                // Set the column number that is to be sorted; default to ascending.
                sorter.SortColumn = e.Column;
                sorter.Order = SortOrder.Ascending;
            }

            listview.Sort();
        }
    }
}
