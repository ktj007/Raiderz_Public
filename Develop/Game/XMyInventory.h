#ifndef _XMY_INVENTORY_H
#define _XMY_INVENTORY_H

#include "XItemContainer.h"

class XItem;

/// 내 인벤토리 아이템
class XMyInventory : public XItemContainer
{
public:
	XMyInventory();
	virtual ~XMyInventory();

	virtual int		GetSlotSize()	{ return m_bExpended ? MAX_INVENTORY_SIZE : BASE_INVENTORY_SIZE;}

	void			Use(int nSlotID, CHR_STATUS* chr_status, int nAmount);
	XItem*			GetUsableSelfRebirthItem();
	vector<XItem*>	GetSortedAllItem();	
	void			SortSlot();

	bool			IsHaveGatherTool(GATHER_TYPE nType);
	XItem*			GetRandomItem();
	XItem*			GetRandomSellableItem();

private:
	vector<XItem*>	SelectEquipItemForSort(set<XItem*>& setItem);
	vector<XItem*>	SelectUsableItemForSort(set<XItem*>& setItem);
	vector<XItem*>	SelectQuestItemForSort(set<XItem*>& setItem);
	vector<XItem*>	SelectMaterialItemForSort(set<XItem*>& setItem);
	vector<XItem*>	SelectEtcItemForSort(set<XItem*>& setItem);	

protected:
	bool			m_bExpended;						///< 확장슬롯 사용여부
};

#endif // _XMY_INVENTORY_H