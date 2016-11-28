#include "StdAfx.h"
#include "LCmdHandler_Master_MoveServer.h"
#include "CCommandTable_Master.h"
#include "CCommandResultTable.h"
#include "LCmdRouter_MoveServer.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LGlobal.h"
#include "LPlayerAcceptManager.h"

#include "CTransData.h"
#include "LTimeoutManager.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LPlayerLoginGameServerRetryer.h"
#include "SCmdRouter_Login.h"
#include "LCommandCenter.h"


LCmdHandler_Master_MoveServer::LCmdHandler_Master_MoveServer(MCommandCommunicator* pCC)
: MCommandHandler(pCC)
{
	SetCmdHandler(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER,	OnResponseLoginGameServer);
	SetCmdHandler(MLC_COMM_READY_ENTER_LOGINSERVER_REQ,	OnReadyEnterLoginServerReq);
	SetCmdHandler(MLC_COMM_CANCEL_MOVE_LOGINSERVER,		OnCancelMoveLoginServer);
}


MCommandResult LCmdHandler_Master_MoveServer::OnResponseLoginGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 인자 확인
	MUID uidPlayer;
	int nResult = 0;
	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo = NULL;
	int nCount = 0;

	if (!pCommand->GetParameter(&uidPlayer,	0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,	1, MPT_INT))		return CR_ERROR;
	if (!pCommand->GetBlob(pLoginGameServerInfo, nCount, 2))	return CR_ERROR;

	// Timeout 체크 정지
	gmgr.pTimeoutManager->StopWatchingSelMyCharTimeout(uidPlayer);

	// 요청 플레이어 확인
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return CR_TRUE;
	if (pPlayer->GetState() != POS_LOGIN_GAME_SERVER) return CR_TRUE;

	if (CR_SUCCESS_LOGIN_GAME_SERVER_RETRY == nResult)
	{
		// 재시도 응답일 경우 Retryer 에 등록
		gsys.pPlayerLoginGameServerRetryer->AddRetryer(uidPlayer, pPlayer->GetSelectedCharIndex());

		return CR_TRUE;
	}

	if (CR_SUCCESS == nResult)
	{
		// 결과가 성공일 경우 MOVER_SERVER 상태로 설정.
		pPlayer->OnMoveServer();

		// 서버 이동 Timeout 시작
		gmgr.pTimeoutManager->StartWatchingMoveServerTimeout(uidPlayer);
	}
	else
	{
		// 결과가 실패일 경우, SelectChar 상태로 되돌림.
		pPlayer->OnSelectChar();
	}


	// 결과 전달
	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseSelMyChar(pPlayer->GetUID(), nResult);

	if (nResult == CR_SUCCESS && pLoginGameServerInfo != NULL)
	{
		cmdRouter.MoveToGameServer(pPlayer->GetUID(), pLoginGameServerInfo);
	}

	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_MoveServer::OnReadyEnterLoginServerReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID	uidRequestGameServer;
	MUID	uidRequester;
	MUID	uidConnectionKey;
	AID		nAID;
	wstring	strUserID;

	if (!pCommand->GetParameter(&uidRequestGameServer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequester,			1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidConnectionKey,		2, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nAID,					3, MPT_INT64))	return CR_ERROR;
	if (!pCommand->GetParameter(strUserID,				4, MPT_WSTR))	return CR_ERROR;
	
	if (gmgr.pPlayerAcceptManager->FindAcceptor(uidConnectionKey))
	{
		LCmdRouter_MoveServer::RouteReadyEnterLoginServerRes(uidRequestGameServer, uidRequester, CR_FAIL, uidConnectionKey);
		mlog("Failed! LCmdHandler_Master_MoveServer::OnReadyEnterLoginServerReq(), Already Exist Acceptor! (AID : %I64d)\n", nAID);
		return CR_TRUE;
	}

	LPlayerAcceptor_FromGameServer* pAcceptor = new LPlayerAcceptor_FromGameServer();
	pAcceptor->ReserveAccept(uidConnectionKey, nAID, strUserID, uidRequester);

	if (!gmgr.pPlayerAcceptManager->Add(pAcceptor))
	{
		SAFE_DELETE(pAcceptor);
		LCmdRouter_MoveServer::RouteReadyEnterLoginServerRes(uidRequestGameServer, uidRequester, CR_FAIL, uidConnectionKey);
		return CR_TRUE;
	}

	LCmdRouter_MoveServer::RouteReadyEnterLoginServerRes(uidRequestGameServer, uidRequester, CR_SUCCESS, uidConnectionKey);

	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_MoveServer::OnCancelMoveLoginServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;

	if (!pCommand->GetParameter(&uidConnectionKey, 0, MPT_UID))		return CR_ERROR;
	
	gmgr.pPlayerAcceptManager->Del(uidConnectionKey);
	
	return CR_TRUE;
}
