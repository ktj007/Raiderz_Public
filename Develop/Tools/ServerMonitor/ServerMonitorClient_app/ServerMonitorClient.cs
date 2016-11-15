using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SMCommon;

namespace ServerMonitorClient
{
    public class ServerMonitorClient
    {
        private FormServerMonitorClient m_refView = null;
        private NetClient m_netClient = null;
        private MachineInfoMgr m_machineInfoMgr = new MachineInfoMgr();
        private CommandSender m_commandSender = null;
        private ServerInfoRequester m_serverInfoRequester = null;
        private MachineConnector m_machineConnector = null;

        public MachineInfoMgr refMachineInfoMgr { get { return m_machineInfoMgr; } }
        public CommandSender refCommandSender { get { return m_commandSender; } }

        public bool Init(FormServerMonitorClient refServerMonitorClientView)
        {
            m_refView = refServerMonitorClientView;

            m_netClient = new NetClient();
            m_commandSender = new CommandSender(m_netClient);
            m_netClient.SetCommandHandler(new CommandHandler_Normal(this, m_commandSender));


            m_machineConnector = new MachineConnector(m_machineInfoMgr, m_netClient);
            m_serverInfoRequester = new ServerInfoRequester(m_machineInfoMgr, m_commandSender);

            if (LoadConfig() == false)
                return false;

            m_machineConnector.ConnectToAllMachines();
            RebuildTreeView();
            return true;
        }
        public void Destroy()
        {
            m_netClient.Destroy();
        }
        private bool LoadConfig()
        {
            Config config = new Config(new ConfigFileController());

            MachineInfo[] arrMachineInfos;
            if (config.LoadConfig(out arrMachineInfos) == false)
                return false;

            m_machineInfoMgr.AddMachineInfo(arrMachineInfos);
            return true;
        }
        public void Update(float fDelta)
        {
            m_netClient.Update();

            m_machineConnector.Update(fDelta);
            m_serverInfoRequester.Update(fDelta);

            m_refView.refTrayIconUpdater.Update(fDelta);
        }


        public bool ReconnectReload()
        {
            m_machineConnector.DisconnectAllMachines();
            m_machineInfoMgr.Clear();

            if (LoadConfig() == false)
                return false;

            m_machineConnector.ConnectToAllMachines();
            RebuildTreeView();
            return true;
        }
        public bool Refresh()
        {
            LinkedListNode<MachineInfo> node = m_machineInfoMgr.MachineInfoList.First;
            for (; node != null; node = node.Next)
            {
                ServerInfoMgr refServerInfoMgr = node.Value.ServerInfoMgr;
                refServerInfoMgr.Clear();

                m_commandSender.SendRequestServerList(node.Value.m_nSessionID);
            }
            return true;
        }
        public bool StartServer(string strMachineName, string strServerName)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(strMachineName);
            if (null == refMachineInfo) return false;

            m_commandSender.SendStartServer(refMachineInfo.GetSessionID(), strServerName);
            return true;
        }
        public bool TerminateServer(string strMachineName, string strServerName)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(strMachineName);
            if (null == refMachineInfo) return false;

            m_commandSender.SendTerminateServer(refMachineInfo.GetSessionID(), strServerName);
            return true;
        }
        public bool ToggleServerRevivable(string strMachineName, string strServerName)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(strMachineName);
            if (null == refMachineInfo) return false;

            ServerInfo refServerInfo = refMachineInfo.ServerInfoMgr.Find(strServerName);
            if (null == refServerInfo) return false;

            bool bRevivableToSet = !refServerInfo.IsRevivable();
            m_commandSender.SendToggleReviveServer(refMachineInfo.m_nSessionID, strServerName, bRevivableToSet);
            return true;
        }

        public void ClearServerInfo(string strMachineName)
        {
            ServerInfoMgr refServerInfoMgr = m_machineInfoMgr.GetServerInfoMgr(strMachineName);
            if (null == refServerInfoMgr) return;

            refServerInfoMgr.Clear();

            m_refView.refTreeViewCntroller.ClearServerNodes(strMachineName);
        }
        public void AddServerInfo(string strMachineName, ref TRANS_SERVERLISTNODE refServerListNode)
        {
            ServerInfoMgr refServerInfoMgr = m_machineInfoMgr.GetServerInfoMgr(strMachineName);
            if (refServerInfoMgr == null) return;

            refServerInfoMgr.AddServerInfo(ref refServerListNode);

            m_refView.refTreeViewCntroller.AddServerNode(strMachineName,
                                                        refServerListNode.strServerName);
        }
        public void UpdateServerInfo(string strMachineName, string strServerName, PROCESS_STATE eProcessState, SERVABLE_STATE eServableState, bool bRevivable, int nCurServerPlayerCount)
        {
            ServerInfoMgr refServerInfoMgr = m_machineInfoMgr.GetServerInfoMgr(strMachineName);
            if (null == refServerInfoMgr) return;

            ServerInfo refServerInfo = refServerInfoMgr.Find(strServerName);
            if (null == refServerInfo) return;

            refServerInfo.SetProcessState(eProcessState);
            refServerInfo.SetServableState(eServableState);
            refServerInfo.bRevivable = bRevivable;
            refServerInfo.CurPlayerCount = nCurServerPlayerCount;

            m_refView.refTreeViewCntroller.SetServerIcon(strMachineName, strServerName, eProcessState, eServableState, bRevivable);

            if (refServerInfo.GetServerType() == SERVER_TYPE.SERVER_GAME)
            {
                m_refView.refTreeViewCntroller.UpdateServerNodeExpansionInfo(strMachineName, strServerName, nCurServerPlayerCount);
            }

            if (m_refView.refTreeViewCntroller.IsSelectedServerNode() == false)
                return;
            
            string strSelectedMachineName;
            string strSelectedServerName;
            m_refView.refTreeViewCntroller.GetSelectedMachineName(out strSelectedMachineName);
            m_refView.refTreeViewCntroller.GetSelectedServerName(out strSelectedServerName);

            if (strSelectedMachineName.Length == 0 ||
                strSelectedServerName.Length == 0 ||
                strSelectedMachineName.Equals(strMachineName) == false ||
                strSelectedServerName.Equals(strServerName) == false)
                return;
            
            UpdateView_SelectedServerInfo(strMachineName, strServerName);
            m_refView.UpdateButtonEnableState();
        }
        public void UpdateMachinePlayerCount(string strMachineName)
        {
            int nPlayerCount = m_machineInfoMgr.GetMachinePlayerCount(strMachineName);
            m_refView.refTreeViewCntroller.UpdateMachineNodeExpansionInfo(strMachineName, nPlayerCount);
        }
        public void AddMonitoringLog(string strDateTime, UInt64 nSessionID, string strServerName, int nWorldID, int nServerID, string strLogMessage)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(nSessionID);
            if (null == refMachineInfo) return;

            string strLog;
            strLog = strDateTime;
            strLog += " - ";
            strLog += refMachineInfo.GetName();
            strLog += ", " + strServerName;
            strLog += "(" + nWorldID.ToString();
            strLog += ", ";
            strLog += nServerID.ToString() + ")";
            strLog += ", ";
            strLog += strLogMessage;

            m_refView.refMonitoringLogger.AddLog(strLog);
            m_refView.ShowBalloonTip_ServerInfo(refMachineInfo.GetName(), strServerName, nWorldID, nServerID, strLogMessage);
        }

        public void UpdateView_SelectedMachineInfo(string strMachineName)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(strMachineName);
            if (null == refMachineInfo) return;

            MACHINE_STATE eMachineState = refMachineInfo.GetState();
            int nServerCount = refMachineInfo.ServerInfoMgr.ServerInfoCount;

            string strSelectedMachineInfo;
            strSelectedMachineInfo  = "Machine Name : " + strMachineName + "\r\n";
            strSelectedMachineInfo += "Machine State : " + eMachineState.ToString() + "\r\n";
            strSelectedMachineInfo += "Server Count : " + nServerCount.ToString() + "\r\n";
            strSelectedMachineInfo += "IP Address : " + refMachineInfo.IP.ToString() + "\r\n";
            strSelectedMachineInfo += "Port Number : " + refMachineInfo.Port.ToString() + "\r\n";

            int nPlayerCount = m_machineInfoMgr.GetMachinePlayerCount(strMachineName);
           
            strSelectedMachineInfo += "Machine Player Count : " + nPlayerCount.ToString();

            m_refView.SelectedNodeInfoText = strSelectedMachineInfo;
        }
        public void UpdateView_SelectedServerInfo(string strMachineName, string strServerName)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(strMachineName);
            if (null == refMachineInfo) return;

            ServerInfoMgr refServerInfoMgr = refMachineInfo.ServerInfoMgr;

            ServerInfo refServerInfo = refServerInfoMgr.Find(strServerName);
            if (null == refServerInfoMgr) return;

            string strSelectedServerInfo;
            strSelectedServerInfo  = "Machine Name : " + strMachineName + "\r\n";
            strSelectedServerInfo += "Server Name : " + refServerInfo.GetName() + "\r\n";
            strSelectedServerInfo += "Server Type : " + refServerInfo.GetServerType().ToString() + "\r\n";
            strSelectedServerInfo += "Process State : " + refServerInfo.GetProcessState().ToString() + "\r\n";
            strSelectedServerInfo += "Servable State : " + refServerInfo.GetServableState().ToString() + "\r\n";
            strSelectedServerInfo += "Revivable : " + refServerInfo.bRevivable.ToString() + "\r\n";
            strSelectedServerInfo += "World ID : " + refServerInfo.WorldID.ToString() + "\r\n";
            strSelectedServerInfo += "Server ID : " + refServerInfo.ServerID.ToString() + "\r\n";
            if (refServerInfo.GetServerType() == SERVER_TYPE.SERVER_GAME)
            {
                strSelectedServerInfo += "Server Player Count : " + refServerInfo.CurPlayerCount.ToString() + "\r\n";
            }

            m_refView.SelectedNodeInfoText = strSelectedServerInfo;
        }

        public void OnConnectedMachine(UInt64 nSessionID, bool bIsSuccess)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(nSessionID);
            if (null == refMachineInfo) return;

            if (bIsSuccess == false)
            {
                m_netClient.DeleteClientSession(nSessionID);
                refMachineInfo.SetDisconnected();
                return;
            }

            refMachineInfo.SetConnected();

            m_refView.refTreeViewCntroller.SetMachineIcon(refMachineInfo.GetName(), true);

            m_commandSender.SendCheckCommandVersionReq(nSessionID, CMD.COMMAND_VERSION);

            string strMsg = "서버 모니터 연결 됨\n";
            strMsg += "Machine\t: " + refMachineInfo.GetName();
            m_refView.ShowBalloonTip("ServerMonitor", strMsg);
        }
        public void OnDisconnectedMachine(UInt64 nSessionID)
        {
            MachineInfo refMachineInfo = m_machineInfoMgr.Find(nSessionID);
            if (null == refMachineInfo) return;

            refMachineInfo.SetDisconnected();

            m_refView.refTreeViewCntroller.ClearServerNodes(refMachineInfo.GetName());
            m_refView.refTreeViewCntroller.SetMachineIcon(refMachineInfo.GetName(), false);
            m_refView.refTreeViewCntroller.UpdateMachineNodeExpansionInfo(refMachineInfo.GetName(), 0);

            string strMsg = "서버 모니터 연결 끊김\n";
            strMsg += "Machine\t: " + refMachineInfo.GetName();
            m_refView.ShowBalloonTip("ServerMonitor", strMsg);
        }

        private void RebuildTreeView()
        {
            m_refView.refTreeViewCntroller.Clear();

            LinkedListNode<MachineInfo> node = m_machineInfoMgr.MachineInfoList.First;
            for (; node != null; node = node.Next)
            {
                MachineInfo refMachineInfo = node.Value;

                m_refView.refTreeViewCntroller.AddMachineNode(refMachineInfo.MachineName, refMachineInfo.IsConnected);

                ServerInfoMgr refServerInfoMgr = refMachineInfo.ServerInfoMgr;

                string[] arrServerNames;
                refServerInfoMgr.GetAllServerName(out arrServerNames);
                for (int iServerNameIndex = 0; iServerNameIndex < arrServerNames.Length; ++iServerNameIndex)
                {
                    string strServerName = arrServerNames[iServerNameIndex];
                    ServerInfo refServerInfo = refServerInfoMgr.Find(strServerName);
                    if (null == refServerInfo) continue;

                    m_refView.refTreeViewCntroller.AddServerNode(node.Value.MachineName, strServerName);
                    m_refView.refTreeViewCntroller.SetServerIcon(node.Value.MachineName, strServerName
                                                                , refServerInfo.GetProcessState(), refServerInfo.GetServableState(), refServerInfo.IsRevivable());
                }
            }
        }

    }
}
