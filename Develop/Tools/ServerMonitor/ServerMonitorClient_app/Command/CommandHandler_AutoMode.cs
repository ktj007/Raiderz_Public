using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    class CommandHandler_AutoMode : CommandHandler
    {
        private AutoModeClient m_refAutoModeClient = null;
        private CommandSender m_refCommandSender = null;

        public CommandHandler_AutoMode(AutoModeClient refAutoModeClient, CommandSender refCommandSender)
        {
            m_refAutoModeClient = refAutoModeClient;
            m_refCommandSender = refCommandSender;
        }

        public bool OnCommand(NetCommand command)
        {
            switch (command.CommandID)
            {
                case LocalCMD.CONNECTED:
                    return OnConnected(command);
                case LocalCMD.DISCONNECTED:
                    return OnDisconnected(command);
                case CMD.CHECK_CMD_VERSION_RES:
                    return OnCheckCommandVersionRes(command);
                case CMD.MONITORING_LOG:
                    return OnMonitoringLog(command);
                case CMD.GET_SERVERINFO_RES:
                    return OnGetServerInfoRes(command);
            }
            return true;
        }

        private bool OnConnected(NetCommand command)
        {
            UInt64 nSessionID;
            bool bIsSuccess;

            if (command.ReadUInt64(out nSessionID) == false) return false;
            if (command.ReadBoolean(out bIsSuccess) == false) return false;

            if (bIsSuccess)
            {
                m_refAutoModeClient.OnConnected();
            }
            else
            {
                System.Windows.Forms.Application.Exit();
            }
            return true;
        }
        private bool OnDisconnected(NetCommand command)
        {
            System.Windows.Forms.Application.Exit();
            return true;
        }
        private bool OnCheckCommandVersionRes(NetCommand command)
        {
            bool bSuccess = false;

            if (command.ReadBoolean(out bSuccess) == false) return false;

            if (bSuccess == false)
            {
                FileLog.Instance.Write("커맨드 버젼이 일치하지 않아 종료합니다. 새 버젼으로 업데이트가 필요합니다.");
                System.Windows.Forms.Application.Exit();
                return false;
            }

            if (m_refAutoModeClient.Mode == AUTO_MODE_TYPE.JUST_START)
            {
                if (m_refAutoModeClient.TargetServerName.ToLower().Equals("all"))
                {
                    if (m_refCommandSender.SendStartAllServer(command.SessionID) == false)
                        return false;
                }
                else
                {
                    if (m_refCommandSender.SendStartServer(command.SessionID, m_refAutoModeClient.TargetServerName) == false)
                        return false;
                }
            }
            else if (m_refAutoModeClient.Mode == AUTO_MODE_TYPE.JUST_STOP)
            {
                if (m_refAutoModeClient.TargetServerName.ToLower().Equals("all"))
                {
                    if (m_refCommandSender.SendTerminateAllServer(command.SessionID) == false)
                        return false;
                }
                else
                {
                    if (m_refCommandSender.SendTerminateServer(command.SessionID, m_refAutoModeClient.TargetServerName) == false)
                        return false;
                }
            }

            return true;
        }

        private bool OnMonitoringLog(NetCommand command)
        {
            string[] arrServerName = new string[1];

            if (command.ReadString(out arrServerName[0]) == false) return false;

            UInt64 nSessionID = command.SessionID;
            if (m_refCommandSender.SendRequestServerInfo(nSessionID, arrServerName) == false)
                return false;

            return true;
        }
        private bool OnGetServerInfoRes(NetCommand command)
        {
            System.Windows.Forms.Application.Exit();
            return true;
        }

    }
}
