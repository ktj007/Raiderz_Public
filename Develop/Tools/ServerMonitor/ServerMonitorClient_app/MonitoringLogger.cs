using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ServerMonitorClient
{
    public class MonitoringLogger
    {
        private readonly int MAX_LOG_COUNT = 50;

        private ListBox m_refLogListBox = null;

        public MonitoringLogger(ListBox refLogListBox)
        {
            m_refLogListBox = refLogListBox;
        }

        public void AddLog(string strLog)
        {
            if (m_refLogListBox.Items.Count > MAX_LOG_COUNT)
                m_refLogListBox.Items.RemoveAt(0);

            m_refLogListBox.Items.Add(strLog);
            m_refLogListBox.SelectedIndex = m_refLogListBox.Items.Count - 1;
        }

        public void Clear()
        {
            m_refLogListBox.Items.Clear();
        }

    }
}
