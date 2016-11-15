using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ServerMonitor
{
    class MonitoringLogger_ListBox : MonitoringLogger
    {
        private readonly int MAX_LOG_COUNT = 30;

        private ListBox m_refMonitoringLogListBox = null;

        public MonitoringLogger_ListBox(ListBox refMonitoringLogListBox)
            : base(LOGGER_TYPE.LIST_BOX)
        {
            m_refMonitoringLogListBox = refMonitoringLogListBox;
        }

        public override void Write(string strServerName, int nWorldID, int nServerID, string strLogMessage)
        {
            if (m_refMonitoringLogListBox.Items.Count > MAX_LOG_COUNT)
                m_refMonitoringLogListBox.Items.RemoveAt(0);

            string strCompletedLogString;
            strCompletedLogString = DateTime.Now.ToString("yyyy/MM/dd-HH:mm:ss");
            strCompletedLogString += " - ";
            strCompletedLogString += strServerName;
            strCompletedLogString += "(" + nWorldID.ToString();
            strCompletedLogString += ", ";
            strCompletedLogString += nServerID.ToString() + ")";
            strCompletedLogString += ", ";
            strCompletedLogString += strLogMessage;

            m_refMonitoringLogListBox.Items.Add(strCompletedLogString);
            m_refMonitoringLogListBox.SelectedIndex = m_refMonitoringLogListBox.Items.Count - 1;
        }

    }
}
