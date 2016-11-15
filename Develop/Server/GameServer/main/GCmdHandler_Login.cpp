#include "stdafx.h"
#include "GCmdHandler_Login.h"
#include "MCommand.h"
#include "GCommandTable.h"
#include "GServer.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GDBManager.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "GPlayerAcceptManager.h"
#include "GServerPlayerRouter.h"
#include "GMasterServerFacade.h"
#include "CCommandResultTable.h"
#include "GReserveCmdManager.h"
#include "GLoginLogic.h"
#include "GConst.h"
#include "GConfig.h"
#include "GClientMoveServerRouter.h"


bool GCmdHandler_Login::CheckServerClosing(MUID uidPlayer)
{
	// 서버 종료 중에는 더이상 로그인을 받지 않는다.
	if (gsys.pServer->IsServerClosing())
	{
		gsys.pServer->Disconnect(uidPlayer);
		return true;
	}

	return false;
}

GCmdHandler_Login::GCmdHandler_Login(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_REQUEST_LOGIN_GAME_SERVER,			OnRequestLoginGameServer);
	SetCmdHandler(MC_COMM_REQUEST_MOVE_GAME_SERVER,				OnRequestMoveGameServer);

	SetCmdHandler(MC_CHAR_GAMESTART,							OnGameStart);

	SetCmdHandler(MC_COMM_REQUEST_LOGIN_GAME_SERVER_ON_PMANG,	OnRequestLoginGameServerOnPmang);
	SetCmdHandler(MC_COMM_REQUEST_MOVE_GAME_SERVER_ON_PMANG,	OnRequestMoveGameServerOnPmang);
}

MCommandResult GCmdHandler_Login::OnRequestLoginGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;	

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_FALSE;


	GLoginLogic logic;
	MUID uidPlayer = pCommand->GetSenderUID();

	if (CheckServerClosing(uidPlayer))
	{
		return CR_TRUE;
	}

	// 커맨드 버전 확인
	if (!logic.CheckCommandVersion(uidPlayer, nCommandVersion))
	{
		GClientMoveServerRouter router;
		router.RouteResponseLoginGameServer(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);
		return CR_FALSE;
	}


	// 로그인
	logic.LoginGameServer(uidPlayer, uidConnectionKey);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Login::OnRequestMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;	

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_FALSE;

	MUID uidPlayerSession = pCommand->GetSenderUID();

	
	if (CheckServerClosing(uidPlayerSession))
	{
		return CR_TRUE;
	}


	GLoginLogic logic;

	// 커맨드 버전 확인
	if (!logic.CheckCommandVersion(uidPlayerSession, nCommandVersion))
	{
		GClientMoveServerRouter router;
		router.RouteResponseMoveGameServer(uidPlayerSession, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);
		return CR_FALSE;
	}

	logic.MoveGameServer(uidPlayerSession, uidConnectionKey);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Login::OnRequestLoginGameServerOnPmang(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;
	wstring strStatIndex;
	int nPCCafeID;

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_FALSE;
	if (pCommand->GetParameter(strStatIndex,		2, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nPCCafeID,			3, MPT_INT)==false) return CR_FALSE;


	GLoginLogic logic;
	MUID uidPlayer = pCommand->GetSenderUID();

	if (CheckServerClosing(uidPlayer))
	{
		return CR_TRUE;
	}

	// 커맨드 버전 확인
	if (!logic.CheckCommandVersion(uidPlayer, nCommandVersion))
	{
		GClientMoveServerRouter router;
		router.RouteResponseLoginGameServer(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);
		return CR_FALSE;
	}
	
	bool bOnPmang = true;

	logic.LoginGameServer(uidPlayer, uidConnectionKey, bOnPmang, strStatIndex, nPCCafeID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Login::OnRequestMoveGameServerOnPmang(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;
	wstring strStatIndex;
	int nPCCafeID;

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_FALSE;
	if (pCommand->GetParameter(strStatIndex,		2, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nPCCafeID,			3, MPT_INT)==false) return CR_FALSE;


	GLoginLogic logic;
	MUID uidPlayerSession = pCommand->GetSenderUID();

	if (CheckServerClosing(uidPlayerSession))
	{
		return CR_TRUE;
	}

	// 커맨드 버전 확인
	if (!logic.CheckCommandVersion(uidPlayerSession, nCommandVersion))
	{
		GClientMoveServerRouter router;
		router.RouteResponseMoveGameServer(uidPlayerSession, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);
		return CR_FALSE;
	}

	bool bOnPmang = true;
	
	logic.MoveGameServer(uidPlayerSession, uidConnectionKey, bOnPmang, strStatIndex, nPCCafeID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Login::OnGameStart(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 게임 시작하자. 

	// 들어갈 채널, 필드 선택
	MUID uidPlayer = pCommand->GetSenderUID();
	int nReqFieldID = -1;
	vec3 vReqPos;

	if (IsRunForTest() || GConfig::IsStandAloneMode())
	{
		if (pCommand->GetParameter(&nReqFieldID,	0, MPT_INT)==false) return CR_FALSE;
		if (pCommand->GetParameter(&vReqPos,		1, MPT_VEC)==false) return CR_FALSE;	
	}

	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayerObject)
		return CR_TRUE;

	if (POS_SELECT_CHAR != pPlayerObject->GetState())
		return CR_TRUE;

	GEntityPlayer* pPlayer = pPlayerObject->GetEntity();
	if (NULL == pPlayer)
		return CR_TRUE;

	// 이동 요청한 위치 기억.
	pPlayer->GetPlayerField().GetFieldEntry().ReserveWarp(nReqFieldID, vReqPos);

	// 마스터 서버에 접속 알림
	GServerPlayerRouter* pServerRouter = gsys.pMasterServerFacade->GetPlayerRouter();
	pServerRouter->RoutePlayerState_InWorldReq(pPlayer);

	return CR_TRUE;
}
