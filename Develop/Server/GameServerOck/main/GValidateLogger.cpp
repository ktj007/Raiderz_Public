#include "stdafx.h"
#include "GValidateLogger.h"
#include "GItemManager.h"
#include "GTalentInfo.h"
#include "GConditionInfo.h"
#include "GLootInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GNPCShopInfoMgr.h"
#include "GNPCInfoMgr.h"
#include "GNPCInteractionInfo.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "SDef.h"
#include "GFieldInfoMgr.h"
#include "GSoulBindingInfoMgr.h"
#include "GBuffInfo.h"
#include "GConditionInfo.h"
#include "GConditionsInfoMgr.h"
#include "GFactionInfoMgr.h"
#include "GRecipeInfoMgr.h"

#define MAX_ITEM_ADDREMOVE_AMOUNT 100 // 100개 보다 많이 아이템을 지급하고 뺏으면 기획자가 실수한걸로 판단


GValidateLogger::PUSHANDPOPELEMENT::PUSHANDPOPELEMENT( const ELEMENT& element )
{
	gsys.pValidateLogger->PushElement(element);
}

GValidateLogger::PUSHANDPOPELEMENT::~PUSHANDPOPELEMENT()
{
	gsys.pValidateLogger->PopElement();
}

GValidateLogger::GValidateLogger() : CSValidateLogger()
{

}

GValidateLogger::~GValidateLogger()
{

}

void GValidateLogger::ValidateQuestID(int nID, const wchar_t* szName)
{
	if (false == gmgr.pQuestInfoMgr->IsExist(nID))
	{
		Log(PERCENT_S IS_NOT_IN_QUEST, szName);
	}
}

void GValidateLogger::ValidateItemID(int nID, const wchar_t* szName)
{
	if (false == gmgr.pItemManager->IsExist(nID))
	{
		Log(PERCENT_S L"(" PERCENT_D L")" IS_NOT_IN_ITEM, szName, nID);
	}
}

void GValidateLogger::ValidateNotTradableItemID( int nID, const wchar_t* szName )
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nID);
	if (NULL == pItemData) return;

	if (pItemData->m_bTradable)
	{
		Log(PERCENT_S IS TRADABLEITEM, szName);
	}
}

void GValidateLogger::ValidateTalentID(int nID, const wchar_t* szName)
{
	if (false == gmgr.pTalentInfoMgr->Exist(nID))
	{
		Log(PERCENT_S IS_NOT_IN_TALENT, szName);
	}
}

void GValidateLogger::ValidateLootID(int nID, const wchar_t* szName)
{
	if (NULL == gmgr.pLootInfoMgr->Get(nID))
	{
		Log(PERCENT_S IS_NOT_IN_LOOT, szName);
	}
}

void GValidateLogger::ValidateDialogID(int nID, const wchar_t* szName)
{
	if (NULL == gmgr.pDialogInfoMgr->Get(nID))
	{
		Log(PERCENT_S IS_NOT_IN_DIALOG, szName);
	}
}

void GValidateLogger::ValidateTradeID(int nID, const wchar_t* szName)
{
	if (NULL == gmgr.pNPCShopInfoMgr->Get(nID))
	{
		Log(PERCENT_S IS_NOT_IN_TRADE, szName);
	}
}

void GValidateLogger::ValidateNPCID(int nID, const wchar_t* szName)
{
	if (NULL == gmgr.pNPCInfoMgr->Find(nID))
	{
		Log(PERCENT_S IS_NOT_IN_NPC, szName);
	}
}

void GValidateLogger::ValidateFieldID( int nID, const wchar_t* szName )
{
	if (NULL == gmgr.pFieldInfoMgr->Find(nID))
	{
		Log(PERCENT_S IS_NOT_IN_FIELDLIST, szName);
	}
}

void GValidateLogger::ValidateSoulBindingID(SBID nID, const wchar_t* szName)
{
	if (NULL == gmgr.pSoulBindingInfoMgr->Get(nID))
	{
		Log(PERCENT_S IS_NOT_IN_SOULBINDING, szName);
	}
}

void GValidateLogger::ValidateSensorID(int nFieldID, int nSensorID, const wchar_t* szName)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (NULL != pFieldInfo)
	{
		if (NULL == pFieldInfo->FindSensor(nSensorID))
		{
			Log(PERCENT_S IS_NOT_IN_FIELEXT, szName);
		}
	}
}

void GValidateLogger::ValidateMarkerID(int nFieldID, int nMarkerID, const wchar_t* szName)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (NULL != pFieldInfo)
	{
		if (NULL == pFieldInfo->FindMarker(nMarkerID))
		{
			Log(PERCENT_S IS_NOT_IN_FIELEXT, szName);
		}
	}
}

void GValidateLogger::ValidateBuffID(int nID, const wchar_t* szName)
{
	if (false == gmgr.pBuffInfoMgr->Exist(nID))
	{
		Log(PERCENT_S IS_NOT_IN_BUFFID, szName);
	}
}

void GValidateLogger::ValidateConditionID( int nID, const wchar_t* szName )
{
	if (NULL == gmgr.pCondtionsInfoMgr->Get(nID))
	{
		Log(PERCENT_S IS_NOT_IN_CONDITIONS, szName);
	}
}

void GValidateLogger::ValidateFactionID( int nID, const wchar_t* szName )
{
	if (false == gmgr.pFactionInfoMgr->IsExist(nID))
	{
		Log(PERCENT_S IS_NOT_IN_FACTION, szName);
	}
}

void GValidateLogger::ValidateRecipeID(int nID, const wchar_t* szName)
{
	if (NULL == gmgr.pRecipeInfoMgr->Get(nID))
	{
		Log(PERCENT_S IS_NOT_IN_RECIPE, szName);
	}
}

void GValidateLogger::ValidateConditionQuestState( int nValue, const wchar_t* szName )
{
	if (CQS_NONE >= nValue || CQS_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void GValidateLogger::ValidateConditionType( int nValue, const wchar_t* szName )
{
	if (CT_NONE >= nValue || CT_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void GValidateLogger::ValidateQuestTrialType( int nValue, const wchar_t* szName )
{
	if (QTT_PARTY > nValue || QTT_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void GValidateLogger::ValidateIElementType( int nValue, const wchar_t* szName )
{
	if (IT_NONE >= nValue|| IT_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void GValidateLogger::ValidateDamageAttribType( int nValue, const wchar_t* szName )
{
	if (DA_NONE > nValue|| DA_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void GValidateLogger::ValidateQuestRewardType( int nValue, const wchar_t* szName )
{
	if (QRT_NONE >= nValue|| QRT_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void GValidateLogger::ValidateTNeedTalent(int TNeedTalent, const wchar_t* szName)
{
	if (TNeedTalent == 0)
	{
		return;
	}

	if (!gmgr.pTalentInfoMgr->Exist(TNeedTalent))
	{
		Log(PERCENT_S IS_NOT_IN_TALENT, szName);
	}
}

void GValidateLogger::ValidateSumTotal(int nSum, int nTotal, const wchar_t* szCollection, const wchar_t* szItem)
{
	if (nSum != nTotal)
	{
		Log(NOT_EQUAL_SUM_TOTAL, szCollection, szItem, nTotal);
	}
}

void GValidateLogger::LogNotEqualFormat(const wchar_t* szType1, const wchar_t* szType2)
{
	Log(NOT_EQUAL_FORMAT, szType1, szType2);
}


void GValidateLogger::LogMax(const wchar_t* szName, int nMax)
{
	Log(MUST_COUNT_IN_MAX, szName, nMax);
}

void GValidateLogger::LogFailedLoad(const wchar_t* szFileName)
{
	Log(FAILED_LOAD_FILE, szFileName);
}

void GValidateLogger::ValidateEnumType(const wchar_t* szName, const wchar_t* szValue)
{
	Log(UNKNOWN_ENUM_VALUE, szName, szValue);
}

void GValidateLogger::ValidateItemAddRemoveAmount(int nAmount, const wchar_t* szName )
{
	if (MAX_ITEM_ADDREMOVE_AMOUNT <= nAmount)
	{
		_VLOGGER->Log(PERCENT_S IS_TOO_BIG, szName);
	}
}

void GValidateLogger::ValidateItemMaxStackAmount(int nID, int nAmount, const wchar_t* szName)
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nID);
	if (NULL == pItemData) return;

	if (pItemData->m_nStackAmount < nAmount)
	{
		Log(PERCENT_S OVER_MAX_STACK_AMOUNT, szName);
	}	
}