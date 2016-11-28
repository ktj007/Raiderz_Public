#include "stdafx.h"
#include "GConditionChecker.h"
#include "GConditionInfo.h"
#include "GEntityPlayer.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GGlobal.h"
#include "GPartySystem.h"
#include "GPlayerQObjective.h"
#include "GTalentInfoMgr.h"
#include "GPlayerTalent.h"
#include "GPlayerFactions.h"
#include "GItemHolder.h"
#include "GPlayerEmblem.h"
#include "GPlayerPVPArea.h"
#include "CSDef_QuestPVP.h"
#include "GEquipmentSlot.h"

//////////////////////////////////////////////////////////////////////////
bool GConditionChecker::Check( CSCondition_Component* pCondition, GEntityPlayer* pPlayer, vector<GCondition_Leaf*>& vecFailConditionLeaf)
{
	_ASSERT(NULL != pPlayer);	
	if (NULL == pPlayer) return false;

	switch (pCondition->GetType())
	{
	case CCT_LEAF:
		{
			GCondition_Leaf* pLeaf = dynamic_cast<GCondition_Leaf*>(pCondition);
			if (pLeaf)
			{
				if (false == CheckLeaf(pLeaf, pPlayer))
				{
					vecFailConditionLeaf.push_back(pLeaf);
					return false;
				}
				else
				{
					return true;
				}				
			}
		}
		break;
	case CCT_COMPOSITE:
		{
			GCondition_Composite* pComposite = dynamic_cast<GCondition_Composite*>(pCondition);
			if (pComposite)
			{
				vector<GCondition_Leaf*> vecCompositFailConditionLeaf;
				if (false == CheckComposite(pComposite, pPlayer, vecCompositFailConditionLeaf))
				{
					vecFailConditionLeaf.insert(vecFailConditionLeaf.end(), vecCompositFailConditionLeaf.begin(), vecCompositFailConditionLeaf.end());
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		break;
	}

	return false;
}

bool GConditionChecker::CheckLeaf( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	bool bCheckPass = false;

	switch (pCondition->m_nCT)
	{
	case CT_LEVEL:						bCheckPass = Level(pCondition, pPlayer); break;
	case CT_QUEST:						bCheckPass = Quest(pCondition, pPlayer); break;
	case CT_QUEST_VAR:					bCheckPass = QuestVar(pCondition, pPlayer); break;
	case CT_QUEST_OBJECTIVE_COMPLETE:	bCheckPass = QuestObjective_Complete(pCondition, pPlayer); break;
	case CT_QUEST_OBJECTIVE_INCOMPLETE:	bCheckPass = QuestObjective_Incomplete(pCondition, pPlayer); break;
	case CT_ITEM:						bCheckPass = Item(pCondition, pPlayer); break;
	case CT_NOITEM:						bCheckPass = NoItem(pCondition, pPlayer); break;
	case CT_FEWITEM:					bCheckPass = FewItem(pCondition, pPlayer); break;
	case CT_MANYITEM:					bCheckPass = ManyItem(pCondition, pPlayer); break;
	case CT_EQUIP:						bCheckPass = Equip(pCondition, pPlayer); break;
	case CT_RACE:						bCheckPass = Race(pCondition, pPlayer); break;
	case CT_SEX:						bCheckPass = Sex(pCondition, pPlayer); break;
	case CT_DATE:						bCheckPass = Date(pCondition, pPlayer); break;
	case CT_TALENT:						bCheckPass = Talent(pCondition, pPlayer); break;
	case CT_TALENT_STYLE_TP:			bCheckPass = Talent_Style_TP(pCondition, pPlayer); break;
	case CT_FACTION:					bCheckPass = Faction(pCondition, pPlayer); break;
	case CT_KILL:						bCheckPass = Kill(pCondition, pPlayer); break;
	case CT_QPVP_TEAM:					bCheckPass = QPVP_Team(pCondition, pPlayer); break;
	}

	return bCheckPass;
}

bool GConditionChecker::CheckComposite( GCondition_Composite* pCondition, GEntityPlayer* pPlayer, vector<GCondition_Leaf*>& vecFailConditionLeaf )
{
	if (CAO_NONE == pCondition->m_nCAO) return false;

	bool bOnceResultTrue = false;

	for each (CSCondition_Component* pConditionComponent in pCondition->m_vecConditionComponent)
	{
		bool bResult = Check(pConditionComponent, pPlayer, vecFailConditionLeaf);

		if (CAO_AND == pCondition->m_nCAO && false == bResult)
		{
			return false;
		}

		if (CAO_OR == pCondition->m_nCAO && true == bResult)
		{
			return true;
		}
	}

	if (CAO_AND == pCondition->m_nCAO)
	{
		return true;
	}

	if (CAO_OR == pCondition->m_nCAO)
	{
		return false;
	}

	return false;
}

bool GConditionChecker::Level( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (pCondition->IsValid_For_CT_LEVEL() == false) return false;

	int nMinLevel= pCondition->GetMinLevel_For_CT_LEVEL();
	int nMaxLevel= pCondition->GetMaxLevel_For_CT_LEVEL();
	int nLevel = pPlayer->GetLevel();

	if (nLevel < nMinLevel) return false;
	if (nLevel > nMaxLevel) return false;

	return true;

}

bool GConditionChecker::Quest( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (pCondition->IsValid_For_CT_QUEST() == false) return false;

	int nQuestID = pCondition->GetQuestID_For_CT_QUEST();
	CONDITION_QUEST_STATE nQuestState = pCondition->GetQuestState_For_CT_QUEST();

	switch (nQuestState)
	{
	case CQS_NOTEXIST:
		{
			if (true == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return false;
		}
		break;
	case CQS_EXIST:
		{
			if (false == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return false;
		}
		break;
	case CQS_NOTCOMPLETE:
		{
			GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
			if (NULL == pPlayerQuest)					return false;
			if (true == pPlayerQuest->IsComplete())		return false;
		}
		break;
	case CQS_COMPLETE:
		{
			GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
			if (NULL == pPlayerQuest)					return false;
			if (false == pPlayerQuest->IsComplete())	return false;
		}
		break;
	case CQS_NOTDONE:
		{
			if (true == pPlayer->GetQuests().IsDoneQuest(nQuestID)) return false;
		}
		break;
	case CQS_DONE:
		{
			if (false == pPlayer->GetQuests().IsDoneQuest(nQuestID)) return false;
		}
		break;
	case CQS_ACTIVE:
		{
			// Check Party Member's Questinfo
			if (pPlayer->HasParty())
			{
				MUID uidParty = pPlayer->GetPartyUID();
				GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
				if (pParty != NULL)
				{
					if (pParty->HasQuest(nQuestID))
					{
						return true;
					}
				}
			}

			// Check myself
			GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
			if (NULL == pPlayerQuest)		return false;
			if (!pPlayerQuest->IsActive())	return false;
		}
		break;
	}

	return true;

}

bool GConditionChecker::QuestVar( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (pCondition->IsValid_For_CT_QUEST_VAR() == false) return false;

	int nQuestID = pCondition->GetQuestID_For_CT_QUEST_VAR();
	int nVar = pCondition->GetVar_For_CT_QUEST_VAR();

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return false;

	if (nVar != pPlayerQuest->GetQuestVar()) return false;

	return true;

}

bool GConditionChecker::QuestObjective_Complete( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;	
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nQuestID = pCondition->m_vecParam1[0];
	int nQObjectiveID =pCondition-> m_vecParam2[0];

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return false;
	GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(nQObjectiveID);
	if (NULL == pPlayerQObjective) return false;

	if (false == pPlayerQObjective->IsComplete()) return false;

	return true;

}

bool GConditionChecker::QuestObjective_Incomplete( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;	
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nQuestID = pCondition->m_vecParam1[0];
	int nQObjectiveID = pCondition->m_vecParam2[0];

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return false;
	GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(nQObjectiveID);
	if (NULL == pPlayerQObjective) return false;

	if (true == pPlayerQObjective->IsComplete()) return false;

	return true;
}

bool GConditionChecker::Item( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;

	for each (int nItemID in pCondition->m_vecParam1)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();		
		if (pItemHolder->IsExist(SLOTTYPE_EQUIP, nItemID) ||
			pItemHolder->IsExist(SLOTTYPE_INVENTORY, nItemID))
			return true;		
	}

	return false;
}

bool GConditionChecker::NoItem( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	return !Item(pCondition, pPlayer);
}

bool GConditionChecker::FewItem( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	int nEquipQuantity = pItemHolder->GetItemCount(SLOTTYPE_EQUIP, pCondition->m_vecParam1[0]);
	int nInvenQuantity = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pCondition->m_vecParam1[0]);
	if (nEquipQuantity + nInvenQuantity >= pCondition->m_vecParam2[0])
	{
		return false;
	}

	return true;
}

bool GConditionChecker::ManyItem( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	int nEquipQuantity = pItemHolder->GetItemCount(SLOTTYPE_EQUIP, pCondition->m_vecParam1[0]);
	int nInvenQuantity = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pCondition->m_vecParam1[0]);	
	if (nEquipQuantity + nInvenQuantity <= pCondition->m_vecParam2[0])
	{
		return false;
	}

	return true;

}

bool GConditionChecker::Equip( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	for each (int nQuestID in pCondition->m_vecParam1)
	{
		if (false == pItemHolder->GetEquipment().IsEquipItem(nQuestID)) return false;
	}

	return true;

}

bool GConditionChecker::Race( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;

	RACE nRace = static_cast<RACE>(pCondition->m_vecParam1[0]);

	if (nRace != pPlayer->GetPlayerInfo()->nRace) return false;

	return true;

}

bool GConditionChecker::Sex( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;

	SEX nSex = static_cast<SEX>(pCondition->m_vecParam1[0]);

	if (nSex != pPlayer->GetPlayerInfo()->nSex) return false;

	return true;

}

bool GConditionChecker::Date( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;

	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	int nCurMonthDay = stCurTime.wMonth * 100 + stCurTime.wDay;
	int nCurYearMonthDay = stCurTime.wYear * 10000 + nCurMonthDay;

	if (false == pCondition->m_vecParam1.empty())
	{
		if (10000 > pCondition->m_vecParam1[0])
		{
			if (nCurMonthDay < pCondition->m_vecParam1[0]) return false;					
		}
		else
		{
			if (nCurYearMonthDay < pCondition->m_vecParam1[0]) return false;
		}
	}

	if (false == pCondition->m_vecParam2.empty())
	{
		if (10000 > pCondition->m_vecParam2[0])
		{
			if (nCurMonthDay > pCondition->m_vecParam2[0]) return false;					
		}
		else
		{
			if (nCurYearMonthDay > pCondition->m_vecParam2[0]) return false;
		}
	}

	return true;

}

bool GConditionChecker::Talent( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;

	for each (int nTalentID in pCondition->m_vecParam1)
	{
		GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
		if (NULL == pTalentInfo) continue;

		if (false == pPlayer->GetTalent().IsTrainedTalent(pTalentInfo)) return false;
	}

	return true;

}

bool GConditionChecker::Talent_Style_TP( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	TALENT_STYLE nTalentStyle = static_cast<TALENT_STYLE>(pCondition->m_vecParam1[0]);
	int nTP = pCondition->m_vecParam2[0];

	if (nTP > pPlayer->GetTalent().GetTrainedStyleTP(nTalentStyle)) return false;

	return true;

}

bool GConditionChecker::Faction( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nConditionalFacitonID = pCondition->m_vecParam1[0];
	int nConditionalFactionValue = pCondition->m_vecParam2[0];

	int nCurPlayerFaction = pPlayer->GetPlayerFactions().GetQuantity(nConditionalFacitonID);

	if (nCurPlayerFaction < nConditionalFactionValue) return false;

	return true;

}

bool GConditionChecker::Kill( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nKillNPCID = pCondition->m_vecParam1[0];
	return pPlayer->GetEmblem().HasKilled(nKillNPCID);
}

bool GConditionChecker::QPVP_Team( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == pCondition->m_vecParam1.empty()) return false;
	
	if (1 == pCondition->m_vecParam1.front() && QT_ONE == pPlayer->GetPlayerPVPArea().GetTeam())
	{
		return true;
	}
	else if (2 == pCondition->m_vecParam1.front() && QT_TWO == pPlayer->GetPlayerPVPArea().GetTeam())
	{
		return true;
	}

	return false;
}
