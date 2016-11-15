using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AIMonitor.View.Controls
{
    class DoubleBufferedListView : ListView
    {
        public DoubleBufferedListView()
        {
            this.SetStyle(ControlStyles.AllPaintingInWmPaint |
               //ControlStyles.UserPaint |
               ControlStyles.OptimizedDoubleBuffer, true);

            UpdateStyles();
        }
    }
}
