using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ServerMonitor;
using ServerMonitor.DB;
using SMCommon;

namespace ServerMonitor_unittest.Mock
{
    public class MockDBMgr : DBMgr
    {
        public override bool IsSyncDBOpen { get { return true; } }

        public override bool GetServerStatusInfo_Sync(int nWorldID, int nServerID, out bool bIsRun, out bool bIsServable, out short nPlayerCount)
        {
            bIsRun = true;
            bIsServable = true;
            nPlayerCount = 0;
            return true;
        }

    }
}
