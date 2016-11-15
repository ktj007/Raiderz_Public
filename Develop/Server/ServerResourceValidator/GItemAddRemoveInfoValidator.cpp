#include "StdAfx.h"
#include "GItemAddRemoveInfoValidator.h"
#include "GItemAddRemoveInfo.h"
#include "SDef.h"
#include "GItemAddRemoveInfoParser.h"
#include "GValidateLogger.h"
#include "GGlobal.h"

#define MAX_ITEM_ADDREMOVE_AMOUNT 100 // 100개이상 아이템을 지급하고 뺏으면 기획자가 실수한걸로 판단

void GItemAddRemoveInfoValidator::Check(GItemAddRemoveInfo* pItemInfo)
{
	VALID(pItemInfo);	

	const vector<GItemFillInfo*>& vecFillInfo = pItemInfo->GetItemFillInfo();
	for each (GItemFillInfo* each in vecFillInfo)
	{
		_VLE(_T(ITEM_TAG_FILL));
		_VLP;

		_VLOGGER->ValidateItemID(each->GetID(), _T(ITEM_ATTR_ID));
		_VLOGGER->ValidateNotTradableItemID(each->GetID(), _T(ITEM_ATTR_ID));
		_VLOGGER->ValidateMinusORZero(each->GetAmount(), _T(ITEM_ATTR_AMOUNT));

		_VLOGGER->ValidateItemAddRemoveAmount(each->GetAmount(), _T(ITEM_ATTR_AMOUNT));
		_VLOGGER->ValidateItemMaxStackAmount(each->GetID(), each->GetAmount(), _T(ITEM_ATTR_AMOUNT));
	}

	const vector<GItemAddInfo*>& vecAddInfo = pItemInfo->GetItemAddInfo();
	for each (GItemAddInfo* each in vecAddInfo)
	{
		_VLE(_T(ITEM_TAG_ADD));
		_VLP;

		_VLOGGER->ValidateItemID(each->GetID(), _T(ITEM_ATTR_ID));
		_VLOGGER->ValidateNotTradableItemID(each->GetID(), _T(ITEM_ATTR_ID));
		_VLOGGER->ValidateMinusORZero(each->GetAmount(), _T(ITEM_ATTR_AMOUNT));

		_VLOGGER->ValidateItemAddRemoveAmount(each->GetAmount(), _T(ITEM_ATTR_AMOUNT));
		_VLOGGER->ValidateItemMaxStackAmount(each->GetID(), each->GetAmount(), _T(ITEM_ATTR_AMOUNT));
	}

	const vector<GItemRemoveInfo*>& vecRemoveInfo = pItemInfo->GetItemRemoveInfo();
	for each (GItemRemoveInfo* each in vecRemoveInfo)
	{
		_VLE(_T(ITEM_TAG_REMOVE));
		_VLP;

		_VLOGGER->ValidateItemID(each->GetID(), _T(ITEM_ATTR_ID));
		_VLOGGER->ValidateMinusORZero(each->GetAmount(), _T(ITEM_ATTR_AMOUNT));

		_VLOGGER->ValidateItemAddRemoveAmount(each->GetAmount(), _T(ITEM_ATTR_AMOUNT));
		_VLOGGER->ValidateItemMaxStackAmount(each->GetID(), each->GetAmount(), _T(ITEM_ATTR_AMOUNT));
	}

	const vector<GItemRemoveAllInfo*>& vecRemoveAllInfo = pItemInfo->GetItemRemoveAllInfo();
	for each (GItemRemoveAllInfo* each in vecRemoveAllInfo)
	{
		_VLE(_T(ITEM_TAG_REMOVE));
		_VLP;

		_VLOGGER->ValidateItemID(each->GetID(), _T(ITEM_ATTR_ID));
	}
}