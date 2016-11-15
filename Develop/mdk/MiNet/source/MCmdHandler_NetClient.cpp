#include "stdafx.h"
#include "MCmdHandler_NetClient.h"
#include "MCommand.h"
#include "MCommandTable.h"
#include "MNetClient.h"
#include "MErrorTable.h"

namespace minet {

MCmdHandler_NetClient::MCmdHandler_NetClient(MCommandCommunicator* pCC) 
	: MCommandHandler(pCC)
{
	SetCmdHandler(MC_NET_REPLY_CONNECT,				OnNetReplyConnect);

	SetCmdHandler(MC_LOCAL_NET_CONNECT,				OnLocalNetConnect);
	SetCmdHandler(MC_LOCAL_NET_CONNECT_WITH_NIC,	OnLocalNetConnectWIthNIC);
	SetCmdHandler(MC_LOCAL_NET_DISCONNECT,			OnLocalNetDisconnect);
}

MCmdHandler_NetClient::~MCmdHandler_NetClient()
{

}

MCommandResult MCmdHandler_NetClient::OnLocalNetConnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	char szRemoteIP[256] = "";
	int nPort = 0;

	if( !pCommand->GetParameter(szRemoteIP,			0, MPT_STR, sizeof(szRemoteIP)) )			return CR_ERROR;
	if( !pCommand->GetParameter(&nPort, 			1, MPT_INT) )								return CR_ERROR;

	int nReturn = NetClient(pHandler)->Connect(szRemoteIP, nPort);
	if(nReturn != MOK)
	{
		minet_log("Can't connect to server");
	}

	return CR_TRUE;
}

MCommandResult MCmdHandler_NetClient::OnLocalNetConnectWIthNIC(MCommand* pCommand, MCommandHandler* pHandler)
{
	char szRemoteIP[256] = "";
	int nPort = 0;
	char szMyNetworkCardIP[256] = "";

	if( !pCommand->GetParameter(szRemoteIP,			0, MPT_STR, sizeof(szRemoteIP)) )			return CR_ERROR;
	if( !pCommand->GetParameter(&nPort, 			1, MPT_INT) )								return CR_ERROR;
	if( !pCommand->GetParameter(szMyNetworkCardIP,	2, MPT_STR, sizeof(szMyNetworkCardIP)) )	return CR_ERROR;

	int nReturn = NetClient(pHandler)->Connect(szRemoteIP, nPort, szMyNetworkCardIP);
	if(nReturn != MOK)
	{
		minet_log("Can't connect to server");
	}

	return CR_TRUE;
}

MCommandResult MCmdHandler_NetClient::OnLocalNetDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	NetClient(pHandler)->Disconnect();
	return CR_TRUE;
}

MCommandResult MCmdHandler_NetClient::OnNetReplyConnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidAlloc;	// 할당 받을 클라이언트 UID
	MUID uidHost;	// 서버 UID
	unsigned long int nTimeStamp;

	if( !pCommand->GetParameter(&uidHost, 0, MPT_UID) ) return CR_ERROR;
	if( !pCommand->GetParameter(&uidAlloc, 1, MPT_UID) ) return CR_ERROR;
	if( !pCommand->GetParameter(&nTimeStamp, 2, MPT_UINT) ) return CR_ERROR;

	NetClient(pHandler)->OnReplyConnect(uidHost, uidAlloc, nTimeStamp);

	return CR_TRUE;
}

MNetClient* MCmdHandler_NetClient::NetClient( MCommandHandler* pHandler )
{
	return (static_cast<MNetClient*>(pHandler->GetCommandCommunicator()));
}
} // namespace minet