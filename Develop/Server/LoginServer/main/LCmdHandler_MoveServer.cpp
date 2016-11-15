#include "StdAfx.h"
#include "LCmdHandler_MoveServer.h"
#include "CCommandTable_Login.h"
#include "LCmdRouter_MoveServer.h"
#include "LMoveServerLogicFromGameServer.h"


LCmdHandler_MoveServer::LCmdHandler_MoveServer(MCommandCommunicator* pCC)
: MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_REQUEST_MOVE_LOGIN_SERVER,			OnRequestMoveLoginServer);
	SetCmdHandler(MC_COMM_REQUEST_MOVE_LOGIN_SERVER_ON_PMANG,	OnRequestMoveLoginServerOnPmang);
}


MCommandResult LCmdHandler_MoveServer::OnRequestMoveLoginServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_ERROR;

	MUID uidPlayer = pCommand->GetSenderUID();

	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		LCmdRouter_MoveServer::RouteResponseMoveLoginServer(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION, MUID::ZERO);
		return CR_TRUE;
	}

	LMoveServerLogicFromGameServer logic;
	logic.MoveLoginServer(uidPlayer, uidConnectionKey);
	
	return CR_TRUE;
}

MCommandResult LCmdHandler_MoveServer::OnRequestMoveLoginServerOnPmang(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;
	wstring strStatIndex;
	int nPCCafeID;

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_FALSE;
	if (pCommand->GetParameter(strStatIndex,		2, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nPCCafeID,			3, MPT_INT)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();

	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		LCmdRouter_MoveServer::RouteResponseMoveLoginServer(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION, MUID::ZERO);
		return CR_TRUE;
	}

	LMoveServerLogicFromGameServer logic;
	logic.MoveLoginServer(uidPlayer, uidConnectionKey, true, strStatIndex, nPCCafeID);

	return CR_TRUE;
}
