using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor;

namespace ServerMonitor_unittest.Mock
{
    public class MockProcessSystem : ProcessSystem
    {
        private readonly int TEST_PROCESS_ID = 1000;

        private bool m_bCallStartProcess = false;
        private bool m_bCallTerminateProcess = false;

        public bool CallStartProcess { get { return m_bCallStartProcess; } }
        public bool CallTerminateProcess { get { return m_bCallTerminateProcess; } }

        public override bool StartProcess(string strProcessExePath, out int nProcessID)
        {
            nProcessID = TEST_PROCESS_ID;

            if (strProcessExePath.Length == 0) return false;

            m_bCallStartProcess = true;
            return true;
        }

        public override bool SendDestroyMessage(int nProcessID)
        {
            if (nProcessID == 0) return false;

            m_bCallTerminateProcess = true;
            return true;
        }

        public override bool PostSignal_Terminate(ServerInfo refServerInfo)
        {
            if (null == refServerInfo) return false;

            m_bCallTerminateProcess = true;
            return true;
        }

        public override bool IsRunningProcess(string strProcessName, int nProcessID)
        {
            if (strProcessName.Length == 0 || nProcessID == 0) return false;
            return true;
        }
    }
}
