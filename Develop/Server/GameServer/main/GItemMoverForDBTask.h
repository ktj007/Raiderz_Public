#pragma once

class GItem;

class GItemMoverForDBTask : public MTestMemPool<GItemMoverForDBTask>
{
public:
	bool Move(const MUID& uidPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID=0);
	void OnPreCompleted(const MUID& uidPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);

private:
	void _Move(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);
	void Split(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID);
	void Merge(GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem, int nAmount);
	void Swap(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, GItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);

	void Route(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID);
};
