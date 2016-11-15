#include "StdAfx.h"
#include "LCmdRouter_MoveServer.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "CCommandTable_Master.h"
#include "LCommandCenter.h"
#include "CCommandTable_Login.h"


//////////////////////////////////////////////////////////////////////////
/// For GameServer
void LCmdRouter_MoveServer::RouteReadyEnterLoginServerRes(const MUID& uidRequesterGameServer, const MUID& uidRequesterPlayer, int nResult, MUID uidConnectionKey)
{
	minet::MCommand* pNewCommand = gsys.pServer->GetMasterClient()->MakeNewSendCommand(MLC_COMM_READY_ENTER_LOGINSERVER_RES
		, 4
		, NEW_UID(uidRequesterGameServer)
		, NEW_UID(uidRequesterPlayer)
		, NEW_INT(nResult)
		, NEW_UID(uidConnectionKey)
		);

	gsys.pServer->GetMasterClient()->GetNetClient()->Post(pNewCommand);
}

//////////////////////////////////////////////////////////////////////////
/// For LoginServer
void LCmdRouter_MoveServer::RouteResponseMoveLoginServer(const MUID& uidPlayer, int nResult, const MUID& uidAllocUID)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER
		, uidPlayer
		, 2
		, NEW_INT(nResult)
		, NEW_UID(uidAllocUID)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}
