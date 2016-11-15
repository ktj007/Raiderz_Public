#include "stdafx.h"
#include "GCmdHandler_Master_Login.h"
#include "CCommandTable_Master.h"
#include "STransData.h"
#include "GServerGateRouter.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GPlayerAcceptManager.h"
#include "GPlayerObjectManager.h"
#include "CCommandResultTable.h"
#include "GEntityPlayer.h"
#include "GServer.h"
#include "GGameStarter.h"
#include "GMsgCommandFacade.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "GPlayerObject.h"

GCmdHandler_Master_Login::GCmdHandler_Master_Login(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_COMM_REQUEST_LOGIN_GAME_SERVER,			OnRequestLoginGameServer);
	SetCmdHandler(MMC_PLAYER_STATE_IN_WORLD_RES,				OnPlayerStateInWorldRes);
	SetCmdHandler(MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER,		OnRequestReadyEnterMovePlayer);
	SetCmdHandler(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER,	OnResponseReserveMoveGameServer);
	SetCmdHandler(MMC_COMM_CANCEL_RESERVED_MOVE_PLAYER,			OnCancelReservedMovePlayer);
}

MCommandResult GCmdHandler_Master_Login::OnRequestLoginGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestServer;
	MUID uidRequestID;
	MUID uidConnectionKey;
	TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo;

	if (!pCommand->GetParameter(&uidRequestServer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestID,		1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidConnectionKey,	2, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pLoginAccountInfo,	3))				return CR_ERROR;

	// ReservePlayerEnter --
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	GPlayerAcceptor_FromLoginServer* pPlayerAcceptor = new GPlayerAcceptor_FromLoginServer();
	pPlayerAcceptor->ReserveLogin(pLoginAccountInfo, uidConnectionKey, uidRequestID);

	GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	if (false == pAcceptManager->Add(pPlayerAcceptor, GPlayerAcceptor::FROM_LOGIN_SERVER))
	{
		SAFE_DELETE(pPlayerAcceptor);

		pServerRouter->ResponseLoginGameServer(uidRequestServer, uidRequestID, CR_FAIL_CHAR_ALEADY_RESERVED_CONNECTION, uidConnectionKey);
		return CR_TRUE;
	}

	pServerRouter->ResponseLoginGameServer(uidRequestServer, uidRequestID, CR_SUCCESS, uidConnectionKey);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Login::OnPlayerStateInWorldRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nResult;

	if( !pCommand->GetParameter(&nCID,		0, MPT_INT) )	return CR_ERROR;
	if( !pCommand->GetParameter(&nResult,	1, MPT_INT) )	return CR_ERROR;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (NULL == pPlayer) return CR_TRUE;

	if (CR_SUCCESS != nResult)
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(pPlayer->GetUID(), (CCommandResultTable)nResult);
		gsys.pServer->Disconnect(pPlayer->GetUID());
		return CR_TRUE;
	}

	int nFieldID = pPlayer->GetPlayerField().GetFieldEntry().GetReservedWarpFieldID();
	vec3 vReqPos = pPlayer->GetPlayerField().GetFieldEntry().GetReservedWarpPos();
	pPlayer->GetPlayerField().GetFieldEntry().ClearReserveWarp();


	// 게임 시작
	GGameStarter gameStarter;
	gameStarter.Start(pPlayer->GetUID(), nFieldID, vReqPos);

	if (gameStarter.GetResult() == GGameStarter::RESULT_SUCCESS)
		return CR_TRUE;

	// 실패 시, 메시지 전달
	switch(gameStarter.GetResult())
	{
	case GGameStarter::RESULT_FAILED_TO_FIND_PLAYER:
		gsys.pMsgCommandFacade->RouteSystemMsg(pPlayer->GetUID(), CR_FAIL_SYSTEM_INVALID_PC_UID);
		break;
	case GGameStarter::RESULT_FAILED_TO_GATE:
		gsys.pMsgCommandFacade->RouteSystemMsg(pPlayer->GetUID(), CR_FAIL_ENTER_FIELD);		
		break;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Login::OnRequestReadyEnterMovePlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestGameServer;
	MUID uidRequestPlayer;
	MUID uidConnectionKey;	

	if( !pCommand->GetParameter(&uidConnectionKey,		0, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&uidRequestGameServer,	1, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&uidRequestPlayer,		2, MPT_UID) )	return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(3);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;


	// ReservePlayerEnter --
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	GPlayerAcceptor_FromGameServer* pPlayerAcceptor = new GPlayerAcceptor_FromGameServer();
	pPlayerAcceptor->ReserveLogin(pCommand);

	GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	if (pAcceptManager->Add(pPlayerAcceptor, GPlayerAcceptor::FROM_GAME_SERVER) == false)
	{
		delete pPlayerAcceptor;

		pServerRouter->ResponseReadyEnterMovePlayer(uidRequestGameServer, uidRequestPlayer, CR_FAIL_CHAR_ALEADY_RESERVED_CONNECTION, uidConnectionKey);

		return CR_TRUE;
	}

	pServerRouter->ResponseReadyEnterMovePlayer(uidRequestGameServer, uidRequestPlayer, CR_SUCCESS, uidConnectionKey);

	CC_Check(birdkr_GCmdHandler_Master_Global_OnRequestReadyEnterMovePlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Login::OnResponseReserveMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequestPlayer;
	int nResult;
	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo;
	int nCount;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 1, MPT_INT)) return CR_ERROR;
	if (!pCommand->GetBlob(pLoginGameServerInfo, nCount, 2)) return CR_ERROR;


	// 요청 플레이어 확인
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidRequestPlayer);
	if (pPlayerObject == NULL)
	{
		return CR_TRUE;
	}


	// 커맨드 처리
	if (nResult == CR_SUCCESS && pLoginGameServerInfo != NULL)
	{
		pPlayerObject->GetSetMoveGameServer(pLoginGameServerInfo);
	}
	else
	{
		dlog("ReserveMoveGameServer Failed(CommandResult = %d)\n", nResult);

		pPlayerObject->CancelMoveGameServer();	// 서버이동 취소
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Login::OnCancelReservedMovePlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 예약 키를 받는다
	MUID uidConnectionKey;

	if (!pCommand->GetParameter(&uidConnectionKey, 0, MPT_UID))	return CR_FALSE;


	// 예약 플레이어 제거
	GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();

	pAcceptManager->Del(uidConnectionKey, GPlayerAcceptor::FROM_GAME_SERVER);
	pAcceptManager->Update();


	return CR_TRUE;
}
