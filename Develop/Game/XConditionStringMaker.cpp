#include "StdAfx.h"
#include "XConditionStringMaker.h"
#include "CSConditionInfo.h"
#include "XTalentInfoMgr.h"
#include "XItemManager.h"
#include "XStrings.h"
#include "XFactionInfo.h"
#include "XFactionInfoMgr.h"
#include "XInnInfo.h"
#include "XInnInfoMgr.h"
#include "XNPCInfo.h"
#include "CSFactionCalculator.h"

bool XConditionStringMaker::Check( CSCondition_Component* pCondition, wstring& strText)
{
	switch (pCondition->GetType())
	{
	case CCT_LEAF:
		{
			CSCondition_Leaf* pLeaf = dynamic_cast<CSCondition_Leaf*>(pCondition);
			if (pLeaf)
			{
				return CheckLeaf(pLeaf, strText);
			}
		}
		break;
	case CCT_COMPOSITE:
		{
			CSCondition_Composite* pComposite = dynamic_cast<CSCondition_Composite*>(pCondition);
			if (pComposite)
			{
				return CheckComposite(pComposite, strText);
			}
		}
		break;
	}
	return false;
}

bool XConditionStringMaker::CheckLeaf( CSCondition_Leaf* pCondition, wstring& strText )
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

bool XConditionStringMaker::CheckComposite( CSCondition_Composite* pCondition, wstring& strText)
{
	if (CAO_NONE == pCondition->m_nCAO) return false;

	bool bOnceResultTrue = false;
	wchar_t buff[128] = {0};

	for each (CSCondition_Component* pConditionComponent in pCondition->m_vecConditionComponent)
	{
		if (m_nCAO != pCondition->m_nCAO && CAO_AND == pCondition->m_nCAO)
		{
			swprintf_s(buff, 128, XStringResMgr::GetInstance()->GetString(L"CT_AND"));
			strText = strText + buff;
		}
		if (m_nCAO != pCondition->m_nCAO && CAO_OR == pCondition->m_nCAO)
		{
			swprintf_s(buff, 128, XStringResMgr::GetInstance()->GetString(L"CT_OR"));
			strText = strText + buff;
		}

		m_nCAO = pCondition->m_nCAO;

		bool bResult = Check(pConditionComponent, strText);
	}

	return false;
}

bool XConditionStringMaker::Level( CSCondition_Leaf* pCondition, wstring& strText )
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
		strBuff = FormatString(XGetStr(L"CLS_MINMAX"), FSParam(nMinLevel, nMaxLevel));		
	}

	strText = strText + strBuff;

	return true;
}

bool XConditionStringMaker::Quest( CSCondition_Leaf* pCondition, wstring& strText )
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
	case CQS_EXIST:			strBuff = XGetStr(L"CQS_EXIST");		break;
	case CQS_NOTCOMPLETE:	strBuff = XGetStr(L"CQS_NOTCOMPLETE");	break;
	case CQS_COMPLETE:		strBuff = XGetStr(L"CQS_COMPLETE");		break;
	case CQS_NOTDONE:		strBuff = XGetStr(L"CQS_NOTDONE");		break;
	case CQS_DONE:			strBuff = XGetStr(L"CQS_DONE");			break;
	case CQS_ACTIVE:		strBuff = XGetStr(L"CQS_ACTIVE");		break;
	}

	strText = FormatString(XGetStr(L"CQS_QUEST"), FSParam(pQuestInfo->GetTitle(), strBuff));

	return true;
}

bool XConditionStringMaker::QuestVar( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (pCondition->IsValid_For_CT_QUEST_VAR() == false) return false;

	int nQuestID = pCondition->GetQuestID_For_CT_QUEST_VAR();
	int nVar = pCondition->GetVar_For_CT_QUEST_VAR();

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CQS_VAR"), FSParam(pQuestInfo->GetTitle()));

	return true;

}


bool XConditionStringMaker::QuestObjective_Complete( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;	
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nQuestID = pCondition->m_vecParam1[0];
	int nQObjectiveID =pCondition-> m_vecParam2[0];

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CQS_OBJECTIVE_COMPLETE"), FSParam(pQuestInfo->GetTitle(), nQObjectiveID));

	return true;

}

bool XConditionStringMaker::QuestObjective_Incomplete( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;	
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nQuestID = pCondition->m_vecParam1[0];
	int nQObjectiveID = pCondition->m_vecParam2[0];

	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CQS_OBJECTIVE_INCOMPLETE"), FSParam(pQuestInfo->GetTitle(), nQObjectiveID));

	return true;
}

bool XConditionStringMaker::Item( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nItemID = pCondition->m_vecParam1[0];
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_ITEM"), FSParam(pItemData->GetName()));

	return true;
}

bool XConditionStringMaker::NoItem( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nItemID = pCondition->m_vecParam1[0];
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_NOITEM"), FSParam(pItemData->GetName()));

	return true;
}

bool XConditionStringMaker::FewItem( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	XItemData* pItemData = info.item->GetItemData(pCondition->m_vecParam1[0]);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_FEWITEM"), FSParam(pItemData->GetName(), pCondition->m_vecParam2[0]));

	return true;
}

bool XConditionStringMaker::ManyItem( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	XItemData* pItemData = info.item->GetItemData(pCondition->m_vecParam1[0]);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_MANYITEM"), FSParam(pItemData->GetName(), pCondition->m_vecParam2[0]));

	return true;

}

bool XConditionStringMaker::Equip( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	XItemData* pItemData = info.item->GetItemData(pCondition->m_vecParam1[0]);
	if(pItemData == NULL) return false;

	strText += FormatString(XGetStr(L"CIS_EQUIP"), FSParam(pItemData->GetName()));

	return true;

}

bool XConditionStringMaker::Race( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	RACE nRace = static_cast<RACE>(pCondition->m_vecParam1[0]);

	switch (nRace)
	{
	case RACE_HUMAN:	strText += XStringResMgr::GetInstance()->GetString(L"CRS_HUMAN");	break;
	case RACE_VOLO:		strText += XStringResMgr::GetInstance()->GetString(L"CRS_VOLO");		break;
	case RACE_NEBIT:	strText += XStringResMgr::GetInstance()->GetString(L"CRS_NEBIT");	break;
	default:	return false;
	}

	// 인간이 아니고 사람이여서 패쓰
	//strText += XStrings::Race(nRace);

	return true;

}

bool XConditionStringMaker::Sex( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	SEX nSex = static_cast<SEX>(pCondition->m_vecParam1[0]);
	wstring strSex;
	switch (nSex)
	{
	case SEX_MALE:		strSex = L"남성";	break;
	case SEX_FEMALE:	strSex = L"여성";	break;
	case SEX_SEXLESS:	strSex = L"무성";	break;
	default:	return false;
	}

	strText += FormatString(XGetStr(L"CSS_SEX"), FSParam(strSex));

	return true;

}

bool XConditionStringMaker::Date( CSCondition_Leaf* pCondition, wstring& strText )
{
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	int nCurMonthDay = stCurTime.wMonth * 100 + stCurTime.wDay;
	int nCurYearMonthDay = stCurTime.wYear * 10000 + nCurMonthDay;

	//if (false == pCondition->m_vecParam1.empty())
	//{
	//	if (10000 > pCondition->m_vecParam1[0])
	//	{
	//		if (nCurMonthDay < pCondition->m_vecParam1[0]) return false;					
	//	}
	//	else
	//	{
	//		if (nCurYearMonthDay < pCondition->m_vecParam1[0]) return false;
	//	}
	//}

	//if (false == pCondition->m_vecParam2.empty())
	//{
	//	if (10000 > pCondition->m_vecParam2[0])
	//	{
	//		if (nCurMonthDay > pCondition->m_vecParam2[0]) return false;					
	//	}
	//	else
	//	{
	//		if (nCurYearMonthDay > pCondition->m_vecParam2[0]) return false;
	//	}
	//}

	strText += FormatString(XGetStr(L"CT_DATE"), FSParam(stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay, pCondition->m_vecParam2[0]));

	return true;
}

bool XConditionStringMaker::Talent( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nTalentID = pCondition->m_vecParam1[0];
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (NULL == pTalentInfo) return false;

	strText += FormatString(XGetStr(L"CT_TALENT"), FSParam(pTalentInfo->GetName()));

	return true;
}

bool XConditionStringMaker::Talent_Style_TP( CSCondition_Leaf* pCondition, wstring& strText )
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

	strText += FormatString(XGetStr(L"CT_TALENT_STYLE_TP"), FSParam(style, nTP));

	return true;
}

bool XConditionStringMaker::Faction( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;
	if (true == pCondition->m_vecParam2.empty()) return false;

	int nConditionalFacitonID = pCondition->m_vecParam1[0];
	int nConditionalFactionValue = pCondition->m_vecParam2[0];

	XFactionInfo* pFactionInfo = info.faction->Get(nConditionalFacitonID);
	if (pFactionInfo == NULL) return false;

	FACTION_RELATION_TYPE nFRT = CSFactionCalculator::CalculRelation(nConditionalFactionValue);
	
	strText += FormatString(XGetStr(L"CFS_VALUE"), FSParam(pFactionInfo->GetTitle(), XStrings::FacionRelationTypeToString(nFRT)));

	return true;

}

// "CK_HAS"등 Data\system\Strings\interface.xml에 기술되어있음
bool XConditionStringMaker::Kill( CSCondition_Leaf* pCondition, wstring& strText )
{
	if (true == pCondition->m_vecParam1.empty()) return false;

	int nKillNPCID = pCondition->m_vecParam1[0];
	XNPCInfo* pNPCInfo = info.npc->Get(nKillNPCID);
	if (pNPCInfo == NULL) return false;

	strText += FormatString(XGetStr(L"CK_HAS"), FSParam(pNPCInfo->GetName()));

	return true;
}