#include "StdAfx.h"
#include "GTradeValidator.h"
#include "GTradeInfoMgr.h"
#include "GTradeInfo.h"

bool GTradeInfoValidator::Check()
{
	m_ValidateLogger.m_strFileName = FILENAME_TRADE;

	MAP_TRADEINFO& mapTradeInfo = gmgr.pTradeInfoMgr->GetContainer();
	for each (pair<int, GTradeInfo*> pairTradeInfo in mapTradeInfo)
	{
		GTradeInfo* pTradeInfo = pairTradeInfo.second;
		_ASSERT(NULL != pTradeInfo);
		if (NULL == pTradeInfo) continue;

		CheckTrade(pTradeInfo);
	}

	return true;
}

void GTradeInfoValidator::CheckTrade(GTradeInfo* pTradeInfo)
{
	_ASSERT(NULL != pTradeInfo);
	if (NULL == pTradeInfo) return;

	char szNumber[512];
	_itoa_s(pTradeInfo->m_nID, szNumber, 512, 10);

	_VLE(TRADE_XML_TAG_TRADE);
	_VLA(TRADE_XML_ATTR_ID, szNumber);
	_VLP(&m_ValidateLogger);

	m_ValidateLogger.ValidateMinusORZero(pTradeInfo->m_nID, TRADE_XML_ATTR_ID);
	if (NULL == pTradeInfo->m_pDialogInfo)
	{
		m_ValidateLogger.Log(TRADE_XML_ATTR_DIALOG IS_NOT_IN_DIALOG);
	}

	m_ValidateLogger.ValidateMinusORZero(pTradeInfo->m_fBuyMod, TRADE_XML_ATTR_BUYMOD);
	m_ValidateLogger.ValidateMinusORZero(pTradeInfo->m_fSellMod, TRADE_XML_ATTR_SELLMOD);
	m_ValidateLogger.ValidateMinus(pTradeInfo->m_Repair.m_fRate, TRADE_XML_ATTR_REPAIR_RATE);

	if (false == pTradeInfo->m_Repair.m_vecAbleType.empty())
	{
		m_ValidateLogger.ValidateMinus(pTradeInfo->m_Repair.m_fRate, TRADE_XML_ATTR_REPAIR_RATE);
		m_ValidateLogger.ValidateEmptyText(pTradeInfo->m_Repair.m_strSuccessText, TRADE_XML_ATTR_REPAIR_SUCCESS_TEXT);
		m_ValidateLogger.ValidateEmptyText(pTradeInfo->m_Repair.m_strFailText, TRADE_XML_ATTR_REPAIR_FAIL_TEXT);
	}
}

