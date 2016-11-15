#include "StdAfx.h"
#include "ZCmdHandler_MoveServer.h"
#include "CCommandTable_Master.h"
#include "ZMoveServerLogic.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZLoginServerFacade.h"


ZCmdHandler_MoveServer::ZCmdHandler_MoveServer(MCommandCommunicator* pCC)
: MCommandHandler(pCC)
{
	SetCmdHandler(MLC_COMM_REQUEST_LOGIN_GAME_SERVER,			OnRequestLoginGameServer);
	SetCmdHandler(MMC_COMM_RESPONSE_LOGIN_GAME_SERVER,			OnResponseLoginGameServer);

	SetCmdHandler(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER,	OnRequestReserveMoveGameServer);
	SetCmdHandler(MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER,	OnResponseReadyEnterMovePlayer);

	SetCmdHandler(MMC_COMM_CANCEL_MOVE_GAME_SERVER,				OnCancelMoveGameServer);
	
	// From GameServer
	SetCmdHandler(MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ,		OnCommReserveMoveLoginServerReq);
	SetCmdHandler(MMC_COMM_CANCEL_MOVE_LOGINSERVER,				OnCommCancelMoveLoginServer);

	// From LoginServer
	SetCmdHandler(MLC_COMM_READY_ENTER_LOGINSERVER_RES,			OnCommReadyEnterLoginServerRes);
}


MCommandResult ZCmdHandler_MoveServer::OnRequestLoginGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequester;
	TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo;
	TD_LOGIN_FIELD_INFO* pLoginFieldInfo;

	if (!pCommand->GetParameter(&uidRequester,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pLoginAccountInfo, 1))			return CR_ERROR;
	if (!pCommand->GetSingleBlob(pLoginFieldInfo, 2))			return CR_ERROR;

	// 요청서버 확인
	MUID uidLoginServer = pCommand->GetSenderUID();
	if (uidLoginServer != gmgr.pLoginServerFacade->GetUID())	return CR_TRUE;

	ZMoveServerLogic logic;
	logic.RequestLoginGameServer(uidRequester, pLoginAccountInfo, pLoginFieldInfo);
	
	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnResponseLoginGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 인자 확인
	MUID uidRequestLoginServer;
	MUID uidRequester;
	int nResult;
	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo = NULL;

	if (!pCommand->GetParameter(&uidRequestLoginServer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequester,			1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,				2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pLoginGameServerInfo,	3))				return CR_ERROR;

	// 요청서버 확인
	MUID uidDestGameServer = pCommand->GetSenderUID();
	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidDestGameServer);
	if (pGameServerObject == NULL)	return CR_TRUE;

	ZMoveServerLogic logic;
	logic.ResponseLoginGameServer(uidRequester, nResult, uidDestGameServer, pLoginGameServerInfo);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnRequestReserveMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nToGameServerID;
	MUID uidRequester;

	if (!pCommand->GetParameter(&nToGameServerID,	0, MPT_INT)) return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequester,		1, MPT_UID)) return CR_ERROR;

	// 요청서버 찾기
	MUID uidFromGameServer = pCommand->GetSenderUID();
	ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pFromGameServer == NULL) return CR_TRUE;

	ZMoveServerLogic logic;
	logic.RequestReserveMoveGameServer(uidFromGameServer, uidRequester, nToGameServerID, pCommand);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnResponseReadyEnterMovePlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidFromGameServer;
	MUID uidRequester;
	int nResult;
	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo;
	int nCount;

	if (!pCommand->GetParameter(&uidFromGameServer,		0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequester,			1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,				2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetBlob(pLoginGameServerInfo, nCount,3))				return CR_ERROR;

	// 응답 서버 확인
	MUID uidToGameServer = pCommand->GetSenderUID();
	ZGameServerObject* pDestServer = gmgr.pGameServerObjectManager->FindGameServer(uidToGameServer);
	if (pDestServer == NULL) return CR_TRUE;

	ZMoveServerLogic logic;
	logic.ResponseReadyEnterMovePlayer(uidFromGameServer, uidRequester, nResult, uidToGameServer, pLoginGameServerInfo);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnCancelMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 이동 취소 요청자 확인
	MUID uidRequester;

	if (!pCommand->GetParameter(&uidRequester, 0, MPT_UID))	return CR_ERROR;

	// 요청 서버 확인
	MUID uidFromGameServer = pCommand->GetSenderUID();
	ZGameServerObject* pStayServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pStayServer == NULL) return CR_TRUE;

	ZMoveServerLogic logic;
	logic.CancelMoveGameServer(uidFromGameServer, uidRequester);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnCommReserveMoveLoginServerReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;

	if (!pCommand->GetParameter(&uidRequestPlayer,	0, MPT_UID))	return CR_ERROR;

	// 요청 플레이어 확인.
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pPlayer == NULL) return CR_TRUE;

	MUID uidGameServer = pCommand->GetSenderUID();

	ZMoveServerLogic logic;
	logic.ReserveMoveLoginServerReq(uidRequestPlayer, uidGameServer);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnCommCancelMoveLoginServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;

	if (!pCommand->GetParameter(&uidRequestPlayer,	0, MPT_UID))	return CR_ERROR;

	// 요청 플레이어 확인.
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pPlayer == NULL) return CR_TRUE;

	MUID uidGameServerToStay = pCommand->GetSenderUID();

	ZMoveServerLogic logic;
	logic.CancelMoveLoginServer(uidRequestPlayer, uidGameServerToStay);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_MoveServer::OnCommReadyEnterLoginServerRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestGameServer;
	MUID uidRequestPlayer;
	int nResult;
	MUID uidConnectionKey;

	if (!pCommand->GetParameter(&uidRequestGameServer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer,		1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,				2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidConnectionKey,		3, MPT_UID))	return CR_ERROR;

	// 요청 플레이어 확인.
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pPlayer == NULL) return CR_TRUE;

	ZMoveServerLogic logic;
	logic.ReadyEnterLoginServerRes(uidRequestGameServer, uidRequestPlayer, nResult, uidConnectionKey);

	return CR_TRUE;
}