using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerMonitorClient
{
    public enum MACHINE_STATE
    {
        OFFLINE,
        ONLINE
    }

    public class MachineInfo
    {
        private ServerInfoMgr m_serverInfoMgr = new ServerInfoMgr();

        public UInt64 m_nSessionID = 0;

        public string MachineName;
        public bool IsRequestConnect = false;
        public bool IsConnected = false;
        public string IP;
        public int Port = 0;
        public MACHINE_STATE eMachineState = MACHINE_STATE.OFFLINE;

        public ServerInfoMgr ServerInfoMgr { get { return m_serverInfoMgr; } }
        public bool IsConnectable { get { return (IsConnected == false && IsRequestConnect == false && m_nSessionID == 0); } }

        public string GetName() { return MachineName; }
        public UInt64 GetSessionID() { return m_nSessionID; }
        public MACHINE_STATE GetState() { return eMachineState; }

        public void SetRequestConnect(UInt64 nSessionID)
        {
            m_nSessionID = nSessionID;
            IsRequestConnect = true;
        }
        public void SetConnected()
        {
            IsRequestConnect = false;
            IsConnected = true;
            eMachineState = MACHINE_STATE.ONLINE;
        }
        public void SetDisconnected()
        {
            m_serverInfoMgr.Clear();

            IsConnected = false;
            IsRequestConnect = false;
            m_nSessionID = 0;
            eMachineState = MACHINE_STATE.OFFLINE;
        }

    }
}
