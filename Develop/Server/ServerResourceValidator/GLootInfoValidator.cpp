#include "StdAfx.h"
#include "GLootInfoValidator.h"
#include "GServerSystems.h"
#include "GDef.h"
#include "GLootInfoMgr.h"
#include "GItemManager.h"
#include "GLootInfo.h"
#include "GValidateLogger.h"

#define MAX_DROPITEM_AMOUNT 20 
#define MAX_DROPMONEY_AMOUNT 1000000

bool GLootInfoValidator::Check()
{
	if (gmgr.pLootInfoMgr == NULL) return true;
	_VLI(FILENAME_LOOT_INFO);

	for (LOOT_INFO_MAP::iterator itor = gmgr.pLootInfoMgr->GetContainer().begin();
		itor != gmgr.pLootInfoMgr->GetContainer().end(); itor++)
	{
		GLootInfo* pLootInfo =(*itor).second;

		if (!pLootInfo->m_bValidation)
		{
			continue;
		}

		CheckLoot(pLootInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GLootInfoValidator::CheckLoot(GLootInfo* pLootInfo)
{
	_VLE(_T(LOOT_XML_TAG_LOOT));	
	_VLA(_T(LOOT_XML_ATTR_ID), pLootInfo->m_nID);
	_VLP;
	
	_VLOGGER->ValidateID(pLootInfo->m_nID);
	_VLOGGER->ValidateMinusORZero(pLootInfo->m_nLimitDropQuantity, _T(LOOT_XML_ATTR_MAX));

	for each (LOOT_ITEM_INFO* pLooItemInfo in pLootInfo->m_vecLootItems)
	{
		if (NULL == pLooItemInfo) continue;
		CheckItem(pLooItemInfo);
	}

	for each (LOOT_MONEY_INFO* pLooMoneyInfo in pLootInfo->m_vecLootMoneys)
	{
		if (NULL == pLooMoneyInfo) continue;
		CheckMoney(pLooMoneyInfo);
	}	
}

void GLootInfoValidator::CheckItem(LOOT_ITEM_INFO* pLooItemInfo)
{
	if (NULL == pLooItemInfo) return;

	_VLE(_T(LOOT_XML_TAG_ITEM));
	_VLA(_T(LOOT_XML_ATTR_ID), pLooItemInfo->m_nID);
	_VLA(_T(LOOT_XML_ATTR_MIN), pLooItemInfo->m_nMin);
	_VLA(_T(LOOT_XML_ATTR_MAX), pLooItemInfo->m_nMax);
	_VLA(_T(LOOT_XML_ATTR_RATE), static_cast<int>(pLooItemInfo->m_fRate));
	_VLP;

	_VLOGGER->ValidateItemID(pLooItemInfo->m_nID, _T(LOOT_XML_ATTR_ID));
	_VLOGGER->ValidateMinus(pLooItemInfo->m_nMin, _T(LOOT_XML_ATTR_MIN));
	_VLOGGER->ValidateMinus(pLooItemInfo->m_nMax, _T(LOOT_XML_ATTR_MAX));
	_VLOGGER->ValidateMinus(pLooItemInfo->m_fRate, _T(LOOT_XML_ATTR_RATE));	
	_VLOGGER->ValidateBiggerThan(pLooItemInfo->m_nMin, pLooItemInfo->m_nMax, _T(LOOT_XML_ATTR_MIN), _T(LOOT_XML_ATTR_MAX));
	_VLOGGER->ValidateTooBig(pLooItemInfo->m_nMax, MAX_DROPITEM_AMOUNT, _T(LOOT_XML_ATTR_MAX));

	GItemData* pItem = gmgr.pItemManager->GetItemData(pLooItemInfo->m_nID);
	if (NULL == pItem) return;

	_VLOGGER->ValidateBiggerThan(pLooItemInfo->m_nMax, pItem->m_nStackAmount, _T(LOOT_XML_ATTR_MAX), L"MaxStackAmount");
}

void GLootInfoValidator::CheckMoney(LOOT_MONEY_INFO* pLootMoneyInfo)
{
	if (NULL == pLootMoneyInfo) return;

	_VLE(_T(LOOT_XML_TAG_MONEY));
	_VLA(_T(LOOT_XML_ATTR_MIN), pLootMoneyInfo->m_nMin);
	_VLA(_T(LOOT_XML_ATTR_MAX), pLootMoneyInfo->m_nMax);
	_VLA(_T(LOOT_XML_ATTR_RATE), static_cast<int>(pLootMoneyInfo->m_fRate));
	_VLP;

	_VLOGGER->ValidateMinus(pLootMoneyInfo->m_nMin, _T(LOOT_XML_ATTR_MIN));
	_VLOGGER->ValidateMinus(pLootMoneyInfo->m_nMax, _T(LOOT_XML_ATTR_MAX));
	_VLOGGER->ValidateMinus(pLootMoneyInfo->m_fRate, _T(LOOT_XML_ATTR_RATE));
	_VLOGGER->ValidateBiggerThan(pLootMoneyInfo->m_nMin, pLootMoneyInfo->m_nMax, _T(LOOT_XML_ATTR_MIN), _T(LOOT_XML_ATTR_MAX));
	_VLOGGER->ValidateTooBig(pLootMoneyInfo->m_nMax, MAX_DROPMONEY_AMOUNT, _T(LOOT_XML_ATTR_MAX));
}