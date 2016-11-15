using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;
using System.Threading;

namespace ServerMonitor
{
    public class ServerInfo
    {
        private SERVER_TYPE m_eType;
        private string m_strExePath = "";
        private string m_strProcessName = "";
        private int m_nProcessID = 0;

        private bool m_bIsRun = false;
        private bool m_bIsServable = false;
        private int m_nCurPlayerCount = 0;

        private string m_strServerName = "";
        private int m_nWorldID = 0;
        private int m_nServerID = 0;

        private PROCESS_STATE m_eProcessState = PROCESS_STATE.TERMINATED;
        private bool m_bIsRevivable = false;

        private SERVABLE_STATE m_eServableState = SERVABLE_STATE.INACTIVE;
        private DateTime m_timeLatestStart = DateTime.MinValue;

        private Semaphore m_smpEvent_Terminate = null;


        public SERVER_TYPE Type { get { return m_eType; } }
        public string ExePath { get { return m_strExePath; } }
        public string ProcessName { get { return m_strProcessName; } }
        public int ProcessID { get { return m_nProcessID; } }

        public bool IsRun { get { return m_bIsRun; } }
        public bool IsServable { get { return m_bIsServable; } }
        public int CurPlayerCount { get { return m_nCurPlayerCount; } }
        public string ServerName { get { return m_strServerName; } }
        public int WorldID { get { return m_nWorldID; } }
        public int ServerID { get { return m_nServerID; } }
        public PROCESS_STATE ProcessState { get { return m_eProcessState; } }
        public bool IsRevivable { get { return m_bIsRevivable; } }
        public SERVABLE_STATE ServableState { get { return m_eServableState; } }
        public DateTime LatestStartTime { get { return m_timeLatestStart; } }

        public Semaphore Event_Terminate { get { return m_smpEvent_Terminate; } }


        public ServerInfo(SERVER_TYPE eType, string strExePath, string strProcessName, string strServerName, int nWorldID, int nServerID)
        {
            m_eType = eType;
            m_strExePath = strExePath;
            m_strProcessName = strProcessName;
            m_strServerName = strServerName;
            m_nWorldID = nWorldID;
            m_nServerID = nServerID;

            m_smpEvent_Terminate = new Semaphore(1, 1, m_strServerName);
        }

        public void SetServerID(int nWorldID, int nServerID)
        {
            m_nWorldID = nWorldID;
            m_nServerID = nServerID;
        }
        public void SetType(SERVER_TYPE eType)
        {
            m_eType = eType;
        }
        public void SetDBInfo(bool bIsRun, bool bIsServable, int nPlayerCount)
        {
            m_bIsRun = bIsRun;
            m_bIsServable = bIsServable;
            m_nCurPlayerCount = nPlayerCount;
        }

        public void SetProcessStarting(int nProcessID)
        {
            m_eProcessState = PROCESS_STATE.STARTING;
            m_nProcessID = nProcessID;
            m_timeLatestStart = DateTime.Now;

            ResetEventSignal();
        }
        public void SetProcessRunning()
        {
            m_eProcessState = PROCESS_STATE.RUNNING;
        }
        public void SetProcessRevivable(bool bIs)
        {
            m_bIsRevivable = bIs;
        }
        public void SetProcessDied()
        {
            m_eProcessState = PROCESS_STATE.DIED;
            m_eServableState = SERVABLE_STATE.INACTIVE;
            m_nProcessID = 0;
            m_nCurPlayerCount = 0;
        }
        public void SetProcessTerminating()
        {
            m_eProcessState = PROCESS_STATE.TERMINATING;
            m_eServableState = SERVABLE_STATE.INACTIVE;
            m_nCurPlayerCount = 0;
        }
        public void SetProcessTerminated()
        {
            m_eProcessState = PROCESS_STATE.TERMINATED;
            m_eServableState = SERVABLE_STATE.INACTIVE;
            m_nProcessID = 0;
            m_nCurPlayerCount = 0;

            ResetEventSignal();
        }
        public void SetProcessNoUpdate()
        {
            m_eProcessState = PROCESS_STATE.NOUPDATE;
            m_eServableState = SERVABLE_STATE.INACTIVE;
        }
        public void SetServableState(SERVABLE_STATE eState)
        {
            m_eServableState = eState;
        }


        public void SetEventSignal_Terminate()
        {
            m_smpEvent_Terminate.Release(1);
        }
        private void ResetEventSignal()
        {
            m_smpEvent_Terminate.WaitOne(1);
        }

    }
}
