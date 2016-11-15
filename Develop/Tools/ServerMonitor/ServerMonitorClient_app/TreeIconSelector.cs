using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public enum NODE_ICON
    {
        ICON_INDEX_NONE = -1,
        ICON_INDEX_MACHINE_CONN = 0,
        ICON_INDEX_MACHINE_DISCONN,

        ICON_INDEX_SERVER_NOT_DETECTED,
        ICON_INDEX_SERVER_PROCESS_CHANGING,
        ICON_INDEX_SERVER_BAD,
        ICON_INDEX_SERVER_FINE,
        ICON_INDEX_SERVER_FINE_NOT_REVIVE,
        ICON_INDEX_SERVER_INACTIVE,
        ICON_INDEX_SERVER_ERROR,
    }

    public class TreeIconSelector
    {
        public NODE_ICON GetMachineIcon(bool bConnected)
        {
            NODE_ICON eServerIcon = NODE_ICON.ICON_INDEX_NONE;
            eServerIcon = (bConnected ? NODE_ICON.ICON_INDEX_MACHINE_CONN : NODE_ICON.ICON_INDEX_MACHINE_DISCONN);
            return eServerIcon;
        }
        public NODE_ICON GetServerIcon(PROCESS_STATE eProcessState, SERVABLE_STATE eServableState, bool bRevivable)
        {
            NODE_ICON eServerIcon = NODE_ICON.ICON_INDEX_NONE;
            if (PROCESS_STATE.DIED == eProcessState ||
                PROCESS_STATE.NOUPDATE == eProcessState)
            {
                eServerIcon = NODE_ICON.ICON_INDEX_SERVER_ERROR;
            }
            else if (PROCESS_STATE.TERMINATING == eProcessState ||
                    PROCESS_STATE.STARTING == eProcessState)
            {
                eServerIcon = NODE_ICON.ICON_INDEX_SERVER_PROCESS_CHANGING;
            }
            else if (PROCESS_STATE.TERMINATED == eProcessState)
            {
                eServerIcon = NODE_ICON.ICON_INDEX_SERVER_BAD;
            }
            else if (PROCESS_STATE.RUNNING == eProcessState)
            {
                if (SERVABLE_STATE.INACTIVE == eServableState)
                {
                    return NODE_ICON.ICON_INDEX_SERVER_INACTIVE;
                }

                if (bRevivable)
                {
                    eServerIcon = NODE_ICON.ICON_INDEX_SERVER_FINE;
                }
                else
                {
                    eServerIcon = NODE_ICON.ICON_INDEX_SERVER_FINE_NOT_REVIVE;
                }
            }
            else if (PROCESS_STATE.STARTING == eProcessState)
            {
                eServerIcon = NODE_ICON.ICON_INDEX_SERVER_PROCESS_CHANGING;
            }
            return eServerIcon;
        }
    }
}
