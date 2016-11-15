#pragma once

class GEntityPlayer;
class GItem;
struct GDBT_ITEM_EQUIP;

enum SH_ITEM_SWITCH_WEAPON;
enum SH_ITEM_SLOT;

TEST_FORWARD_FT(ItemEquiper, Fixture, CheckSlot);
TEST_FORWARD_FT(ItemEquiper, Fixture, CheckReservedDummySlot);
TEST_FORWARD_FT(ItemEquiper, Fixture, CheckHasPassiveTalent_Weapon);
TEST_FORWARD_FT(ItemEquiper, Fixture, CheckHasPassiveTalent_Armor);
TEST_FORWARD_FT(ItemEquiper, Fixture, SelectUnequipItem);
TEST_FORWARD_FT(TestDBCache_ItemEquip, FIXTURE, TestItemEquipSyncSuccess);
TEST_FORWARD_FT(TestDBCache_ItemEquip, FIXTURE, TestItemEquipSyncFail);
TEST_FORWARD_FT(TestDBCache_ItemEquip, FIXTURE, TestItemUnequipSyncFail);

class GItemEquiper : public MTestMemPool<GItemEquiper>
{
TEST_FRIEND_FT(ItemEquiper, Fixture, CheckSlot);
TEST_FRIEND_FT(ItemEquiper, Fixture, CheckReservedDummySlot);
TEST_FRIEND_FT(ItemEquiper, Fixture, CheckHasPassiveTalent_Weapon);
TEST_FRIEND_FT(ItemEquiper, Fixture, CheckHasPassiveTalent_Armor);
TEST_FRIEND_FT(ItemEquiper, Fixture, SelectUnequipItem);
TEST_FRIEND_FT(TestDBCache_ItemEquip, FIXTURE, TestItemEquipSyncSuccess);
TEST_FRIEND_FT(TestDBCache_ItemEquip, FIXTURE, TestItemEquipSyncFail);
TEST_FRIEND_FT(TestDBCache_ItemEquip, FIXTURE, TestItemUnequipSyncFail);

public:	
	bool Equip(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot);
	bool CheckHasPassiveTalent(GEntityPlayer* pPlayer, GItem* pItem);

private:		
	bool Check(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot);
	bool CheckSlot(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot );
	bool CheckReservedDummySlot( GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot);
	bool CheckReservedEquipSlot( GEntityPlayer* pPlayer, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot);
	bool CheckCondition( GEntityPlayer* pPlayer, GItem* pItem );	
	bool CheckActableItem( GEntityPlayer* pPlayer, GItem* pItem );	

	bool MakeDBTaskData(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot, bool bBind, GDBT_ITEM_EQUIP& data);
	int SelectUnequipItem(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot, SH_ITEM_SLOT nSubSlot, GItem*& pUnequipItem1, GItem*& pUnequipItem2);
	GItem* SelectTogetherUnequipableWeaponItem(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot);

	// 인벤 아이템을 장착 할때 장착해제 되는 아이템을 장착한 아이템이 있던 인벤슬롯으로 보낼지 판단.
	bool IsUnequipSwapItem(GItem* pUnequipItem1, GItem* pEquipItem);
};
