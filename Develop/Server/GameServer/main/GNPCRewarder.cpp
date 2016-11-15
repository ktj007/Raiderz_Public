#include "StdAfx.h"
#include "GNPCRewarder.h"
#include "GEntityNPC.h"
#include "GNPCRewardOwner.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GExpSystem.h"
#include "GQuestSystem.h"
#include "GFactionSystem.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GLootSystem.h"
#include "GEntityPlayer.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"
#include "GNPCInfoMgr.h"
#include "GLootModifyHandler.h"
#include "GQObjUpdater.h"
#include "GPartySystem.h"

GNPCRewarder::GNPCRewarder()
{
}

void GNPCRewarder::RewardByDie(GEntityNPC* pNPC)
{	
	if (NULL == pNPC) return;
	GNPCInfo* pNPCInfo = pNPC->GetNPCInfo();
	if (NULL == pNPCInfo) return;

	vector<int> vecAllBeneficiaryCID;
	MUID nPartyUID;
	pNPC->GetNPCRewardOwner().GetBeneficiaryAndParty(vecAllBeneficiaryCID, nPartyUID);
	if (true == vecAllBeneficiaryCID.empty())
	{
		// 플레이어가 죽은 상태에서 몬스터가 도트데미지로 죽었을때를 위해서 따로 처리
		if (pNPC->GetKiller().IsInvalid())
			return;

		GEntityPlayer* pKiller = pNPC->FindPlayer(pNPC->GetKiller());
		if (NULL == pKiller) return;

		vecAllBeneficiaryCID  = gsys.pPartySystem->CollectNeighborMemberCID(pKiller);
		nPartyUID = pKiller->GetUID();
	}

	vector<GEntityPlayer*> vecSameServerBeneficiary;
	vector<int> vecOtherServerBeneficiaryCID;
	DivideContriubtorByServer(vecAllBeneficiaryCID, vecSameServerBeneficiary, vecOtherServerBeneficiaryCID);

	if (true == vecSameServerBeneficiary.empty()) return;

	if (pNPC->ExpRewardable())
	{
		if (!IsChallengerField(pNPC->GetFieldInfo()))
		{
			// 도전자퀘스트 진행중인 필드에서는 경험치 얻지 않음
			Exp(vecSameServerBeneficiary, static_cast<int>(vecAllBeneficiaryCID.size()), pNPCInfo);
		}	
	}

	if (pNPC->QuestRewardable())
	{
		Quest(vecSameServerBeneficiary, pNPC);
	}
	
	Faction(vecSameServerBeneficiary, pNPCInfo);
	ItemByDie(vecAllBeneficiaryCID, nPartyUID, pNPC);

	NotifyReward(vecSameServerBeneficiary, pNPCInfo->nID);
	RewardOtherServerBeneficiary(vecOtherServerBeneficiaryCID, static_cast<int>(vecAllBeneficiaryCID.size()), pNPCInfo->nID);
}

void GNPCRewarder::RewardByDieForMasterServer(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, int nNPCID)
{
	GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(nNPCID);
	if (NULL == pNPCInfo) return;

	Exp(vecSameServerBeneficiary, nAllBeneficiaryCount, pNPCInfo);
	Faction(vecSameServerBeneficiary, pNPCInfo);

	NotifyReward(vecSameServerBeneficiary, pNPCInfo->nID);
}

void GNPCRewarder::RewardByGather(GEntityPlayer* pPlayer, int nLootID, GEntityNPC* pNPC)
{
	if (NULL == pPlayer) return;
	if (NULL == pNPC) return;

	ItemByGather(pPlayer, nLootID, pNPC);
}

void GNPCRewarder::DivideContriubtorByServer( const vector<int>& vecBeneficiaryCID, vector<GEntityPlayer*>& outvecSameServerBeneficiary, vector<int>& outvecOtherServerBeneficiaryCID )
{
	for each(int nBeneficiaryCID in vecBeneficiaryCID)
	{
		GEntityPlayer* pBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL == pBeneficiary)
		{
			outvecOtherServerBeneficiaryCID.push_back(nBeneficiaryCID);
		}
		else
		{
			outvecSameServerBeneficiary.push_back(pBeneficiary);
		}		
	}
}

void GNPCRewarder::Exp(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, GNPCInfo* pNPCInfo)
{
	gsys.pExpSystem->RewardExpByNPCKill(vecSameServerBeneficiary, nAllBeneficiaryCount, pNPCInfo);	
}

void GNPCRewarder::Quest(const vector<GEntityPlayer*>& vecSameServerBeneficiary, GEntityNPC* pNPC)
{
	if (NULL == pNPC) return;
	if (true == vecSameServerBeneficiary.empty()) return;

	float fNearDistance = FLT_MAX;
	GEntityPlayer* pQuestBeneficiary = NULL;
	for each (GEntityPlayer* pPlayer in vecSameServerBeneficiary)
	{
		if (pPlayer->GetUID() == pNPC->GetKiller())
		{
			pQuestBeneficiary = pPlayer;
			break;
		}

		float fDistance = pPlayer->DistanceTo(pNPC);
		if (fDistance < fNearDistance)
		{
			pQuestBeneficiary = pPlayer;
			fNearDistance = fDistance;
		}
	}

	gsys.pQuestSystem->GetQObjUpdater().UpdDestroyQObj(pQuestBeneficiary, pNPC->GetID(), 1, pNPC);
}

void GNPCRewarder::Faction(const vector<GEntityPlayer*>& vecSameServerBeneficiary, GNPCInfo* pNPCInfo)
{
	gsys.pFactionSystem->RewardFaction(vecSameServerBeneficiary, pNPCInfo);
}

void GNPCRewarder::ItemByDie(const vector<int>& vecBeneficiaryCID, MUID nPartyUID, GEntityNPC* pNPC)
{
	if (NULL == pNPC) return;

	if (pNPC->GetNPCLoot().GetDropList().DropByKill(vecBeneficiaryCID, nPartyUID))
	{
		gsys.pLootSystem->GetModifyHandler().RouteInsertLootableNPC(pNPC);
	}	
}

void GNPCRewarder::ItemByGather(GEntityPlayer* pPlayer, int nLootID, GEntityNPC* pNPC)
{
	if (NULL == pPlayer) return;
	if (NULL == pNPC) return;

	if (pNPC->GetNPCLoot().GetDropList().DropByGather(pPlayer->GetCID(), nLootID))
	{
		gsys.pLootSystem->GetModifyHandler().RouteInsertLootableNPC(pNPC);
	}
}

void GNPCRewarder::RewardOtherServerBeneficiary(const vector<int>& vecOtherServerBeneficiaryCID, int nAllBeneficiaryCount, int nNPCID)
{
	if (true == vecOtherServerBeneficiaryCID.empty()) return;

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_CHAR_NPCDIE_REWARD_REQ,
		3,
		NEW_BLOB(vecOtherServerBeneficiaryCID),
		NEW_INT(nAllBeneficiaryCount),
		NEW_INT(nNPCID));

	gsys.pMasterServerFacade->Route(pNewCmd);	
}

void GNPCRewarder::NotifyReward(vector<GEntityPlayer*> vecSameServerBeneficiary, int nNPCID)
{
	for each (GEntityPlayer* pSameServerBeneficiary in vecSameServerBeneficiary)
	{
		NotifyReward(pSameServerBeneficiary, nNPCID);
	}
}

void GNPCRewarder::NotifyReward( GEntityPlayer* pSameServerBeneficiary, int nNPCID )
{
	pSameServerBeneficiary->OnRewarded(nNPCID);
}

bool GNPCRewarder::IsChallengerField( const GFieldInfo* pFieldInfo ) const
{
	if (!pFieldInfo)
		return false;

	if (!pFieldInfo->m_pFieldGroup)
		return false;

	return pFieldInfo->m_pFieldGroup->bChallengerQuest;
}