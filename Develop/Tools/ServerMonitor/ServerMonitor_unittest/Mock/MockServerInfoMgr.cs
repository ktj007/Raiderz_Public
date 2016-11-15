using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor;
using SMCommon;

namespace ServerMonitor_unittest.Mock
{
    class MockServerInfoMgr : ServerInfoMgr
    {
        public bool MockDelegatorAddServer(SERVER_TYPE eServerType, string strServerExePath, string strProcessName) { return true; }
        public bool MockDelegatorDeleteServer(string strExePath) { return true; }

        public bool Init(List<ServerInfo> refListServerInfo)
        {
            return base.Init(refListServerInfo, MockDelegatorAddServer, MockDelegatorDeleteServer);
        }
    }
}
