using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SMCommon;

namespace ServerMonitorClient
{
    public enum TRAY_ICON_TYPE
    {
        FINE = 0,
        BAD,
        CHANGING
    }

    public class TrayIconUpdater
    {
        public static readonly float TIME_DELAY_TO_UPDATE = 1.0f;

        private NotifyIcon m_refOriginIcon = null;
        private MachineInfoMgr m_refMachineInfoMgr = null;
        private Dictionary<TRAY_ICON_TYPE, NotifyIcon> m_dicTrayIcon = new Dictionary<TRAY_ICON_TYPE, NotifyIcon>();
        private TimeRegulator m_rgltUpdate = new TimeRegulator();

        public TrayIconUpdater(NotifyIcon refOriginIcon, MachineInfoMgr refMachineInfoMgr)
        {
            m_refOriginIcon = refOriginIcon;
            m_refMachineInfoMgr = refMachineInfoMgr;

            m_rgltUpdate.Start(TIME_DELAY_TO_UPDATE);
        }
        public void RegIcon(TRAY_ICON_TYPE eType, NotifyIcon refIcon)
        {
            m_dicTrayIcon.Add(eType, refIcon);
        }

        public void Update(float fDelta)
        {
            if (false == m_rgltUpdate.IsReady(fDelta)) return;

            bool bIsBad = false;
            bool bIsChangine = false;

            LinkedListNode<MachineInfo> nodeMachineInfo = m_refMachineInfoMgr.MachineInfoList.First;
            for (; nodeMachineInfo != null; nodeMachineInfo = nodeMachineInfo.Next)
            {
                MachineInfo refMachineInfo = nodeMachineInfo.Value;
                if (false == refMachineInfo.IsConnected) continue;

                ServerInfoMgr refServerInfoMgr = refMachineInfo.ServerInfoMgr;

                LinkedListNode<ServerInfo> nodeServerInfo = refServerInfoMgr.ServerInfoList.First;
                for (; nodeServerInfo != null; nodeServerInfo = nodeServerInfo.Next)
                {
                    ServerInfo refServerInfo = nodeServerInfo.Value;

                    if (PROCESS_STATE.DIED == refServerInfo.GetProcessState() ||
                        PROCESS_STATE.NOUPDATE == refServerInfo.GetProcessState() ||
                        PROCESS_STATE.TERMINATING == refServerInfo.GetProcessState() ||
                        PROCESS_STATE.TERMINATED == refServerInfo.GetProcessState()
                        )
                    {
                        bIsBad = true;
                    }

                    if (PROCESS_STATE.STARTING == refServerInfo.GetProcessState() ||
                        (PROCESS_STATE.RUNNING == refServerInfo.GetProcessState() &&
                        SERVABLE_STATE.INACTIVE == refServerInfo.GetServableState())
                        )
                    {
                        bIsChangine = true;
                    }
                }
            }

            if (bIsBad)
            {
                SetIcon(TRAY_ICON_TYPE.BAD);
                return;
            }
            if (!bIsBad && bIsChangine)
            {
                SetIcon(TRAY_ICON_TYPE.CHANGING);
                return;
            }
            SetIcon(TRAY_ICON_TYPE.FINE);            
        }


        private void SetIcon(TRAY_ICON_TYPE eType)
        {
            m_refOriginIcon.Icon = m_dicTrayIcon[eType].Icon;
            m_refOriginIcon.Text = m_dicTrayIcon[eType].Text;
        }
    }

}
