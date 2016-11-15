#pragma once

class GItem;

enum ITEM_STATE_TYPE
{
	IST_NONE = 0,

	IST_INVENTORY,		// 인벤토리에 있는 아이템
	IST_STORAGE,		// 창고에 있는 아이템
	IST_POST,			// 편지함에 있는 아이템
	IST_EQUIPED,		// 착용중인 장비 아이템
	IST_ARRANGED,		// 배치한 가구 아이템
	IST_TRADING,		// 거래중인 아이템
	IST_ENCNAHTING,		// 강화중인 아이템
	IST_DYEING,			// 염색중인 아이템
	IST_MAX
};


class GItemStateSelector
{
public:
	ITEM_STATE_TYPE Select(GEntityPlayer* pPlayer, const GItem* pItem);

private:
	bool IsInventoryItem(GEntityPlayer* pPlayer, const GItem* pItem);
	bool IsStorageItem(GEntityPlayer* pPlayer, const GItem* pItem);
	bool IsPostItem(GEntityPlayer* pPlayer, const GItem* pItem);
	bool IsEquipedItem(GEntityPlayer* pPlayer, const GItem* pItem);
	bool IsArrangedItem(GEntityPlayer* pPlayer, const GItem* pItem);
	bool IsTradingItem(GEntityPlayer* pPlayer, const GItem* pItem);	
	bool IsEnchant(GEntityPlayer* pPlayer);
	bool IsDyeingItem(GEntityPlayer* pPlayer);
};