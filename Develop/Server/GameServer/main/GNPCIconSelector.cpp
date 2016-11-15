#include "StdAfx.h"
#include "GNPCIconSelector.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GConditionInfo.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GPlayerQuests.h"
#include "GPlayerQObjective.h"
#include "GLootInfoMgr.h"
#include "GLootInfo.h"
#include "GNPCIEChecker.h"
#include "GQuestInfo.h"
#include "GItemHolder.h"


vector<NPC_ICON> GNPCIconSelector::Select(GEntityPlayer* pPlayer, const vector<MUID>& vecUIDNPC, CONDITION_TYPE nConditionType)
{
	if (NULL == pPlayer) return vector<NPC_ICON>();

	vector<NPC_ICON> vecNPCIcon;
	for each (const MUID& uidNPC in vecUIDNPC)
	{
		GEntityNPC* pNPC = pPlayer->FindNPC(uidNPC);
		if (NULL == pNPC) continue;

		if (CT_MAX != nConditionType)
		{
			if (false == pNPC->GetNPCInfo()->m_InteractionInfo.HasCondition(nConditionType) &&
				false == HasAutoCondition(pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, nConditionType)) continue;
		}

		NPC_ICON_TYPE nIconType = SingleSelect(pPlayer, pNPC);
		vecNPCIcon.push_back(NPC_ICON(pNPC, nIconType));
	}

	return vecNPCIcon;
}

bool GNPCIconSelector::HasAutoCondition(const vector<GNPCInteractionElementInfo*>& vecIElementInfo, CONDITION_TYPE nConditionType )
{
	for each (GNPCInteractionElementInfo* pIElementInfo in vecIElementInfo)
	{
		if (NULL == pIElementInfo) continue;

		if (CT_QUEST == nConditionType ||
			CT_QUEST_VAR == nConditionType || 
			CT_QUEST_OBJECTIVE_COMPLETE == nConditionType || 
			CT_QUEST_OBJECTIVE_INCOMPLETE == nConditionType)
		{
			if (IT_QUEST_BEGIN == pIElementInfo->m_nType || 
				IT_QUEST_END == pIElementInfo->m_nType ||
				IT_QUEST_INTERACT_IMMORTAL == pIElementInfo->m_nType ||
				IT_QUEST_INTERACT_MORTAL == pIElementInfo->m_nType)
			{
				return true;
			}
		}

		if (CT_FACTION == nConditionType &&
			IT_NPCSHOP == pIElementInfo->m_nType)
		{
			return true;
		}

		if ((CT_TALENT == nConditionType ||	CT_ITEM == nConditionType) &&
			IsIEGatherType(pIElementInfo->m_nType))
		{
			return true;
		}

		if (CT_ITEM == nConditionType && 
			ITRT_ITEM == pIElementInfo->GetTriggerType())
		{
			return true;
		}
	}

	return false;
}


NPC_ICON_TYPE GNPCIconSelector::SingleSelect(GEntityPlayer* pPlayer, GEntityNPC* pNPC)
{
	if (NULL == pPlayer)				return NIT_NONE;
	if (NULL == pNPC)					return NIT_NONE;
	if (NULL == pNPC->GetNPCInfo())		return NIT_NONE;	

	bool bExistInteractableIcon = false;
	NPC_ICON_TYPE nTopIconType = NIT_NONE;
	for each (GNPCInteractionElementInfo* pIEInfo in pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo)
	{
		NPC_ICON_TYPE nIconType = NIT_NONE;
		if (ITRT_CLICK == pIEInfo->GetTriggerType())
		{
			nIconType = ClickIETypeToIconType(pPlayer, pNPC, pIEInfo);
		}
		else if (ITRT_ITEM == pIEInfo->GetTriggerType())
		{
			nIconType = ItemIETypeToIconType(pPlayer, pNPC, pIEInfo);
		}

		if (NIT_NONE != nIconType && 
			NIT_QUEST_NOT_BEGIN_ONLYICON != nIconType && 
			NIT_QUEST_CONTINUE_ONLYICON !=nIconType)
		{
			bExistInteractableIcon = true;
		}

		if (nTopIconType < nIconType)
		{
			nTopIconType = nIconType;
		}
	}

	if (bExistInteractableIcon)
	{
		if (NIT_QUEST_NOT_BEGIN_ONLYICON == nTopIconType)
		{
			nTopIconType = NIT_QUEST_NOT_BEGIN;
		}
		else if (NIT_QUEST_CONTINUE_ONLYICON == nTopIconType)
		{
			nTopIconType = NIT_QUEST_CONTINUE;
		}
	}


	return nTopIconType;
}

NPC_ICON_TYPE GNPCIconSelector::ClickIETypeToIconType(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo*pIEInfo)
{
	if (NULL == pPlayer) return NIT_NONE;
	if (NULL == pNPC) return NIT_NONE;
	if (NULL == pIEInfo) return NIT_NONE;

	GNPCIEChecker IEChecker;
	bool bAutoCheckPass = true;
	vector<GCondition_Leaf*> vecFailConditionLeaf;

	if (!IEChecker.Check(pPlayer, pNPC, pIEInfo, bAutoCheckPass, vecFailConditionLeaf))
	{
		if (IT_QUEST_END != pIEInfo->m_nType && IT_QUEST_BEGIN != pIEInfo->m_nType)
			return NIT_NONE;
	}

	switch (pIEInfo->m_nType)
	{
	case IT_NONE:			return NIT_NONE;	
	case IT_CRAFT:			return NIT_CRAFT;
	case IT_GUILD_STORAGE:	return NIT_GUILD_CREATE;
	case IT_GUILD_DESTROY:	return NIT_GUILD_CREATE;
	case IT_NPCSHOP:		return NIT_SHOP;
	case IT_DIALOG:			return NIT_TALK;
	case IT_LOOT_IMMORTAL:
	case IT_LOOT_MORTAL:	return ClickIETypeToIconType_Loot(pPlayer, pIEInfo);
	case IT_QUEST_INTERACT_IMMORTAL:
	case IT_QUEST_INTERACT_MORTAL:
	case IT_QUEST_END:
	case IT_QUEST_BEGIN:	return ClickIETypeToIconType_Quest(pPlayer, pIEInfo, bAutoCheckPass, vecFailConditionLeaf);
	case IT_TRIGGER:		return NIT_TRIGGER;
	default:				return NIT_ONLYINTERACTION;
	}

	return NIT_NONE;	
}

NPC_ICON_TYPE GNPCIconSelector::ClickIETypeToIconType_Quest( GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIEInfo, bool bAutoCheckPass, const vector<GCondition_Leaf*>& vecFailConditionLeaf )
{
	if (NULL == pPlayer) return NIT_NONE;
	if (NULL == pIEInfo) return NIT_NONE;

	if (!bAutoCheckPass && IT_QUEST_END != pIEInfo->m_nType)
		return NIT_NONE;

	switch (pIEInfo->m_nType)
	{
	case IT_QUEST_INTERACT_IMMORTAL:
	case IT_QUEST_INTERACT_MORTAL:	return NIT_QUEST_INTERACT;	
	case IT_QUEST_BEGIN:
		{
			if (vecFailConditionLeaf.empty())
			{
				return NIT_QUEST_BEGIN;
			}
			else
			{
				bool bExistLevelCondition = false;
				for each (GCondition_Leaf* pFailConditionLeaf in vecFailConditionLeaf)
				{
					if (CT_LEVEL != pFailConditionLeaf->m_nCT) continue;

					bExistLevelCondition = true;
					int nMinLevel = pFailConditionLeaf->GetMinLevel_For_CT_LEVEL();
					int nLevel = pPlayer->GetLevel();

					if (nMinLevel <= nLevel + 2)
					{
						return NIT_QUEST_NOT_BEGIN_ONLYICON;
					}					

				}

				if (!bExistLevelCondition)
					return NIT_QUEST_NOT_BEGIN_ONLYICON;
			}
		}
		break;
	case IT_QUEST_END:
		{
			if (!vecFailConditionLeaf.empty()) return NIT_NONE;

			GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pIEInfo->GetSinlgeAct());
			if (NULL == pQuestInfo) return NIT_NONE;			

			if (pPlayer->GetQuests().IsCompleteQuest(pIEInfo->GetSinlgeAct()))
			{
				return NIT_QUEST_END;
			}
			else if (pPlayer->GetQuests().IsDoingQuest(pIEInfo->GetSinlgeAct()))
			{
				GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pIEInfo->GetSinlgeAct());
				if (!pQuestInfo) return NIT_NONE;

				if (pQuestInfo->nIncompleteDialogID)
					return NIT_QUEST_CONTINUE;
				else
					return NIT_QUEST_CONTINUE_ONLYICON;
			}
		}
		break;
	}

	return NIT_NONE;
}

NPC_ICON_TYPE GNPCIconSelector::ClickIETypeToIconType_Loot( GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIEInfo )
{
	GLootInfo* pLootInfo = gmgr.pLootInfoMgr->Get(pIEInfo->GetSinlgeAct());
	if (NULL == pLootInfo) return NIT_NONE;

	for each(LOOT_ITEM_INFO* pLootItemInfo in pLootInfo->m_vecLootItems)
	{
		if (true == pLootItemInfo->m_vecQuestID.empty())
		{
			return NIT_LOOT;
		}
		else
		{
			if (false == gmgr.pQuestInfoMgr->IsCollectItem(pLootItemInfo->m_vecQuestID, pLootItemInfo->m_nID))
			{
				return NIT_LOOT;
			}

			if (0 < pPlayer->GetQuests().GetQuestNeedItemQuantity(pLootItemInfo->m_vecQuestID, pLootItemInfo->m_nID))
			{
				return NIT_LOOT;
			}
		}
	}

	return NIT_NONE;
}

NPC_ICON_TYPE GNPCIconSelector::ItemIETypeToIconType( GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo*pIEInfo )
{
	if (NULL == pPlayer) return NIT_NONE;
	if (NULL == pNPC) return NIT_NONE;
	if (NULL == pIEInfo) return NIT_NONE;

	GNPCIEChecker IEChecker;
	if (!IEChecker.Check(pPlayer, pNPC, pIEInfo))
		return NIT_NONE;
	if (!pPlayer->GetItemHolder()->IsExist(SLOTTYPE_INVENTORY, pIEInfo->m_nUseItemID))
		return NIT_ITEM_UNUSABLE;

	return NIT_ITEM_USABLE;
}