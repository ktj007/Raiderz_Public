#include "StdAfx.h"
#include "GNPCShopInfoValidator.h"
#include "GNPCShopInfo.h"
#include "GNPCShopInfoMgr.h"
#include "GValidateLogger.h"
#include "GServerSystems.h"
#include "GDBManager.h"
#include "GDef.h"
#include "GItemManager.h"
#include "GConst.h"
#include "GMath.h"

#define MIN_BASE_MOD 0.5f
#define MAX_BASE_MOD 2.0f


bool GNPCShopInfoValidator::Check()
{
	if (gmgr.pNPCShopInfoMgr == NULL)	return true;

	_VLI(FILENAME_NPCSHOP);

	MAP_NPCSHOPINFO& mapNPCShopInfo = gmgr.pNPCShopInfoMgr->GetContainer();
	for each (pair<int, GNPCShopInfo*> pairNPCShopInfo in mapNPCShopInfo)
	{
		GNPCShopInfo* pNPCShopInfo = pairNPCShopInfo.second;
		_ASSERT(NULL != pNPCShopInfo);
		if (NULL == pNPCShopInfo) continue;

		CheckNPCShop(pNPCShopInfo);

		for (GItemManager::GItemDataMap::iterator itorItem = gmgr.pItemManager->Begin(); itorItem != gmgr.pItemManager->End(); ++itorItem)
		{
			if (true == CheckItemSellPriceIsHigherThanBuyPrice(pNPCShopInfo, itorItem->second))
			{
				_VLOGGER->Log(L"[TradeID = %d] 판매가격이 구매가격보다 높습니다. [%s(%d)]\n", pNPCShopInfo->m_nID, itorItem->second->m_strName.c_str(), itorItem->second->m_nID);
			}
		}
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GNPCShopInfoValidator::CheckNPCShop(GNPCShopInfo* pNPCShopInfo)
{
	VALID(pNPCShopInfo);

	_VLE(_T(TRADE_XML_TAG_NPCSHOP));
	_VLA(_T(TRADE_XML_ATTR_ID), pNPCShopInfo->m_nID);
	_VLP;

	_VLOGGER->ValidateMinusORZero(pNPCShopInfo->m_nID, _T(TRADE_XML_ATTR_ID));
	if (NULL == pNPCShopInfo->m_pDialogInfo)
	{
		_VLOGGER->Log(_T(TRADE_XML_ATTR_DIALOG) IS_NOT_IN_DIALOG);
	}

	_VLOGGER->ValidateMinusORZero(pNPCShopInfo->m_fBaseBuyMod, _T(TRADE_XML_ATTR_BUYMOD));
	_VLOGGER->ValidateMinusORZero(pNPCShopInfo->m_fBaseSellMod, _T(TRADE_XML_ATTR_SELLMOD));

	_VLOGGER->ValidateMinMax(_T(TRADE_XML_ATTR_BUYMOD), pNPCShopInfo->m_fBaseBuyMod, MIN_BASE_MOD, MAX_BASE_MOD);
	_VLOGGER->ValidateMinMax(_T(TRADE_XML_ATTR_SELLMOD), pNPCShopInfo->m_fBaseSellMod, MIN_BASE_MOD, MAX_BASE_MOD);
}

bool GNPCShopInfoValidator::CheckItemSellPriceIsHigherThanBuyPrice(GNPCShopInfo* pNPCShopInfo, GItemData* pItemData)
{
	if (0 == pItemData->m_nBaseBuyingPrice || 0 == pItemData->m_nBaseSellingPrice) return false;

	float fBuyMod = pNPCShopInfo->m_fBaseBuyMod;
	float fSellMod = pNPCShopInfo->m_fBaseSellMod;
	
	// 우호도가 Excellent일 때의 Mod 값 적용
	fBuyMod = fBuyMod * NPC_SHOP_BUY_MOD_FACTION_EXCELLENT;
	fSellMod = fSellMod * NPC_SHOP_SELL_MOD_FACTION_EXCELLENT;

	int nBuyPrice = (int)GMath::Round(pItemData->m_nBaseBuyingPrice * fBuyMod);
	int nSellPrice = (int)GMath::Round(pItemData->m_nBaseSellingPrice * fSellMod);

	if (nSellPrice > nBuyPrice) return true;

	return false;
}