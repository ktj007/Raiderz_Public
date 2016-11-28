#include "StdAfx.h"
#include "ZDuplicationLoginLogic.h"
#include "ZPlayerManager.h"
#include "ZDuplicationLoginManager.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZLoginServerFacade.h"
#include "ZPlayerCommandRouter.h"
#include "ZPlayerLogic.h"


ZDuplicationLoginLogic::ZDuplicationLoginLogic(void)
{
}

ZDuplicationLoginLogic::~ZDuplicationLoginLogic(void)
{
}

bool ZDuplicationLoginLogic::IsDuplicated(AID nAID)
{
	if (NULL == gmgr.pPlayerManager->Find(nAID)) return false;
	return true;
}

void ZDuplicationLoginLogic::StartKickInWorldPlayer(MUID uidPlayer, AID nAID, const wstring& strUserID, PmUserData* pPmUserData)
{
	// 기존에 중복접속 처리 대기중인 플레이어가 있다면 내쫒는다.
	if (gmgr.pDuplicationLoginManager->IsExist(nAID))
	{
		this->KickWaitingDupLoginPlayer(nAID);
	}

	ZPlayer* pExistPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pExistPlayer)
	{
		mlog("Error! ZPlayerLogic::KickDuplicatedLoginPlayer(), pExistPlayer(AID: %I64d) is Not Exist!\n", nAID);
		return;
	}

	// 중복 로그인 처리 대기 플레이어로 등록.
	gmgr.pDuplicationLoginManager->Add(nAID, strUserID, uidPlayer, pPmUserData);

	// 기존에 접속해있는 플레이어에 내보내기 통보.
	RouteKickDuplicatedPlayer(pExistPlayer);

	// 플레이어 상태 변경
	pExistPlayer->SetState(PS_KICK_BY_DUPLOGIN);
}

void ZDuplicationLoginLogic::KickWaitingDupLoginPlayer(AID nAID)
{
	ZDUPLICATION_LOGIN_INFO* pDuplicationInfo = gmgr.pDuplicationLoginManager->Get(nAID);
	if (pDuplicationInfo == NULL)
	{
		mlog("Error! ZDuplicationLoginLogic::KickWaitingDupLoginPlayer(), pDuplicationInfo(AID: %I64d) is Not Exist!\n", nAID);
		return;
	}

	MUID uidLoginServer = gmgr.pLoginServerFacade->GetUID();
	ZPlayerCommandRouter::SendDuplicatedPlayerLogin_ToLoginServer(uidLoginServer, pDuplicationInfo->uidPlayer);

	gmgr.pDuplicationLoginManager->Del(nAID);
}

void ZDuplicationLoginLogic::RouteKickDuplicatedPlayer(ZPlayer* pKickPlayer)
{
	RVALID(pKickPlayer != NULL);

	if (PS_SELECT_CHAR == pKickPlayer->GetState() ||
		PS_READY_LOGIN_GAMESERVER == pKickPlayer->GetState() ||
		PS_READY_MOVE_LOGINSERVER == pKickPlayer->GetState() ||
		PS_MOVE_LOGINSERVER == pKickPlayer->GetState())
	{
		MUID uidLoginServer = gmgr.pLoginServerFacade->GetUID();
		ZPlayerCommandRouter::SendDuplicatedPlayerLogin_ToLoginServer(uidLoginServer, pKickPlayer->GetUID(), pKickPlayer->GetReservedConnectionKey());
	}

	ZGameServerObject* pCurGameServer = gmgr.pGameServerObjectManager->FindGameServer(pKickPlayer->GetGameServerID());
	if (pCurGameServer)
		ZPlayerCommandRouter::SendDuplicatedPlayerLogin_ToGameServer(pCurGameServer->GetUID(), pKickPlayer->GetUID(), pKickPlayer->GetReservedConnectionKey());

	ZGameServerObject* pReservedGameServer = gmgr.pGameServerObjectManager->FindGameServer(pKickPlayer->GetReservedGameServerID());
	if (pReservedGameServer)
		ZPlayerCommandRouter::SendDuplicatedPlayerLogin_ToGameServer(pReservedGameServer->GetUID(), pKickPlayer->GetUID(), pKickPlayer->GetReservedConnectionKey());
}

bool ZDuplicationLoginLogic::ExistWaitingPlayer(AID nAID)
{
	return gmgr.pDuplicationLoginManager->IsExist(nAID);
}

void ZDuplicationLoginLogic::FlushDuplicatedLogin(AID nAID, const MUID& uidLeavePlayer)
{
	ZDUPLICATION_LOGIN_INFO* pDuplicationLiginInfo = gmgr.pDuplicationLoginManager->Get(nAID);
	if (NULL == pDuplicationLiginInfo) return;

	// 떠난 플레이어가 중복 로그인 처리를 기다리던 플레이어였을 경우.
	if (pDuplicationLiginInfo->uidPlayer == uidLeavePlayer)
	{
		gmgr.pDuplicationLoginManager->Del(nAID);
		return;
	}

	// 피망 데이터가 있으면 참조.
	PmUserData* pPmUserData = NULL;
	if (pDuplicationLiginInfo->bExistPmUserData)
		pPmUserData = &pDuplicationLiginInfo->pmUserData;
 	
	// 기존 플레이어 제거 / 새 플레이어 추가
	ZPlayerLogic playerLogic;
	playerLogic.DeletePlayer(nAID);
	if (!playerLogic.AddPlayer(pDuplicationLiginInfo->uidPlayer, nAID, pDuplicationLiginInfo->strUserID, pPmUserData))
	{
		mlog("Error! ZPlayerLogic::FlushDuplicatedLogin(), Failed AddPlayer. UID(%u,%u) , AID(%I64d)\n"
			, pDuplicationLiginInfo->uidPlayer.High, pDuplicationLiginInfo->uidPlayer.Low, nAID);

		gmgr.pDuplicationLoginManager->Del(nAID);
		return;
	}
	
	MUID uidLoginServer = gmgr.pLoginServerFacade->GetUID();
	ZPlayerCommandRouter::SendAddPlayerResponse(uidLoginServer, pDuplicationLiginInfo->uidPlayer, CR_SUCCESS);
	gmgr.pDuplicationLoginManager->Del(nAID);
}