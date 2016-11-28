#include "stdafx.h"
#include "ZPlayerLogic.h"
#include "ZPlayer.h"
#include "ZPlayerManager.h"
#include "ZLoginServerFacade.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerPlayerMap.h"
#include "ZGlobal.h"
#include "ZPlayerCommandRouter.h"
#include "ZFieldCommandRouter.h"
#include "ZField.h"
#include "ZPMSSystem.h"
#include "PmRegionStat.h"
#include "ZDuplicationLoginManager.h"
#include "ZGateCommandRouter.h"
#include "ZSharedField.h"
#include "ZPlayerStateChanger.h"


bool ZPlayerLogic::AddPlayer(const MUID& uidPlayer, AID nAID, const wstring& strUserID, PmUserData* pPmangUserData/*=NULL*/)
{
	if (NULL != gmgr.pPlayerManager->Find(nAID)) return false;

	ZPlayer* pNewPlayer = gmgr.pPlayerManager->NewPlayer(uidPlayer, nAID, strUserID);
	if (pNewPlayer == NULL) return false;

	gmgr.pPlayerManager->Add(pNewPlayer);
	
	return true;
}

void ZPlayerLogic::DeletePlayer(AID nAID)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer) return;
	
	/// 캐릭터 퇴장 처리
	this->LeaveGameServer(nAID);

	gmgr.pPlayerManager->Delete(nAID);
}

bool ZPlayerLogic::OnSelectChar(AID nAID, MUID uidPlayer)
{
	RVALID_RET(nAID > 0, false);
	RVALID_RET(uidPlayer.IsValid(), false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer) return false;

	// GameServer 로부터 이동해온 플레이어일 경우
	if (pPlayer->GetState() == PS_MOVE_LOGINSERVER)
		this->LeaveGameServer(nAID);

	// Player UID 갱신
	if (pPlayer->GetUID() != uidPlayer)
	{
		gmgr.pPlayerManager->UpdateUID(pPlayer, uidPlayer);
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
	}

	// 상태 변경
	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToSELECT_CHAR(nAID);

	return true;
}

bool ZPlayerLogic::OnInWorld(MUID uidRequestGameServer, AID nAID, CID nCID, MUID uidPlayer, const std::wstring& strPlayerName, int nGID)
{
	RVALID_RET(uidRequestGameServer.IsValid(), false);
	RVALID_RET(nAID > 0, false);
	RVALID_RET(nCID > 0, false);
	RVALID_RET(uidPlayer.IsValid(), false);
	RVALID_RET(strPlayerName.size() > 0, false);

	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (NULL == pGameServerObject)
	{
		mlog("Failed! ZPlayerLogic::InWorld(), Not Exist GameServerObject( UID= %u:%u )\n", uidRequestGameServer.High, uidRequestGameServer.Low);
		return false;
	}
	
	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer)
	{
		mlog("Failed! ZPlayerLogic::InWorld(), Not Exist Player( AID= %I64d )\n", nAID);
		return false;
	}
	
	if (!InWorld_Check(pPlayer))
	{
		ZPlayerCommandRouter::SendPlayerStateInWorldRes(pGameServerObject->GetUID(), nCID, CR_FAIL_LOGIN_INVALID_PLAYER_WORLD_INFO);
		return false;
	}
	
	/// 새로운 월드 정보 설정
	gmgr.pPlayerManager->UpdateCIDUID(pPlayer, nCID, uidPlayer);
	pPlayer->SetWorldInfo(nCID, pGameServerObject->GetID(), uidPlayer, strPlayerName, nGID);
	
	/// 새로 진입한 게임서버에 플레이어 추가.
	pGameServerObject->GetPlayerMap().Add(pPlayer);
	
	/// 상태 변경
	ZPlayerStateChanger stateChanger;
	stateChanger.ChangeToIN_WORLD(nAID);
	
	/// 라우팅
	ZPlayerCommandRouter::SendPlayerStateInWorldRes(pGameServerObject->GetUID(), nCID, CR_SUCCESS);
	ZPlayerCommandRouter::RouteProxyPlayerAdd(pPlayer);
	return true;
}

bool ZPlayerLogic::InWorld_Check(ZPlayer* pPlayer)
{
	RVALID_RET(pPlayer, false);

	if (pPlayer->GetState() != PS_MOVE_GAMESERVER)
	{
		mlog("Failed! ZPlayerLogic::InWorld_Check(), State= %d\n", pPlayer->GetState());
		return false;
	}

	return true;
}

void ZPlayerLogic::LeaveGameServer(AID nAID)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer) return;

	/// 이동 예약된 필드가 있으면 퇴장처리
	if (pPlayer->GetReservedFieldUID().IsValid())
	{
		CancelPrepareGateField(pPlayer);
	}

	/// 현재 필드 퇴장 처리
	MUID uidField = pPlayer->GetFieldUID();
	if (uidField.IsValid())
	{
		OutField(pPlayer, uidField);
	}

	/// 게임서버 퇴장 처리
	int nServerID = pPlayer->GetGameServerID();
	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(nServerID);
	if (NULL != pGameServerObject)
	{
		pGameServerObject->GetPlayerMap().Delete(nAID);

		ZPlayerCommandRouter::RouteProxyPlayerRemove(pPlayer->GetCID());
	}

	/// 플레이어의 InWorld 관련 정보 삭제
	gmgr.pPlayerManager->DelCIDUID(pPlayer);
	pPlayer->ClearWorldInfo();
}

void ZPlayerLogic::PrepareGateField(ZPlayer* pPlayer, ZField* pField)
{
	RVALID(NULL != pPlayer);
	RVALID(NULL != pField);

	if (pPlayer->GetReservedFieldUID().IsValid())
		CancelPrepareGateField(pPlayer);

	pPlayer->SetReservedFieldUID(pField->GetUID());
	pField->IncreasePlayerCount();

	ZFieldCommandRouter::RouteUpdateFieldInfo(pField);
}

void ZPlayerLogic::CancelPrepareGateField(ZPlayer* pPlayer)
{
	RVALID(NULL != pPlayer);

	MUID uidReservedField = pPlayer->GetReservedFieldUID();
	if (uidReservedField.IsInvalid())
		return;

	pPlayer->ClearReservedFieldID();

	ZField* pReservedField = gmgr.pFieldManager->FindField(uidReservedField);
	if (NULL == pReservedField) return;

	pReservedField->DecreasePlayerCount();

	ZFieldCommandRouter::RouteUpdateFieldInfo(pReservedField);
}

void ZPlayerLogic::InField(MUID uidRequestGameServer, CID nCID, MUID uidField, int nFieldID)
{
	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (NULL == pGameServerObject) return;

	ZPlayer* pPlayer = pGameServerObject->GetPlayerMap().FindByCID(nCID);
	if (NULL == pPlayer) return;

	ZField* pInField = pGameServerObject->GetFieldMap().FindField(uidField);
	if (NULL == pInField) return;

	if (nFieldID != pInField->GetID())
	{
		mlog3("Error! ZPlayerLogic::InField(), InField ID is not match. (InFieldID=%d, local FieldID=%d)\n", nFieldID, pInField->GetID());
		return;
	}

	/// 이전필드 퇴장처리
	MUID uidOldField = pPlayer->GetFieldUID();
	if (uidOldField.IsValid())
	{
		OutField(pPlayer, uidOldField);
	}

	pPlayer->ClearReservedFieldID();
	pPlayer->SetFieldUID(uidField);
	pPlayer->SetFieldID(pInField->GetID());
	if (FT_SHARED == pInField->GetUID())
	{
		pPlayer->SetChannelID(ToSharedField(pInField)->GetChannelID());
	}

	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
}

void ZPlayerLogic::OutField(ZPlayer* pPlayer, MUID uidField)
{
	RVALID(NULL != pPlayer);
	RVALID(uidField.IsValid());

	ZField* pField = gmgr.pFieldManager->FindField(uidField);
	if (NULL == pField) return;
	
	pField->DecreasePlayerCount();
	pPlayer->SetFieldUID(MUID::ZERO);
	pPlayer->SetFieldID(INVALID_ID);
	pPlayer->SetChannelID(INVALID_ID);

	ZFieldCommandRouter::RouteUpdateFieldInfo(pField);
}

void ZPlayerLogic::JoinGuild(CID cid, int nGID)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByCID(cid);
	if (pPlayer == NULL)	return;

	pPlayer->SetGuildID(nGID);
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
}

void ZPlayerLogic::LeaveGuild(CID cid)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByCID(cid);
	if (pPlayer == NULL)	return;

	pPlayer->SetGuildID(INVALID_ID);
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
}

void ZPlayerLogic::AllMemberDeleteInLoginServer()
{
	vector<ZPlayer* > vecPlayer;

	ZPlayerAIDMap::iterator itor = const_cast<ZPlayerAIDMap& >(gmgr.pPlayerManager->GetPlayerMap()).begin();
	for (; itor != gmgr.pPlayerManager->GetPlayerMap().end(); ++itor)
	{
		ZPlayer* pPlayer = (*itor).second;

		if (pPlayer->GetState() == PS_SELECT_CHAR ||
			pPlayer->GetState() == PS_READY_LOGIN_GAMESERVER ||
			pPlayer->GetState() == PS_MOVE_LOGINSERVER)
		{
			vecPlayer.push_back(pPlayer);
		}
	}

	for (size_t i = 0; i < vecPlayer.size(); ++i)
	{
		DeletePlayer(vecPlayer[i]->GetAID());
	}
}

void ZPlayerLogic::AllMemberDeleteInGameServer(int nGameServerID)
{
	ZPlayerAIDMap::iterator itor = const_cast<ZPlayerAIDMap& >(gmgr.pPlayerManager->GetPlayerMap()).begin();
	for (; itor != gmgr.pPlayerManager->GetPlayerMap().end();)
	{
		ZPlayer* pPlayer = (*itor).second;

		if (pPlayer->GetGameServerID() == nGameServerID ||
			(pPlayer->IsMoveGameServer() && pPlayer->GetReservedGameServerID() == nGameServerID))
		{
			ZPlayerAIDMap::iterator itorNext =  ++itor;

			DeletePlayer(pPlayer->GetAID());

			itor = itorNext;
		}
		else
		{
			++itor;
		}
	}
}
