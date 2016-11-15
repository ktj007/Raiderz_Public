using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SMCommon
{
    public struct TRANS_SERVERLISTNODE
    {
        public string strServerName;
        public byte nServerType;
        public int nWorldID;
        public int nServerID;
    }

    public struct TRANS_SERVERINFONODE
    {
        public string strServerName;
        public byte nProcessState;
        public byte nServableState;
        public bool bRevivable;
        public int nCurServerPlayerCount;
    }
}
