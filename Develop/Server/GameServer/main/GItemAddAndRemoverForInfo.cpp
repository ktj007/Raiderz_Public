#include "StdAfx.h"
#include "GItemAddAndRemoverForInfo.h"
#include "GItemAdder.h"
#include "GItemAddRemoveInfo.h"
#include "GGlobal.h"
#include "GItemSystem.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GItemRemover.h"

bool GItemAddAndRemoverForInfo::AddAndRemove(GEntityPlayer* pPlayer, GItemAddRemoveInfo* pItemAddRemoveInfo )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItemAddRemoveInfo, false);
	
	const vector<GItemFillInfo*>& vecFillInfo = pItemAddRemoveInfo->GetItemFillInfo();
	const vector<GItemAddInfo*>& vecAddInfo = pItemAddRemoveInfo->GetItemAddInfo();
	const vector<GItemRemoveInfo*>& vecRemoveInfo = pItemAddRemoveInfo->GetItemRemoveInfo();	
	const vector<GItemRemoveAllInfo*>& vecRemoveAllInfo = pItemAddRemoveInfo->GetItemRemoveAllInfo();

	// Add만 성공하면 된다.
	if (false == Check(pPlayer, vecAddInfo)) return false;

	for each (GItemFillInfo* each in vecFillInfo)
	{
		Fill(pPlayer, each);
	}

	for each (GItemAddInfo* each in vecAddInfo)
	{
		Add(pPlayer, each);
	}

	for each (GItemRemoveInfo* each in vecRemoveInfo)
	{
		Remove(pPlayer, each);
	}

	for each (GItemRemoveAllInfo* each in vecRemoveAllInfo)
	{
		RemoveAll(pPlayer, each);
	}

	return true;
}

bool GItemAddAndRemoverForInfo::Check(GEntityPlayer* pPlayer, const vector<GItemAddInfo*>& vecAddInfo)
{
	VALID_RET(pPlayer, false);

	vector<CHECK_ADDABLE_ITEM> vecCheckAddableItem;
	for each (GItemAddInfo* each in vecAddInfo)
	{
		vecCheckAddableItem.push_back(CHECK_ADDABLE_ITEM(each->GetID(), each->GetAmount()));
	}

	return gsys.pItemSystem->GetAdder().CheckItemAddable(pPlayer, vecCheckAddableItem);
}

void GItemAddAndRemoverForInfo::Fill(GEntityPlayer* pPlayer, GItemFillInfo* pFillInfo)
{
	VALID(pPlayer);
	VALID(pFillInfo);

	int nEquipAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_EQUIP, pFillInfo->GetID());
	int nInvenAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pFillInfo->GetID());
	int nStorageAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_STORAGE, pFillInfo->GetID());
	if (pFillInfo->GetAmount() <= nEquipAmount+nInvenAmount+nStorageAmount) return;

	int nAddAmount = pFillInfo->GetAmount() - nInvenAmount;
	gsys.pItemSystem->GetAdder().Add(pPlayer, pFillInfo->GetID(), nAddAmount);
}

void GItemAddAndRemoverForInfo::Add(GEntityPlayer* pPlayer, GItemAddInfo* pAddInfo)
{
	VALID(pPlayer);
	VALID(pAddInfo);

	gsys.pItemSystem->GetAdder().Add(pPlayer, pAddInfo->GetID(), pAddInfo->GetAmount());
}

void GItemAddAndRemoverForInfo::Remove(GEntityPlayer* pPlayer, GItemRemoveInfo* pRemoveInfo)
{
	VALID(pPlayer);
	VALID(pRemoveInfo);

	gsys.pItemSystem->GetRemover().Remove(pPlayer, pRemoveInfo->GetID(), pRemoveInfo->GetAmount());
}

void GItemAddAndRemoverForInfo::RemoveAll(GEntityPlayer* pPlayer, GItemRemoveAllInfo* pRemoveAllInfo)
{
	VALID(pPlayer);
	VALID(pRemoveAllInfo);

	int nEquipAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_EQUIP, pRemoveAllInfo->GetID());
	int nInvenAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pRemoveAllInfo->GetID());
	int nStorageAmount = pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_STORAGE, pRemoveAllInfo->GetID());

	gsys.pItemSystem->GetRemover().Remove(pPlayer, pRemoveAllInfo->GetID(), nEquipAmount+nInvenAmount+nStorageAmount);
}