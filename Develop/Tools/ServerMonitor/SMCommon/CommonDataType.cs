using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SMCommon
{
    public enum SERVER_TYPE
    {
        SERVER_LOGIN        = 0,
        SERVER_MASTER       = 1,
        SERVER_GAME         = 2,
        SERVER_APPLICATION  = 3
    }

    public enum PROCESS_STATE
    {
        TERMINATED,
        TERMINATING,
        STARTING,
        RUNNING,
        DIED,
        NOUPDATE,
    }

    public enum SERVABLE_STATE
    {
        INACTIVE,
        ACTIVE,
    }

}
