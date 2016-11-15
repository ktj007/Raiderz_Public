#include "StdAfx.h"
#include "XConditionHelper.h"
#include "XItemManager.h"
#include "XStrings.h"
#include "XFactionInfo.h"
#include "XFactionInfoMgr.h"
#include "XInnInfo.h"
#include "XInnInfoMgr.h"
#include "XNPCInfo.h"
#include "CSFactionCalculator.h"

bool XConditionHelper::CheckItemCondition( CSCondition_Leaf* pCondition, wstring& strText )
{
	switch (pCondition->m_nCT)
	{
	case CT_LEVEL:						return Level(pCondition, strText);
	case CT_QUEST:						return Quest(pCondition, strText);
	case CT_QUEST_VAR:					return QuestVar(pCondition, strText);
	case CT_QUEST_OBJECTIVE_COMPLETE:	return QuestObjective_Complete(pCondition, strText);
	case CT_QUEST_OBJECTIVE_INCOMPLETE:	return QuestObjective_Incomplete(pCondition, strText);
	case CT_ITEM:						return Item(pCondition, strText);
	case CT_NOITEM:						return NoItem(pCondition, strText);
	case CT_FEWITEM:					return FewItem(pCondition, strText);
	case CT_MANYITEM:					return ManyItem(pCondition, strText);
	case CT_EQUIP:						return Equip(pCondition, strText);
	case CT_RACE:						return Race(pCondition, strText);
	case CT_SEX:						return Sex(pCondition, strText);
	case CT_DATE:						return Date(pCondition, strText);
	case CT_TALENT:						return Talent(pCondition, strText);
	case CT_TALENT_STYLE_TP:			return Talent_Style_TP(pCondition, strText);
	case CT_FACTION:					return Faction(pCondition, strText);
	case CT_KILL:						return Kill(pCondition, strText);
	}

	return false;
}

bool XConditionHelper::Level( CSCondition_Leaf* pCondition, wstring& strText)
{
	if (pCondition->IsValid_For_CT_LEVEL() == false) return false;

	int nMinLevel= pCondition->GetMinLevel_For_CT_LEVEL();
	int nMaxLevel= pCondition->GetMaxLevel_For_CT_LEVEL();

	wstring strBuff;

	if (pCondition->m_vecParam1.empty() == true && pCondition->m_vecParam2.empty() == false)
	{
		strBuff = FormatString(XGetStr(L"CLS_MIN"), FSParam(nMaxLevel));
	}
	else if (pCondition->m_vecParam1.empty() == false && pCondition->m_vecParam2.empty() == true)
	{
		strBuff = FormatString(XGetStr(L"CLS_MAX"), FSParam(nMinLevel));
	}
	else
	{
		strBuff = FormatString(XGetStr(L"REQUIREDLEVELRANGE"), FSParam(nMinLevel, nMaxLevel));
	}

	strText = FormatString(XGetStr(L"CLS_ITEMUSEFAIL"), strBuff);

	return true;
}

bool XConditionHelper::Quest( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (pCondition->IsValid_For_CT_QUEST() == false) return false;

	int nQuestID = pCondition->GetQuestID_For_CT_QUEST();
	CONDITION_QUEST_STATE nQuestState = pCondition->GetQuestState_For_CT_QUEST();

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	wstring strBuff;

	switch (nQuestState)
	{
	case CQS_NOTEXIST:		strBuff = XGetStr(L"CQS_NOTEXIST");		break;
	case CQS_EXIST:			strBuff = XGetStr(L"CQS_EXIST_EXT");	break;
	case CQS_NOTCOMPLETE:	strBuff = XGetStr(L"CQS_NOTCOMPLETE");	break;
	case CQS_COMPLETE:		strBuff = XGetStr(L"CQS_COMPLETE_EXT");	break;
	case CQS_NOTDONE:		strBuff = XGetStr(L"CQS_NOTDONE");		break;
	case CQS_DONE:			strBuff = XGetStr(L"CQS_DONE_EXT");		break;
	case CQS_ACTIVE:		strBuff = XGetStr(L"CQF_ACTIVE");		break;
	}

	strText = FormatString(XGetStr(L"CQS_ITEMUSEFAIL"), FSParam(pQuestInfo->GetTitle(), strBuff));

	return true;
}

bool XConditionHelper::QuestVar( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (pCondition->IsValid_For_CT_QUEST_VAR() == false) return false;

	int nQuestID = pCondition->GetQuestID_For_CT_QUEST_VAR();
	int nVar = pCondition->GetVar_For_CT_QUEST_VAR();

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CQS_VAR_ITEMUSEFAIL"), FSParam(pQuestInfo->GetTitle()));

	return true;
}

bool XConditionHelper::QuestObjective_Complete( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;	
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nQuestID = pCondition->m_vecParam1[0];
	int nQObjectiveID =pCondition-> m_vecParam2[0];

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CQS_OBJECTIVE_COMPLETE_ITEMUSEFAIL"), FSParam(pQuestInfo->GetTitle(), nQObjectiveID));

	return true;
}

bool XConditionHelper::QuestObjective_Incomplete( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;	
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nQuestID = pCondition->m_vecParam1[0];
	int nQObjectiveID = pCondition->m_vecParam2[0];

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CQS_OBJECTIVE_INCOMPLETE_ITEMUSEFAIL"), FSParam(pQuestInfo->GetTitle(), nQObjectiveID));

	return true;
}

bool XConditionHelper::Item( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nItemID = pCondition->m_vecParam1[0];
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_ITEM_USEFAIL"), FSParam(pItemData->GetName()));

	return true;
}

bool XConditionHelper::NoItem( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nItemID = pCondition->m_vecParam1[0];
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_NOITEM_USEFAIL"), FSParam(pItemData->GetName()));

	return true;
}

bool XConditionHelper::FewItem( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	XItemData* pItemData = info.item->GetItemData(pCondition->m_vecParam1[0]);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_FEWITEM_USEFAIL"), FSParam(pItemData->GetName(), pCondition->m_vecParam2[0]));

	return true;
}

bool XConditionHelper::ManyItem( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	XItemData* pItemData = info.item->GetItemData(pCondition->m_vecParam1[0]);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_MANYITEM_USEFAIL"), FSParam(pItemData->GetName(), pCondition->m_vecParam2[0]));

	return true;
}

bool XConditionHelper::Equip( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	XItemData* pItemData = info.item->GetItemData(pCondition->m_vecParam1[0]);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_EQUIP_USEFAIL"), FSParam(pItemData->GetName()));

	return true;
}

bool XConditionHelper::Race( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	RACE nRace = static_cast<RACE>(pCondition->m_vecParam1[0]);

	switch (nRace)
	{
	case RACE_HUMAN:	strText += XStringResMgr::GetInstance()->GetString(L"CRS_HUMAN_USEFAIL");	break;
	case RACE_VOLO:		strText += XStringResMgr::GetInstance()->GetString(L"CRS_VOLO_USEFAIL");	break;
	case RACE_NEBIT:	strText += XStringResMgr::GetInstance()->GetString(L"CRS_NEBIT_USEFAIL");	break;
	default:	return false;
	}

	return true;
}

bool XConditionHelper::Sex( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	SEX nSex = static_cast<SEX>(pCondition->m_vecParam1[0]);
	wstring strSex;
	switch (nSex)
	{
	case SEX_MALE:		strText += XStringResMgr::GetInstance()->GetString(L"CSS_SEX_MALE_USEFAIL");	break;
	case SEX_FEMALE:	strText += XStringResMgr::GetInstance()->GetString(L"CSS_SEX_FEMALE_USEFAIL");	break;
	case SEX_SEXLESS:	strText += XStringResMgr::GetInstance()->GetString(L"CSS_SEX_SEXLESS_USEFAIL");	break;
	default:	return false;
	}

	return true;
}

bool XConditionHelper::Date( CSCondition_Leaf* pCondition, wstring& strText )
{
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	int nCurMonthDay = stCurTime.wMonth * 100 + stCurTime.wDay;
	int nCurYearMonthDay = stCurTime.wYear * 10000 + nCurMonthDay;

	strText += FormatString(XGetStr(L"CT_DATE_USEFAIL"), FSParam(stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay, pCondition->m_vecParam2[0]));

	return true;
}

bool XConditionHelper::Talent( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nTalentID = pCondition->m_vecParam1[0];
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (NULL == pTalentInfo) return false;

	strText += FormatString(XGetStr(L"CT_TALENT_USEFAIL"), FSParam(pTalentInfo->GetName()));

	return true;
}

bool XConditionHelper::Talent_Style_TP( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	TALENT_STYLE nTalentStyle = static_cast<TALENT_STYLE>(pCondition->m_vecParam1[0]);
	int nTP = pCondition->m_vecParam2[0];

	wstring style;
	switch (nTalentStyle)
	{
	case TS_DEFENDER:	style = XGetStr(L"TALENT_STYLE_DEFENDER");	break;
	case TS_BERSERKER:	style = XGetStr(L"TALENT_STYLE_BESERKER");	break;
	case TS_ASSASSIN:	style = XGetStr(L"TALENT_STYLE_ASSASSIN");	break;
	case TS_RANGER:		style = XGetStr(L"TALENT_STYLE_RANGER");	break;
	case TS_SORCERER:	style = XGetStr(L"TALENT_STYLE_SORCERER");	break;
	case TS_CLERIC:		style = XGetStr(L"TALENT_STYLE_CLERIC");	break;
	default:	return false;
	}

	strText += FormatString(XGetStr(L"CT_TALENT_STYLE_TP_USEFAIL"), FSParam(style, nTP));

	return true;
}

bool XConditionHelper::Faction( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nConditionalFacitonID = pCondition->m_vecParam1[0];
	int nConditionalFactionValue = pCondition->m_vecParam2[0];

	XFactionInfo* pFactionInfo = info.faction->Get(nConditionalFacitonID);
	if (pFactionInfo == NULL) return false;

	FACTION_RELATION_TYPE nFRT = CSFactionCalculator::CalculRelation(nConditionalFactionValue);

	strText += FormatString(XGetStr(L"CFS_VALUE_USEFAIL"), FSParam(pFactionInfo->GetTitle(), XStrings::FacionRelationTypeToString(nFRT)));

	return true;
}

bool XConditionHelper::Kill( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nKillNPCID = pCondition->m_vecParam1[0];
	XNPCInfo* pNPCInfo = info.npc->Get(nKillNPCID);
	if (pNPCInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CK_HAS_USEFAIL"), FSParam(pNPCInfo->GetName()));

	return true;
}
