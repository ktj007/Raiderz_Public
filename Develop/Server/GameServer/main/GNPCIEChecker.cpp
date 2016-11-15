#include "StdAfx.h"
#include "GNPCIEChecker.h"
#include "GGlobal.h"
#include "GConditionsSystem.h"
#include "GNPCInteractionInfo.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GEntityPlayer.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GInteractionSystem.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GPlayerDoing.h"
#include "GNPCLoot.h"
#include "GNPCInteractor.h"
#include "GLootInfoMgr.h"
#include "GPlayerQObjective.h"
#include "GNPCShopInfo.h"
#include "GNPCShopInfoMgr.h"
#include "GPlayerFactions.h"
#include "GFactionCalculator.h"
#include "GItemHolder.h"
#include "GPlayerTalent.h"
#include "GDropList.h"
#include "GRelationChecker.h"
#include "GNPCInteraction.h"
#include "GInventory.h"
#include "GGuildMgr.h"

vector<GNPCInteractionElementInfo*> GNPCIEChecker::FilterIElementInfo(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GNPCInteractionElementInfo*>& vecIElementInfo, ITRIGGER_TYPE nITrigType, MUID uidUseItem)
{
	if (NULL == pPlayer) return vector<GNPCInteractionElementInfo*>();
	if (NULL == pNPC) return vector<GNPCInteractionElementInfo*>();

	vector<GNPCInteractionElementInfo*> vecFilteredIElementInfo;
	for each(GNPCInteractionElementInfo* pIElementInfo in vecIElementInfo)
	{
		if (pIElementInfo->GetTriggerType() != nITrigType)
			continue;		
		if (false == Check(pPlayer, pNPC, pIElementInfo))
			continue;

		if (ITRT_ITEM == nITrigType)
		{
			GItem* pUseItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, uidUseItem);
			if (NULL == pUseItem)
				continue;
			if (pIElementInfo->m_nUseItemID != pUseItem->GetID())
				continue;
		}

		vecFilteredIElementInfo.push_back(pIElementInfo);
	}

	return vecFilteredIElementInfo;
}

bool GNPCIEChecker::Check( GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo )
{
	bool bAutoCheckPass = true;
	vector<GCondition_Leaf*> vecFailConditionLeaf;

	return Check(pPlayer, pNPC, pIElementInfo, bAutoCheckPass, vecFailConditionLeaf);
}

bool GNPCIEChecker::Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo, bool& bAutoCheckPass, vector<GCondition_Leaf*>& vecFailConditionLeaf)
{
	VALID_RET(NULL != pPlayer,			false);
	VALID_RET(NULL != pIElementInfo,	false);

	bAutoCheckPass = CheckAuto(pPlayer, pNPC, pIElementInfo);
	bool bConditionCheckPass = gsys.pConditionsSystem->Check(pPlayer, pIElementInfo->m_pConditionsInfo, vecFailConditionLeaf);

	if (!bAutoCheckPass) return false;
	if (!bConditionCheckPass) return false;

	return true;
}

bool GNPCIEChecker::QuestBegin(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIElementInfo)
{
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pIElementInfo->GetSinlgeAct());
	if (NULL == pQuestInfo) return false;

	if (false == pQuestInfo->bRepeatable)
	{
		if (true == pPlayer->GetQuests().IsDoneQuest(pQuestInfo->nID))	return false;
	}

	if (true == pPlayer->GetQuests().IsDoingQuest(pQuestInfo->nID))	return false;

	return true;
}
bool GNPCIEChecker::QuestEnd(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIElementInfo)
{
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pIElementInfo->GetSinlgeAct());
	if (NULL == pQuestInfo) return false;
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
	if (NULL == pPlayerQuest) return false;

	if (false == pPlayerQuest->IsComplete() && 0 == pQuestInfo->nIncompleteDialogID ) return false;

	return true;
}
bool GNPCIEChecker::QuestInteract(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo)
{
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pIElementInfo->GetSinlgeAct());
	if (NULL == pQuestInfo) return false;
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
	if (NULL == pPlayerQuest) return false;
	if (true == pPlayerQuest->IsFail()) return false;
	if (1>= pIElementInfo->m_vecAct.size()) return false;

	for (size_t i=1; i<pIElementInfo->m_vecAct.size(); i++)
	{
		GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(pIElementInfo->m_vecAct[i]);
		if (NULL == pPlayerQObjective) continue;
		if (true == pPlayerQObjective->IsComplete()) continue;

		return true;
	}

	return false;
}
bool GNPCIEChecker::NPCShop(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIElementInfo)
{
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(pIElementInfo->GetSinlgeAct());
	if (NULL == pNPCShopInfo) return false;

	if (pNPCShopInfo->m_nFactionID != 0)
	{
		uint16 nFactionQuantity = pPlayer->GetPlayerFactions().GetQuantity(pNPCShopInfo->m_nFactionID);
		GFactionCalculator factionCalculator;
		if (factionCalculator.CheckTradable(nFactionQuantity) == false) return false;
	}

	return true;
}

bool GNPCIEChecker::GuildStorage( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (false == gmgr.pGuildMgr->IsExist(pPlayer->GetGID())) return false;

	return true;
}

bool GNPCIEChecker::Gather(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nGatherTier, UNGATHERABLE_REASON& outnUR)
{
	INTERACTION_TYPE nIEType = (INTERACTION_TYPE)(IT_GATHER_1 + (nGatherTier-1));
	return Gather(pPlayer, pNPC, nIEType, outnUR);
}

bool GNPCIEChecker::Gather( GEntityPlayer* pPlayer, GEntityNPC* pNPC, INTERACTION_TYPE nIETypeGathering )
{
	UNGATHERABLE_REASON nUR;
	return Gather(pPlayer, pNPC, nIETypeGathering, nUR);
}

bool GNPCIEChecker::Gather(GEntityPlayer* pPlayer, GEntityNPC* pNPC, INTERACTION_TYPE nIETypeGathering, UNGATHERABLE_REASON& outnUR)
{
	outnUR = UR_NONE;

	if (NPC_TYPE_GATHER != pNPC->GetNPCInfo()->nNpcType)
	{
		outnUR = UR_NPCTYPE;
		return false;
	}

	/// 채집 도구를 가지고 있는지 체크 (단, GATHER_LUMBER 가 아닐 경우에만.)
	GATHER_TYPE eGatherType = pNPC->GetNPCInfo()->nGatherType;
	if (GATHER_LUMBER != eGatherType)
	{
		if (false == pPlayer->GetItemHolder()->GetInventory().HasGatherTool(eGatherType))
		{
			outnUR = UR_TOOL;
			return false;
		}
	}

	/// 채집 탤런트를 배웠는지 체크
	if (false == pPlayer->GetTalent().IsTrainedGatherTalent(1))
	{
		outnUR = UR_TRAINING;
		return false;
	}

	/// 배운 채집 탤런트의 레벨이 채집 대상의 레벨보다 높은 체크
	int nGatherTier = (nIETypeGathering - IT_GATHER_1) + 1;
	if (false == pPlayer->GetTalent().IsTrainedGatherTalent(nGatherTier))
	{
		outnUR = UR_HIGHERTRAINING;
		return false;
	}

	return true;
}

bool GNPCIEChecker::CheckITargetCondition( GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo )
{
	if (ITCT_LIVE == pIElementInfo->m_nTargetCondition)
	{
		if (pNPC->IsDead())
			return false;
	}

	if (ITCT_DEAD == pIElementInfo->m_nTargetCondition)
	{
		if (!pNPC->IsDead())
			return false;
		if (!pNPC->GetNPCLoot().GetDropList().IsEmpty())
			return false;
	}		

	return true;
}

bool GNPCIEChecker::CheckAuto( GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo )
{
	if (!pNPC->GetNPCInteraction().IsEnable()) return false;

	GRelationChecker relationChecker;
	if (!pNPC->IsDead() && relationChecker.IsEnemy(pPlayer, pNPC)) return false;

	switch (pIElementInfo->m_nType)
	{
	case IT_QUEST_BEGIN:			if (false == QuestBegin(pPlayer, pIElementInfo)) return false; break;
	case IT_QUEST_END:				if (false == QuestEnd(pPlayer, pIElementInfo)) return false; break;

	case IT_QUEST_INTERACT_IMMORTAL:
	case IT_QUEST_INTERACT_MORTAL:	if (false == QuestInteract(pPlayer, pNPC, pIElementInfo)) return false; break;
	case IT_NPCSHOP:				if (false == NPCShop(pPlayer, pIElementInfo)) return false; break;

	case IT_GATHER_1 :
	case IT_GATHER_2 :
	case IT_GATHER_3 :
	case IT_GATHER_4 :
	case IT_GATHER_5 :
	case IT_GATHER_6 :
	case IT_GATHER_7 :
	case IT_GATHER_8 :
	case IT_GATHER_9 :
	case IT_GATHER_10:				if (false == Gather(pPlayer, pNPC, pIElementInfo->m_nType)) return false; break;
	case IT_GUILD_STORAGE :			if (false == GuildStorage(pPlayer)) return false; break;
	}

	if (false == CheckITargetCondition(pNPC, pIElementInfo))
		return false;

	return true;
}