#pragma once

#include "CSItemData.h"

class GItem;

class GOverlappedEquipmentSlot : public MTestMemPool<GOverlappedEquipmentSlot>
{
public:
	GOverlappedEquipmentSlot(void);
	~GOverlappedEquipmentSlot(void);

	bool IsOverlapped() const;

	void Reset();

	GItem*	GetLeftWeapon() const;				///< 현재 왼손에 장비한 무기 아이템의  인스턴스
	GItem*	GetRightWeapon() const;				///< 현재 오른손에 장비한 무기 아이템의  인스턴스	
	GItem*	GetItem(SH_ITEM_SLOT nSlot) const;
	void	SetItem(SH_ITEM_SLOT nSlot, GItem* pItem);

	void EquipLeftWeapon(GItem* pItem);
	void EquipRightWeapon(GItem* pItem);

private:
	vector<GItem*>	m_vecEquipItems;				// 변경될 장비 아이템
	bool m_bOverlapped;
};
