#pragma once

class GItem;
class GEntityPlayer;

class GItemMover : public MTestMemPool<GItemMover>
{
struct PLAYER_RELATE_ID
{
	int64 nFromAID;
	int64 nFromCIDorGID;
	int64 nToAID;
	int64 nToCIDorGID;
};

public:
	bool Move(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, int nFromItemID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nToItemID, int nAmount);

private:
	bool Check(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, int nFromItemID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nToItemID, int nAmount);
	bool CheckMove(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
	bool CheckSplit(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID);
	bool CheckMerge(GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem, int nAmount);
	bool CheckSwap(GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem);

	bool DB(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);bool DBMove(GEntityPlayer* pPlayer,  GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);
	bool DBSplit(GEntityPlayer* pPlayer,  GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount);
	bool DBMerge( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem, int nAmount );
	bool DBSwap( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem );

	void GetPlayerRelateID(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, SH_ITEM_SLOT_TYPE nToSlotType, PLAYER_RELATE_ID& playerRelateID);
};