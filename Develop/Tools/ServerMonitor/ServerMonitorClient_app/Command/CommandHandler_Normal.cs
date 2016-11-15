using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    class CommandHandler_Normal : CommandHandler
    {
        private ServerMonitorClient m_refServerMonitorClient = null;
        private CommandSender m_refCommandSender = null;

        public CommandHandler_Normal(ServerMonitorClient refServerMonitorClient, CommandSender refCommandSender)
        {
            m_refServerMonitorClient = refServerMonitorClient;
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
                case CMD.GET_SERVERLIST_RES:
                    return OnGetServerListRes(command);
                case CMD.GET_SERVERINFO_RES:
                    return OnGetServerInfoRes(command);
                case CMD.MONITORING_LOG:
                    return OnMonitoringLog(command);
            }
            return true;
        }

        private bool OnConnected(NetCommand command)
        {
            UInt64 nSessionID;
            bool bIsSuccess;

            if (command.ReadUInt64(out nSessionID) == false) return false;
            if (command.ReadBoolean(out bIsSuccess) == false) return false;

            m_refServerMonitorClient.OnConnectedMachine(nSessionID, bIsSuccess);
            return true;
        }
        private bool OnDisconnected(NetCommand command)
        {
            UInt64 nSessionID;

            if (command.ReadUInt64(out nSessionID) == false) return false;

            m_refServerMonitorClient.OnDisconnectedMachine(nSessionID);
            return true;
        }
        private bool OnCheckCommandVersionRes(NetCommand command)
        {
            bool bSuccess = false;

            if (command.ReadBoolean(out bSuccess) == false) return false;

            if (false == bSuccess)
            {
                MachineInfo refMachineInfo = m_refServerMonitorClient.refMachineInfoMgr.Find(command.SessionID);
                if (null == refMachineInfo) return false;
                
                System.Windows.Forms.MessageBox.Show(refMachineInfo.GetName() + " 머신과 커맨드 버젼이 일치하지 않아 종료합니다.");
                System.Windows.Forms.Application.Exit();
                return true;
            }

            m_refCommandSender.SendRequestServerList(command.SessionID);

            return true;
        }
        private bool OnGetServerListRes(NetCommand command)
        {
            byte nServerCount;

            if (command.ReadByte(out nServerCount) == false) return false;

            TRANS_SERVERLISTNODE[] arrServerListNode = new TRANS_SERVERLISTNODE[nServerCount];
            for (int i = 0; i < nServerCount; ++i)
            {
                if (command.ReadString(out arrServerListNode[i].strServerName) == false) return false;
                if (command.ReadByte(out arrServerListNode[i].nServerType) == false) return false;
                if (command.ReadInt32(out arrServerListNode[i].nWorldID) == false) return false;
                if (command.ReadInt32(out arrServerListNode[i].nServerID) == false) return false;
            }

            UInt64 nSessionID = command.SessionID;

            MachineInfo refMachineInfo = m_refServerMonitorClient.refMachineInfoMgr.Find(nSessionID);
            if (null == refMachineInfo) return false;

            m_refServerMonitorClient.ClearServerInfo(refMachineInfo.GetName());
            for (int i = 0; i < nServerCount; ++i)
            {
                m_refServerMonitorClient.AddServerInfo(refMachineInfo.GetName(), ref arrServerListNode[i]);
            }


            ServerInfoMgr refServerInfoMgr = m_refServerMonitorClient.refMachineInfoMgr.GetServerInfoMgr(refMachineInfo.GetName());

            string[] arrServerNames;
            refServerInfoMgr.GetAllServerName(out arrServerNames);
            m_refCommandSender.SendRequestServerInfo(nSessionID, arrServerNames);
            return true;
        }
        private bool OnGetServerInfoRes(NetCommand command)
        {
            byte nServerCount;

            if (command.ReadByte(out nServerCount) == false) return false;

            TRANS_SERVERINFONODE[] arrServerInfo = new TRANS_SERVERINFONODE[nServerCount];
            for (int i = 0; i < nServerCount; ++i)
            {
                if (command.ReadString(out arrServerInfo[i].strServerName) == false) return false;
                if (command.ReadByte(out arrServerInfo[i].nProcessState) == false) return false;
                if (command.ReadByte(out arrServerInfo[i].nServableState) == false) return false;
                if (command.ReadBoolean(out arrServerInfo[i].bRevivable) == false) return false;
                if (command.ReadInt32(out arrServerInfo[i].nCurServerPlayerCount) == false) return false;
            }

            UInt64 nSessionID = command.SessionID;

            MachineInfo refMachineInfo = m_refServerMonitorClient.refMachineInfoMgr.Find(nSessionID);
            if (null == refMachineInfo) return false;

            for (int i = 0; i < nServerCount; ++i)
            {
                string strServerName = arrServerInfo[i].strServerName;
                PROCESS_STATE eProcessState = (PROCESS_STATE)arrServerInfo[i].nProcessState;
                SERVABLE_STATE eServableState = (SERVABLE_STATE)arrServerInfo[i].nServableState;
                bool bRevivable = arrServerInfo[i].bRevivable;
                int nCurServerPlayerCount = arrServerInfo[i].nCurServerPlayerCount;

                m_refServerMonitorClient.UpdateServerInfo(refMachineInfo.GetName(), strServerName, eProcessState, eServableState, bRevivable, nCurServerPlayerCount);
            }

            m_refServerMonitorClient.UpdateMachinePlayerCount(refMachineInfo.GetName());

            return true;
        }
        private bool OnMonitoringLog(NetCommand command)
        {
            string strDateTime;
            string strServerName;
            int nWorldID;
            int nServerID;
            string strLogMessage;

            if (command.ReadString(out strDateTime) == false) return false;
            if (command.ReadString(out strServerName) == false) return false;
            if (command.ReadInt32(out nWorldID) == false) return false;
            if (command.ReadInt32(out nServerID) == false) return false;
            if (command.ReadString(out strLogMessage) == false) return false;

            UInt64 nSessionID = command.SessionID;

            m_refServerMonitorClient.AddMonitoringLog(strDateTime, nSessionID, strServerName, nWorldID, nServerID, strLogMessage);

            string[] arrServerName = { strServerName };
            m_refCommandSender.SendRequestServerInfo(nSessionID, arrServerName);

            return true;
        }

    }
}
