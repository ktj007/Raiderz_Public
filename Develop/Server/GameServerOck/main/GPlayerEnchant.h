#pragma once

#include "GActorObserver.h"

class GEntityPlayer;

class GPlayerEnchant : public GActorObserver, public MTestMemPool<GPlayerEnchant>
{
public:
	GPlayerEnchant(GEntityPlayer* pOwner);
	virtual ~GPlayerEnchant(void);

private:
	// 아이템 강화 성공 (Player 전용)
	virtual void OnItemEnchantSuccess(GItem* pItem, int nEnchantItemID) override;
	// 아이템 장착 (Player 전용)
	virtual void OnItemEquipped(GItem* pItem) override;
	// 아이템 장착해제 (Player 전용)
	virtual void OnItemUnequipped(GItem* pItem) override;
	
private:
	void GainBuffFromEquip(GItem* pItem);
	void LostBuffFromEquip(GItem* pItem);
	bool IsSameWeaponSet(GItem* pItem) const;
	int GetLinkedBuffID(int nEnchantItemID) const;
private:
	GEntityPlayer*	m_pOwner;
};
