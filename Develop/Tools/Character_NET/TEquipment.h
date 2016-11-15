#pragma once

class TEquipment
{
protected:
	string m_hf[ITEMSLOT_MAX];
	string m_hm[ITEMSLOT_MAX];

	bool	EquipItem(const CSItemData* pItem, int nSlot);
public:
	TEquipment(void);
	~TEquipment(void);

	bool	SaveEquipment();
	bool	LoadEquipment();

	void	DefaultItemEquip(PLAYER_TYPE playertype);

	bool	EquipItemName(string itemname);
	bool	EquipItemID(int itemid, int nSlot);
	bool	EquipItemSlotIndex(int index);

	void	SetItemEquipInfo( const CSItemData* pItem, int nSlot );
	void	SetEquipInfo(tstring MeshName, const char* NodeName, PLAYER_TYPE playertype);

	void	SetUnEquipInfo(int islot, PLAYER_TYPE playertype);

	bool	IsEquip(SH_ITEM_SLOT islot, PLAYER_TYPE playertype);

	ARMOR_TYPE GetArmorType(SH_ITEM_SLOT iSlot);
};
