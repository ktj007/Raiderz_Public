using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public interface CommandHandler
    {
        bool OnCommand(NetCommand command);
    }
}
