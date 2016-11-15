#pragma once

class XItemHelper
{
public:
	static XTalentInfo* GetTalentUsableItem( int ItemID );
	static bool IsUsableItemAtDead(int ItemID);
	static bool IsUsableSelfRebirthItem(int ItemID);
	static bool IsAvailable(int ItemID );
	static int GetInvenItemReuseTimeRemaining(int ItemID);

	static XItem* GetItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID);
	static void PushItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, XItem* pItem);
	static XItem* PopItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID);

	static 	void MoveItem(XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);
	static 	XItem* SplitItem(XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);
	static 	void MergeItem(XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, XItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nAmount);
	static 	void SwapItem(XItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, XItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
	static 	bool DecreaseItemAmount(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount);
};
