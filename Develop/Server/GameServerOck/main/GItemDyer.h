#pragma once

class GItem;
class GEntityPlayer;

class GDBT_ITEM_DYE;

class GItemDyer : public MTestMemPool<GItemDyer>
{
public:
	bool Prepare(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID);
	bool PrepareCancel(GEntityPlayer* pPlayer);

	bool Dye(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID);
	bool DyeForGM(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nColor);

	void DyeForDBTask(GDBT_ITEM_DYE& data);		

private:
	bool Check(GEntityPlayer* pPlayer, GItem* pTargetItem, GItem* pDyeItem);
	void Route(GEntityPlayer* pPlayer, GItem* pTargetItem, int nColor);
};
