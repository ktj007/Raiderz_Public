#pragma once

class TItemData;

class SlotList
{
public:
	typedef std::vector<TItemData*> ITEMDATALIST;
	ITEMDATALIST m_Index;
};

class TTabItem
{
protected:
	typedef std::map<int, SlotList> ITEMMAP;
	ITEMMAP				m_ItemMap;

	int					m_nSelSlot;
	SlotList			m_SelSlotList;

public:
	TTabItem(void);
	virtual ~TTabItem(void);

	void	Init();
	void	InitItemMap();

	//void	SelectItemEquip( int nSelSlot, int nID );
	SlotList* SelectItem(int nSlot);

	int		GetSelSlot() const { return m_nSelSlot; }	///< 현재 선택된 아이템의 slot index

	CSItemData*	GetItemData(int index);
	CSItemData* GetItemDataByID(int nID);
};
