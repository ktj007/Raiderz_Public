#pragma once

#include "CSItemData.h"
#include "GDef.h"

class GEntityPlayer;
class GItemData;
class GItem;

class GUTHelper_Item
{
public:
	// 새로운 아이템정보를 생성하고 ItemManager에 추가한다.  nID를 INVALID_ID로 넣을 경우, 랜덤한 ID를 생성한다.
	static GItemData* NewItemData(int nID=INVALID_ID, SH_ITEM_SLOT nFirstSlot=ITEMSLOT_NONE, SH_ITEM_SLOT nSecondSlot=ITEMSLOT_NONE);
	// 새로운 무기 아이템정보를 생성하고 ItemManager에 추가.  장착부위는 ITEMSLOT_RWEAPON로 설정.
	static GItemData* NewWeaponItemData(int nID, WEAPON_TYPE nWeaponType);
	// 새로운 방어구 아이템정보를 생성하고 ItemManager에 추가.  장착부위는 ITEMSLOT_HEAD로 설정.
	static GItemData* NewArmorItemData(int nID, ARMOR_TYPE nArmorType);
	// 새로운 방어구 정보를 생성하고 ItemManager에 추가. 장착부위는 ITEMSLOT_LWEAPON로 설정.
	static GItemData* NewShieldItemData(int nID=INVALID_ID);

	// 새로운 아이템을 만든다
	static GItem* NewItem(int nItemID=INVALID_ID, int nAmount=1);
	// 새로운 아이템을 만들고 지급한다.
	static GItem* GiveNewItem(GEntityPlayer* pPlayer, int nItemID=INVALID_ID, int nItemQuantity=1, SH_ITEM_SLOT_TYPE nSlotType = SLOTTYPE_INVENTORY);
	// 새로운 아이템을 만들고 보관함에 지급한다.
	static GItem* GiveNewItemToStorage(GEntityPlayer* pPlayer, int nItemID=INVALID_ID, int nItemQuantity=1);
	// 새로운 아이템을 만들고 보관함에 지급한다.
	static GItem* GiveNewItemToGuildStorage(GEntityPlayer* pPlayer, int nItemID=INVALID_ID, int nItemQuantity=1);
	// 새로운 아이템을 만들고 지급한후, 장착 시킨다.
	static GItem* EquipNewItem(GEntityPlayer* pPlayer, int nItemID=INVALID_ID, SH_ITEM_SLOT nItemSlot = ITEMSLOT_NONE);
	// 무기 아이템정보를 생성하고, pOwner의 인벤토리에 추가한다.
	static GItem* GainWeaponItem(GEntityPlayer* pOwner, int nID=INVALID_ID, WEAPON_TYPE nWeaponType = WEAPON_1H_SLASH);
	// 방어구 아이템정보를 생성하고, pOwner의 인벤토리에 추가한다.
	static GItem* GainArmorItem(GEntityPlayer* pOwner, ARMOR_TYPE nArmorType = ARMOR_CLOTH);
	// 방패 아이템정보를 생성하고, pOwner의 인벤토리에 추가한다.
	static GItem* GainShieldItem(GEntityPlayer* pOwner);

	// 염색 아이템정보를 생성하고, pOwner의 인벤토리에 추가한다.
	static GItem* GainDyeItem( GEntityPlayer* pOwner, int nColor);
	static GItem* GainDyeItem( GEntityPlayer* pOwner, vector<int> vecColor);


	// 무기를 pOwner의 인벤토리에 추가하고 장착한다.
	static GItem* EquipNewWeapon(GEntityPlayer* pOwner, WEAPON_TYPE nWeaponType, SH_ITEM_SLOT nSlot=ITEMSLOT_RWEAPON);
	// 방어구를 pOwner의 인벤토리에 추가하고 장착한다.
	static GItem* EquipNewShield(GEntityPlayer* pOwner);
	// 단순이 아이템 장비
	static bool EquipItem(GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot=ITEMSLOT_RWEAPON);
	// 단순이 아이템 해제
	static bool UnequipItem(GEntityPlayer* pPlayer, GItem* pItem);

	// 인벤토리를 아이템으로 채움
	static bool FullItemToInven(GEntityPlayer* pPlayer, int nEmptySize=0);

	// 강화석 만들기
	GItem* NewEnchantStoneItem(GEntityPlayer* pPlayer, int nLinkedBuffID=INVALID_BUFF_ID);
};