using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public class ServerInfo
    {
        private string ServerName;
        private SERVER_TYPE m_eServerType = SERVER_TYPE.SERVER_LOGIN;
        private PROCESS_STATE m_eProcessState = PROCESS_STATE.TERMINATED;
        private SERVABLE_STATE m_eServableState = SERVABLE_STATE.INACTIVE;
        
        public bool bRevivable = false;
        public int WorldID = 0;
        public int ServerID = 0;
        public int CurPlayerCount = 0;


        public void SetName(string name) { ServerName = name; }
        public string GetName() { return ServerName; }

        public void SetServerType(SERVER_TYPE eType) { m_eServerType = eType; }
        public SERVER_TYPE GetServerType() { return m_eServerType; }

        public void SetProcessState(PROCESS_STATE eState) { m_eProcessState = eState; }
        public PROCESS_STATE GetProcessState() { return m_eProcessState; }

        public void SetServableState(SERVABLE_STATE eState) { m_eServableState = eState; }
        public SERVABLE_STATE GetServableState() { return m_eServableState; }

        public void SetRevivable(bool bIs) { bRevivable = bIs; }
        public bool IsRevivable() { return bRevivable; }
    }
}
