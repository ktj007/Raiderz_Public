#include "StdAfx.h"
#include "ZMoveServerLogic.h"
#include "ZLoginServerFacade.h"
#include "ZPlayer.h"
#include "ZPlayerManager.h"
#include "ZMoveServerCommandRouter.h"
#include "ZServer.h"
#include "CCommandResultTable.h"
#include "ZPlayerStateChanger.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZPlayerCommandRouter.h"
#include "ZPlayerLogic.h"
#include "ZLoginCommandRouter.h"
#include "CTransData.h"
#include "ZLoginCommandRouter.h"
#include "ZGateCommandRouter.h"
#include "ZLoginFieldGuide.h"
#include "ZLoginCommandRouter.h"



bool ZMoveServerLogic::RequestLoginGameServer(const MUID& uidRequester, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo)
{
	if (!RequestLoginGameServer_Check(uidRequester, pLoginAccountInfo, pLoginFieldInfo)) return false;
	RequestLoginGameServer_Apply(uidRequester, pLoginAccountInfo, pLoginFieldInfo);
	return true;
}
bool ZMoveServerLogic::RequestLoginGameServer_Check(const MUID& uidRequester, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo)
{
	RVALID_RET(pLoginAccountInfo, false);
	RVALID_RET(pLoginFieldInfo, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequester);
	if (NULL == pPlayer) return false;

	if (PS_SELECT_CHAR != pPlayer->GetState())
	{
		mlog3("Error! ZMoveServerLogic::RequestLoginGameServer_Check(), Player State is Not PS_SELECT_CHAR! (AID= %I64d , CID= %I64d)\n", pLoginAccountInfo->nAID, pLoginAccountInfo->nCID);
		return false;
	}

	return true;
}
void ZMoveServerLogic::RequestLoginGameServer_Apply(const MUID& uidRequester, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo)
{
	// 처리할 플레이어 확인
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequester);
	if (NULL == pPlayer) return;

	MUID uidLoginServer = gmgr.pLoginServerFacade->GetUID();
	ZGameServerObject* pDestServer = NULL;	///< 입장할 게임서버
	ZField* pDestField = NULL;				///< 입장할 필드

	/// 입장할 필드 탐색
	ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo;
	stPlayerFieldInfo.nSharedFieldID		= pLoginFieldInfo->nSharedFieldID;
	stPlayerFieldInfo.uidDynamicFieldGroup	= pLoginFieldInfo->uidDynamicFieldGroup;
	stPlayerFieldInfo.nDynamicFieldID		= pLoginFieldInfo->nDynamicFieldID;

	ZLoginFieldGuide loginFieldGuide(pLoginAccountInfo->nCID, stPlayerFieldInfo);
	if (false == loginFieldGuide.Search(pDestServer, pDestField))
	{
		ZLoginCommandRouter::RouteResponseLoginGameServer(uidLoginServer, uidRequester, CR_FAIL_LOGIN_GAME_SERVER_NOT_FOUND, NULL);
		return;
	}

	// 지금 입장할 필드를 찾지 못함. "잠시 후 다시 시도 해주세요"
	if (NULL == pDestServer || NULL == pDestField)
	{
		ZLoginCommandRouter::RouteResponseLoginGameServer(uidLoginServer, uidRequester, CR_SUCCESS_LOGIN_GAME_SERVER_RETRY, NULL);
		return;
	}

	// 필드 입장 예약
	ZPlayerLogic playerLogic;
	playerLogic.PrepareGateField(pPlayer, pDestField);

	// 플레이어 정보 셋팅
	MUID uidConnectionKey = gsys.pServer->NewReserverKey();
	pPlayer->SetReservedGameServerID(pDestServer->GetID());
	pPlayer->SetReservedConnectionKey(uidConnectionKey);

	// 플레이어 상태 변경
	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToREADY_LOGIN_GAMESERVER(pPlayer->GetAID());

	// 성공 커맨드 전달
	ZLoginCommandRouter::RouteRequestLoginGameServer(pDestServer->GetUID(), uidLoginServer, uidRequester, uidConnectionKey, pLoginAccountInfo);
}



//////////////////////////////////////////////////////////////////////////
/// 
bool ZMoveServerLogic::ResponseLoginGameServer(const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo)
{
	if (CR_SUCCESS != nResult)
	{
		ResponseLoginGameServer_ResultFAIL(uidRequester, nResult); return true;
	}
	
	if (!ResponseLoginGameServer_Check(uidRequester, uidToGameServer, nResult, pToGameServerInfo)) return false;
	ResponseLoginGameServer_Apply(uidRequester, nResult, uidToGameServer, pToGameServerInfo);
	return true;
}
void ZMoveServerLogic::ResponseLoginGameServer_ResultFAIL(const MUID& uidRequester, int nResult)
{
	if (CR_SUCCESS == nResult) return;

	// 플레이어 확인
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequester);
	if (NULL == pPlayer) return;

	pPlayer->SetReservedConnectionKey(MUID::ZERO);
	pPlayer->SetReservedGameServerID(INVALID_ID);

	// 로그인서버 연결 확인
	if (!gmgr.pLoginServerFacade->IsConnected()) return;

	// 필드 입장 예약 취소
	ZPlayerLogic playerLogic;
	playerLogic.CancelPrepareGateField(pPlayer);

	// 플레이어 상태 복구 : 이 경우 LoginServer 에서 이동요청 취소 커맨드를 보내지 않기 때문에 여기서 상태를 변경합니다.
	ZPlayerStateChanger stateChanger;
	stateChanger.Cancel_READY_LOGIN_GAMESERVER(pPlayer->GetAID());

	// LoginServer에 결과 전달
	MUID uidRequestLoginServer = gmgr.pLoginServerFacade->GetUID();
	ZLoginCommandRouter::RouteResponseLoginGameServer(uidRequestLoginServer, uidRequester, nResult, NULL);
}
bool ZMoveServerLogic::ResponseLoginGameServer_Check(const MUID& uidRequester, const MUID& uidToGameServer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo)
{
	if (!gmgr.pLoginServerFacade->IsConnected())
	{
		mlog("Error! ZMoveServerLogic::ResponseLoginGameServer_Check(), LoginServer Disconnected!\n");
		return false;
	}

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequester);
	if (pPlayer == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseLoginGameServer_Check(), Not Exist Player (UID= %u:%u)\n", uidRequester.High, uidRequester.Low);
		return false;
	}

	// 상태 검사.
	if (pPlayer->GetState() != PS_READY_LOGIN_GAMESERVER)
	{
		mlog("Error! ZMoveServerLogic::ResponseLoginGameServer_Check(), Player State= %d\n", pPlayer->GetState());
		return false;
	}

	ZGameServerObject* pDestGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidToGameServer);
	if (pDestGameServer == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseLoginGameServer_Check(), Not Exist GameServer ( UID= %u:%u )\n", uidToGameServer.High, uidToGameServer.Low);
		return false;
	}

	if (nResult == CR_SUCCESS && pToGameServerInfo == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseLoginGameServer_Check(), LoginGameServerInfo is NULL when CR_SUCCESS.\n");
		return false;
	}

	return true;
}
void ZMoveServerLogic::ResponseLoginGameServer_Apply(const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo)
{
	RVALID(pToGameServerInfo);

	// 플레이어 상태 변경
	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToMOVE_GAMESERVER(uidRequester);

	// LoginServer에 결과 전달
	MUID uidRequestLoginServer = gmgr.pLoginServerFacade->GetUID();
	ZLoginCommandRouter::RouteResponseLoginGameServer(uidRequestLoginServer, uidRequester, nResult, pToGameServerInfo);
}


//////////////////////////////////////////////////////////////////////////
/// RequestReserveMoveGameServer
bool ZMoveServerLogic::RequestReserveMoveGameServer(const MUID& uidFromGameServer, const MUID& uidRequester, int nToGameServerID, MCommand* pCommand)
{
	if (!RequestReserveMoveGameServer_Check(uidFromGameServer, uidRequester, nToGameServerID, pCommand)) return false;
	RequestReserveMoveGameServer_Apply(uidFromGameServer, uidRequester, nToGameServerID, pCommand);
	return true;
}
bool ZMoveServerLogic::RequestReserveMoveGameServer_Check(const MUID& uidFromGameServer, const MUID& uidRequester, int nToGameServerID, MCommand* pCommand)
{
	RVALID_RET(pCommand, false);

	ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pFromGameServer == NULL)
		return false;

	ZPlayer* pPlayer = pFromGameServer->GetPlayerMap().FindByUID(uidRequester);
	if (pPlayer == NULL)
		return false;

	if (pPlayer->GetState() != PS_IN_WORLD)
	{
		ZGateCommandRouter::RouteResponseReserveMoveGameServer(uidFromGameServer, uidRequester, CR_FAIL, NULL);
		return false;
	}

	// 목표 게임서버 찾기
	ZGameServerObject* pToGameServer = gmgr.pGameServerObjectManager->FindGameServer(nToGameServerID);
	if (pToGameServer == NULL)
	{
		ZGateCommandRouter::RouteResponseReserveMoveGameServer(uidFromGameServer, uidRequester, CR_FAIL, NULL);
		return false;
	}

	return true;
}
void ZMoveServerLogic::RequestReserveMoveGameServer_Apply(const MUID& uidFromGameServer, const MUID& uidRequester, int nToGameServerID, MCommand* pCommand)
{
	ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pFromGameServer == NULL) return;

	ZPlayer* pPlayer = pFromGameServer->GetPlayerMap().FindByUID(uidRequester);
	if (pPlayer == NULL) return;

	ZGameServerObject* pToGameServer = gmgr.pGameServerObjectManager->FindGameServer(nToGameServerID);
	if (pToGameServer == NULL) return;

	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToREADY_MOVE_GAMESERVER(pPlayer->GetUID());

	/// 연결 키 생성.
	MUID uidConnectionKey = gsys.pServer->NewReserverKey();
	pPlayer->SetReservedGameServerID(pToGameServer->GetID());
	pPlayer->SetReservedConnectionKey(uidConnectionKey);

	// 커맨드 전송
	ZGateCommandRouter::RouteRequestReadyEnterMovePlayer(pToGameServer->GetUID(), uidConnectionKey, uidRequester, uidFromGameServer, pCommand);
}


//////////////////////////////////////////////////////////////////////////
/// ResponseReadyEnterMovePlayer
bool ZMoveServerLogic::ResponseReadyEnterMovePlayer(const MUID& uidFromGameServer, const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo)
{
	// 서버 이동 처리
	if (nResult != CR_SUCCESS)
	{
		ResponseReadyEnterMovePlayer_ResultFAIL(uidFromGameServer, uidRequester, nResult, uidToGameServer); return true;
	}

	if (!ResponseReadyEnterMovePlayer_Check(uidFromGameServer, uidRequester, uidToGameServer, nResult, pToGameServerInfo)) return false;
	ResponseReadyEnterMovePlayer_Apply(uidFromGameServer, uidRequester, nResult, uidToGameServer, pToGameServerInfo);
	return true;
}
void ZMoveServerLogic::ResponseReadyEnterMovePlayer_ResultFAIL(const MUID& uidFromGameServer, const MUID& uidRequester, int nResult, const MUID& uidToGameServer)
{
	if (nResult == CR_SUCCESS) return;

	// 응답받은 서버에서 서버이동 실패처리를 마친 후 CANCEL 커맨드를 보냅니다.
	ZGateCommandRouter::RouteResponseReserveMoveGameServer(uidFromGameServer, uidRequester, nResult, NULL);
}
bool ZMoveServerLogic::ResponseReadyEnterMovePlayer_Check(const MUID& uidFromGameServer, const MUID& uidRequester, const MUID& uidToGameServer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo)
{
	// 요청 서버 확인
	ZGameServerObject* pRequestServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pRequestServer == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseReadyEnterMovePlayer_Check(), Not Exist Request GameServer ( UID= %u:%u )\n", uidFromGameServer.High, uidFromGameServer.Low);
		return false;
	}

	// 플레이어 확인
	ZPlayer* pPlayer = pRequestServer->GetPlayerMap().FindByUID(uidRequester);
	if (pPlayer == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseReadyEnterMovePlayer_Check(), Not Exist Player ( UID= %u:%u )\n", uidRequester.High, uidRequester.Low);
		return false;
	}

	// 대상 서버 확인
	ZGameServerObject* pDestServer = gmgr.pGameServerObjectManager->FindGameServer(uidToGameServer);
	if (pDestServer == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseReadyEnterMovePlayer_Check(), Not Exist Dest GameServer ( UID= %u:%u )\n", uidToGameServer.High, uidToGameServer.Low);
		return false;
	}

	// 상태 검사.
	if (pPlayer->GetState() != PS_READY_MOVE_GAMESERVER)
	{
		mlog("Error! ZMoveServerLogic::ResponseReadyEnterMovePlayer_Check(), Player State is not PS_READY_MOVE_GAMESERVER\n");
		ZGateCommandRouter::RouteResponseReserveMoveGameServer(uidFromGameServer, uidRequester, CR_FAIL, NULL);
		return false;
	}

	if (nResult == CR_SUCCESS && pToGameServerInfo == NULL)
	{
		mlog("Error! ZMoveServerLogic::ResponseReadyEnterMovePlayer_Check(), LoginGameServerInfo is NULL when CR_SUCCESS.\n");
		return false;
	}
	return true;
}
void ZMoveServerLogic::ResponseReadyEnterMovePlayer_Apply(const MUID& uidFromGameServer, const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo)
{
	// 요청 서버 확인
	ZGameServerObject* pRequestServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pRequestServer == NULL)	return;

	// 플레이어 확인
	ZPlayer* pPlayer = pRequestServer->GetPlayerMap().FindByUID(uidRequester);
	if (pPlayer == NULL) return;

	// 대상 서버 확인
	ZGameServerObject* pToGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidToGameServer);
	if (pToGameServer == NULL) return;

	pRequestServer->GetPlayerMap().Delete(pPlayer->GetAID());

	pPlayer->SetGameServerID(INVALID_ID);
	pPlayer->SetReservedGameServerID(pToGameServer->GetID());
	pPlayer->SetReservedConnectionKey(pToGameServerInfo->uidConnectionKey);

	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToMOVE_GAMESERVER(uidRequester);

	// 커맨드 전송
	ZGateCommandRouter::RouteResponseReserveMoveGameServer(uidFromGameServer, uidRequester, nResult, pToGameServerInfo);
}



//////////////////////////////////////////////////////////////////////////
///	Move GameServer
bool ZMoveServerLogic::CancelMoveGameServer(MUID uidFromGameServer, MUID uidRequester)
{
	if (!CancelMoveGameServer_Check(uidFromGameServer, uidRequester)) return false;
	CancelMoveGameServer_Apply(uidFromGameServer, uidRequester);
	return true;
}
bool ZMoveServerLogic::CancelMoveGameServer_Check(const MUID& uidFromGameServer, const MUID& uidRequester)
{
	ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pFromGameServer == NULL) return false;

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequester);
	if (pPlayer == NULL) return false;

	if (pPlayer->GetState() != PS_READY_MOVE_GAMESERVER &&
		pPlayer->GetState() != PS_MOVE_GAMESERVER)	return false;

	int nReservedToServerID = pPlayer->GetReservedGameServerID();
	ZGameServerObject* pToGameServer = gmgr.pGameServerObjectManager->FindGameServer(nReservedToServerID);
	if (pToGameServer == NULL)	return false;

	return true;
}
void ZMoveServerLogic::CancelMoveGameServer_Apply(const MUID& uidFromGameServer, const MUID& uidRequester)
{
	ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pFromGameServer == NULL) return;

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidRequester);
	if (pPlayer == NULL) return;

	int nReservedToServerID = pPlayer->GetReservedGameServerID();
	ZGameServerObject* pToGameServer = gmgr.pGameServerObjectManager->FindGameServer(nReservedToServerID);
	if (pToGameServer == NULL) return;

	// 서버 이동 처리 중이었을 경우, 플레이어 정보 롤백
	if (pPlayer->GetState() == PS_MOVE_GAMESERVER)
	{
		pPlayer->SetGameServerID(pFromGameServer->GetID());
		pFromGameServer->GetPlayerMap().Add(pPlayer);

		// 필드 예약 취소.
		ZPlayerLogic playerLogic;
		playerLogic.CancelPrepareGateField(pPlayer);

		// 프록시 갱신
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
	}

	// 이동상태 취소
	ZPlayerStateChanger stateChanger;
	stateChanger.Cancel_READY_MOVE_GAMESERVER(uidRequester);

	// 서버 이동 관련 정보 지우기
	MUID uidConnectionKey = pPlayer->GetReservedConnectionKey();
	
	pPlayer->SetReservedGameServerID(INVALID_ID);
	pPlayer->SetReservedConnectionKey(MUID::ZERO);

	// 목적지 서버에 통보
	ZGateCommandRouter::RouteCancelReservedMovePlayer(pToGameServer->GetUID(), uidConnectionKey);
}



//////////////////////////////////////////////////////////////////////////
/// ReserveMoveLoginServerReq
bool ZMoveServerLogic::ReserveMoveLoginServerReq(const MUID& uidPlayer, const MUID& uidGameServer)
{
	if (!ReserveMoveLoginServerReq_Check(uidPlayer)) return false;
	ReserveMoveLoginServerReq_Apply(uidPlayer, uidGameServer);
	return true;
}
bool ZMoveServerLogic::ReserveMoveLoginServerReq_Check(const MUID& uidPlayer)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
		return false;

	if (pPlayer->GetState() != PS_IN_WORLD)
		return false;

	return true;
}
void ZMoveServerLogic::ReserveMoveLoginServerReq_Apply(const MUID& uidPlayer, const MUID& uidGameServer)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
		return;

	/// Connection Key 생성.
	MUID uidConnectionKey = gsys.pServer->NewReserverKey();

	// 플레이어 상태변경
	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToREADY_MOVE_LOGINSERVER(pPlayer->GetUID());
	
	/// 서버 이동 준비 요청.
	MUID uidLoginServer = gmgr.pLoginServerFacade->GetUID();
	AID nAID = pPlayer->GetAID();
	wstring& strUserID = pPlayer->GetUserID();

	ZMoveServerCommandRouter::RouteReadyEnterLoginServerReq(uidLoginServer, uidGameServer, uidPlayer, uidConnectionKey, nAID, strUserID);
}



//////////////////////////////////////////////////////////////////////////
/// ReadyEnterLoginServerRes
bool ZMoveServerLogic::ReadyEnterLoginServerRes(const MUID& uidFromGameServer, const MUID& uidPlayer, int nResult, const MUID& uidConnectionKey)
{
	if (!ReadyEnterLoginServerRes_Check(uidFromGameServer, uidPlayer)) return false;
	ReadyEnterLoginServerRes_Apply(uidFromGameServer, uidPlayer, nResult, uidConnectionKey);
	return true;
}
bool ZMoveServerLogic::ReadyEnterLoginServerRes_Check(const MUID& uidFromGameServer, const MUID& uidPlayer)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
		return false;

	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pGameServerObject == NULL)
		return false;

	if (pPlayer->GetState() != PS_READY_MOVE_LOGINSERVER)
		return false;

	return true;
}
void ZMoveServerLogic::ReadyEnterLoginServerRes_Apply(const MUID& uidFromGameServer, const MUID& uidPlayer, int nResult, const MUID& uidConnectionKey)
{
	if (nResult != CR_SUCCESS)
	{
		ZMoveServerCommandRouter::RouteReserveMoveLoginServerRes(uidFromGameServer, uidPlayer, nResult, uidConnectionKey);
		return;
	}
	
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
		return;

	// 요청 서버 PlayerMap 에서 제거.
	ZGameServerObject* pRequestServer = gmgr.pGameServerObjectManager->FindGameServer(uidFromGameServer);
	if (pRequestServer == NULL)
		return;
	
	pRequestServer->GetPlayerMap().Delete(pPlayer->GetAID());
	
	// 플레이어 상태변경
	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToMOVE_LOGINSERVER(pPlayer->GetUID());

	pPlayer->SetGameServerID(INVALID_ID);
	pPlayer->SetReservedConnectionKey(uidConnectionKey); ///< Connection Key 셋팅.

	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);

	ZMoveServerCommandRouter::RouteReserveMoveLoginServerRes(uidFromGameServer, uidPlayer, nResult, uidConnectionKey);
}



//////////////////////////////////////////////////////////////////////////
/// CancelMoveLoginServer
bool ZMoveServerLogic::CancelMoveLoginServer(const MUID& uidPlayer, const MUID& uidGameServerToStay)
{
	if (!CancelMoveLoginServer_Check(uidPlayer, uidGameServerToStay)) return false;
	CancelMoveLoginServer_Apply(uidPlayer, uidGameServerToStay);
	return true;
}
bool ZMoveServerLogic::CancelMoveLoginServer_Check(const MUID& uidPlayer, const MUID& uidGameServerToStay)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
		return false;

	ZGameServerObject* pGameServerToStay = gmgr.pGameServerObjectManager->FindGameServer(uidGameServerToStay);
	if (pGameServerToStay == NULL)
		return false;

	if (pPlayer->GetState() != PS_MOVE_LOGINSERVER &&
		pPlayer->GetState() != PS_READY_MOVE_LOGINSERVER)
		return false;

	return true;
}
void ZMoveServerLogic::CancelMoveLoginServer_Apply(const MUID& uidPlayer, const MUID& uidGameServerToStay)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
		return;

	ZGameServerObject* pGameServerToStay = gmgr.pGameServerObjectManager->FindGameServer(uidGameServerToStay);
	if (pGameServerToStay == NULL)
		return;

	// 이미 이동 중이었으면 데이터 롤백.
	if (pPlayer->GetState() == PS_MOVE_LOGINSERVER)
	{
		int nStayServerID = pGameServerToStay->GetID();

		pPlayer->SetGameServerID(nStayServerID);
		pGameServerToStay->GetPlayerMap().Add(pPlayer);

		// 프록시 정보 갱신
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
	}

	// 플레이어 상태변경
	ZPlayerStateChanger stateChanger;
	stateChanger.Cancel_READY_MOVE_LOGINSERVER(pPlayer->GetUID());

	// LoginServer 에 서버 이동 취소 통보.
	MUID uidLoginServer = gmgr.pLoginServerFacade->GetUID();
	MUID uidConnectionKey = pPlayer->GetReservedConnectionKey();

	ZMoveServerCommandRouter::RouteCancelMoveLoginServer(uidLoginServer, uidConnectionKey);

	pPlayer->SetReservedConnectionKey(MUID::ZERO);
}
