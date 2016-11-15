#ifndef _G_ITEM_UNEQUIPER_H_
#define _G_ITEM_UNEQUIPER_H_

class GEntityPlayer;
enum SH_ITEM_SLOT;
class GItem;
struct GDBT_ITEM_UNEQUIP;

TEST_FORWARD_FT(ItemUnequiper, FItemUnequiper, IsUnequipable);
TEST_FORWARD_FT(ItemUnequiper, FItemUnequiper, Check);
TEST_FORWARD_FT(TestDBCache_ItemUnequip, FIXTURE, TestItemUnequipSyncSuccess);
TEST_FORWARD_FT(TestDBCache_ItemUnequip, FIXTURE, TestItemUnequipSyncFail);

class GItemUnequiper : public MTestMemPool<GItemUnequiper>
{
TEST_FRIEND_FT(ItemUnequiper, FItemUnequiper, IsUnequipable);
TEST_FRIEND_FT(ItemUnequiper, FItemUnequiper, Check);
TEST_FRIEND_FT(TestDBCache_ItemUnequip, FIXTURE, TestItemUnequipSyncSuccess);
TEST_FRIEND_FT(TestDBCache_ItemUnequip, FIXTURE, TestItemUnequipSyncFail);

public:
	bool Unequip(GEntityPlayer* pPlayer, GItem* pItem);
	void UnequipForDBTask(GDBT_ITEM_UNEQUIP& data);
	void FailForDBTask(GDBT_ITEM_UNEQUIP& data);

	void Route(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlotIDFrom, int nSlotIDTo);

private:
	bool Check(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot);	
	bool CheckReservedDummySlot(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot);
	bool CheckReservedEquipSlot( GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot );
	
	bool MakeDBTaskData(GEntityPlayer* pPlayer, GItem* pItem, GDBT_ITEM_UNEQUIP& data );
	GItem* SelectExtraUnequipItem(GEntityPlayer* pPlayer, GItem* pItem);
	
	void RouteToOther(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlotIDFrom);
	void RouteToMe(GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlotIDFrom, int nSlotIDTo);	
};


#endif