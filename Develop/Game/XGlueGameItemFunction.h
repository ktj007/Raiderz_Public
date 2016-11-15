#pragma once

class XGlueGameItemFunction
{
public:
	//아이템
	const char*				GetItemName(int nItemID);
	int						GetItemPrice(int nItemID );
	const char*				GetItemType(int nItemID );
	const char*				GetItemEquipmentType( int nItemID );
	int						GetItemAP(int nItemID);
	int						GetItemSlotType(int nItemID);
	int						GetItemStackSize( int nItemID);
	int						GetItemMaxDurability( int nItemID);
	const char*				GetItemIconName(int nItemID);

	void					UseItem(const char* szuid);
	void					DropItem(const char* szuid);

	//상점
	void					BuyItem(int nItemID, int nCount);
	void					SellItem(const char* szuid, int nCount);
};
