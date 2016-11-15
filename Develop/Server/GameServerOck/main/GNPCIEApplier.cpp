#include "StdAfx.h"
#include "GNPCIEApplier.h"
#include "GGlobal.h"
#include "GNPCInteractionInfo.h"
#include "GDialogSystem.h"
#include "GInteractionSystem.h"
#include "GNPCShopInfo.h"
#include "GNPCShopInfoMgr.h"
#include "GDialogInfo.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GPlayerQuest.h"
#include "GEntityPlayer.h"
#include "GPlayerInteraction.h"
#include "GLootSystem.h"
#include "GCraftInfo.h"
#include "GCraftInfoMgr.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GEntityNPC.h"
#include "GQuestSystem.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GCraftSystem.h"
#include "GNPCInteractor.h"
#include "GChallengerQuestMgr.h"
#include "GLootBeginner.h"
#include "GQObjInteractor.h"
#include "GMailSystem.h"
#include "GGuildSystem.h"
#include "GStorageSystem.h"


bool GNPCIEApplier::Apply(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIEInfo)
{
	if (NULL == pPlayer) return false;
	if (NULL == pIEInfo) return false;

	int nSingleAct = pIEInfo->GetSinlgeAct();

	switch (pIEInfo->m_nType)
	{
	case IT_DIALOG :		return Dialog(pPlayer, nSingleAct);
	case IT_NPCSHOP :		return Trade(pPlayer, nSingleAct);

	case IT_QUEST_BEGIN :	return QuestBegin(pPlayer, nSingleAct);
	case IT_QUEST_END :		return QuestEnd(pPlayer, nSingleAct);
	case IT_QUEST_INTERACT_MORTAL:	return QuestInteractMortal(pPlayer, nSingleAct);
	case IT_QUEST_INTERACT_IMMORTAL:return QuestInteractImmortal(pPlayer, nSingleAct);

	case IT_LOOT_MORTAL :	return LootMortal(pPlayer, nSingleAct);
	case IT_LOOT_IMMORTAL :	return LootImmortal(pPlayer, nSingleAct);
	case IT_CRAFT :			return Craft(pPlayer, nSingleAct);
	case IT_INN_SLEEP :		return InnSleep(pPlayer, nSingleAct);

	case IT_SOULBINDING:	return SoulBinding(pPlayer, nSingleAct);
	case IT_CHALLENGERQUEST:return ChallengerQuest(pPlayer);

	case IT_GATHER_1 :
	case IT_GATHER_2 :
	case IT_GATHER_3 :
	case IT_GATHER_4 :
	case IT_GATHER_5 :
	case IT_GATHER_6 :
	case IT_GATHER_7 :
	case IT_GATHER_8 :
	case IT_GATHER_9 :
	case IT_GATHER_10 :		return Gather(pPlayer, pIEInfo->m_nType, nSingleAct);
	case IT_MAILBOX :		return Mailbox(pPlayer);
	case IT_TRIGGER :		return Trigger(pPlayer, pIEInfo->m_vecAct);
	case IT_GUILD_CREATE :	return GuildCreate(pPlayer);
	case IT_GUILD_DESTROY :	return GuildDestroy(pPlayer);
	case IT_STORAGE :		return Storage(pPlayer);
	case IT_GUILD_STORAGE :	return GuildStorage(pPlayer);
	default :				return false;
	}

	return false;
}

bool GNPCIEApplier::Dialog( GEntityPlayer* pPlayer, int nDialogID )
{
	if (NULL == pPlayer) return false;

	if (false == gsys.pDialogSystem->Start(pPlayer, nDialogID)) return false;

	return true;
}

bool GNPCIEApplier::Trade( GEntityPlayer* pPlayer, int nTradeID )
{
	if (NULL == pPlayer) return false;
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nTradeID);
	if (NULL == pNPCShopInfo) return false;

	if (false == gsys.pDialogSystem->Start(pPlayer, pNPCShopInfo->m_pDialogInfo->m_nID)) return false;

	return true;
}

bool GNPCIEApplier::QuestBegin( GEntityPlayer* pPlayer, int nQuestID )
{
	if (NULL == pPlayer) return false;
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return false;

	if (false == gsys.pDialogSystem->Start(pPlayer, pQuestInfo->nGiveDialogID)) return false;

	return true;
}

bool GNPCIEApplier::QuestEnd( GEntityPlayer* pPlayer, int nQuestID )
{
	if (NULL == pPlayer) return false;
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return false;
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
	if (NULL == pPlayerQuest) return false;

	if (true == pPlayerQuest->IsComplete())
	{
		if (false == gsys.pDialogSystem->Start(pPlayer, pQuestInfo->nRewardDialogID)) return false;
	}
	// 퀘스트를 받고 완료하지 못한경우
	else
	{
		if (false == gsys.pDialogSystem->Start(pPlayer, pQuestInfo->nIncompleteDialogID)) return false;
	}

	return true;
}

bool GNPCIEApplier::QuestInteractMortal(GEntityPlayer* pPlayer, int nQuestID)
{
	if (NULL == pPlayer) return false;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_QUEST_INTERACT_MORTAL)) return false;

	return gsys.pQuestSystem->GetQObjInteractor().Prepare(pPlayer, pPlayer->GetInteraction().GetInteractingNPC(), pPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct());
}

bool GNPCIEApplier::QuestInteractImmortal( GEntityPlayer* pPlayer, int nQuestID )
{
	if (NULL == pPlayer) return false;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_QUEST_INTERACT_IMMORTAL)) return false;

	return gsys.pQuestSystem->GetQObjInteractor().Prepare(pPlayer, pPlayer->GetInteraction().GetInteractingNPC(), pPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct());
}

bool GNPCIEApplier::LootMortal( GEntityPlayer* pPlayer, int nLootID )
{
	if (NULL == pPlayer) return false;
	GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
	if (NULL == pNPC) return false;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_LOOT_MORTAL)) return false;

	if (false == gsys.pLootSystem->GetBeginner().InteractNPC(pPlayer, pNPC, nLootID, false)) return false;	

	return true;
}

bool GNPCIEApplier::LootImmortal( GEntityPlayer* pPlayer, int nLootID )
{
	if (NULL == pPlayer) return false;
	GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
	if (NULL == pNPC) return false;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_LOOT_IMMORTAL)) return false;

	if (false == gsys.pLootSystem->GetBeginner().InteractNPC(pPlayer, pNPC, nLootID, true)) return false;

	return true;
}

bool GNPCIEApplier::Craft( GEntityPlayer* pPlayer, int nCraftID )
{
	if (NULL == pPlayer) return false;
	GCraftInfo* pCraftInfo = gmgr.pCraftInfoMgr->Get(nCraftID);
	if (NULL == pCraftInfo) return false;

	if (false == gsys.pDialogSystem->Start(pPlayer, pCraftInfo->m_nDialogID)) return false;

	return true;
}

bool GNPCIEApplier::InnSleep( GEntityPlayer* pPlayer, int nDialogID)
{
	if (NULL == pPlayer) return false;

	if (false == gsys.pDialogSystem->Start(pPlayer, nDialogID)) return false;

	return true;	
}

bool GNPCIEApplier::SoulBinding(GEntityPlayer* pPlayer, uint8 nSoulBindingID)
{
	if (NULL == pPlayer) return false;
	GSoulBindingInfo* pSoulBindingInfo = gmgr.pSoulBindingInfoMgr->Get(nSoulBindingID);
	if (NULL == pSoulBindingInfo) return false;

	if (false == gsys.pDialogSystem->Start(pPlayer, pSoulBindingInfo->m_nDialogID)) return false;

	return true;	
}

bool GNPCIEApplier::ChallengerQuest(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	gmgr.pChallengerQuestMgr->RouteQuestList(pPlayer);

	return true;	
}

bool GNPCIEApplier::Gather(GEntityPlayer* pPlayer, INTERACTION_TYPE nIETypeGathering, int nLootID)
{
	if (NULL == pPlayer) return false;
	GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
	if (NULL == pNPC) return false;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, nIETypeGathering)) return false;

	int nGatherTier = (nIETypeGathering - IT_GATHER_1) + 1;
	if (false == gsys.pLootSystem->GetBeginner().GatherNPC(pPlayer, pNPC, nGatherTier, nLootID)) return false;	

	return true;
}

bool GNPCIEApplier::Mailbox(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_MAILBOX)) return false;

	gsys.pMailSystem->OpenMailbox(pPlayer);
	return true;
}

bool GNPCIEApplier::Trigger(GEntityPlayer* pPlayer, const vector<int>& vecAct)
{
	if (NULL == pPlayer) return false;
	if (vecAct.empty()) return false;
	GField* pField = pPlayer->GetField();
	if (NULL == pField) return false;

	int nSize = static_cast<int>(vecAct.size());
	for (int i=0; i+1<nSize; i+=2)
	{
		int nNPCID = vecAct[i];
		int nTalentID = vecAct[i+1];
	
		vector<GEntityNPC*> vecNPC = pField->FindNPCByID(nNPCID);
		for each (GEntityNPC* pNPC in vecNPC)
		{
			pNPC->UseTalentSelf(nTalentID);
		}		
	}
	
	gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer->GetInteraction().GetInteractingNPC());

	return true;
}

bool GNPCIEApplier::GuildCreate(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	return gsys.pGuildSystem->RouteShowCreateUI(pPlayer);
}

bool GNPCIEApplier::GuildDestroy( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;

	return gsys.pGuildSystem->RouteShowDestroyUI(pPlayer);
}

bool GNPCIEApplier::Storage( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;

	return gsys.pStorageSystem->Show(pPlayer);
}

bool GNPCIEApplier::GuildStorage( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;

	return gsys.pGuildSystem->ShowStorage(pPlayer);
}
