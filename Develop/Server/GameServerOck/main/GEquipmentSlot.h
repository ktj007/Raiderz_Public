#pragma once

#include "GItemContainer.h"
#include "CSItemData.h"
#include "GItem.h"
#include "GModEffector.h"

class GOverlappedEquipmentSlot;

/// 장비 슬롯
class GEquipmentSlot: public GItemContainer
{
public:
	GEquipmentSlot(GEntityPlayer* pOwner);
	virtual ~GEquipmentSlot();

	virtual void			CollectAllItem(vector<GItem*>& outvecItem) override;	///< 현재 장비한 모든 아이템의 인스턴스	
	virtual bool			PushItem(int nSlotID, GItem* pItem) override;		///< 원래 장착된 아이템을 리턴.;	
	virtual GItem*			PopItem(int nSlotID) override;						///< 역시 원래 장착된 아이템을 리턴.;		

	void					AddModifierByTrain(TALENT_EXTRA_PASSIVE_TYPE nTrainTEPT);
	void					EraseModifierByUntrainAll();
	void					AddModifierByEquip(GItem* pItem);
	void					EraseModifierByUnequip(GItem* pItem);

	void					AddModifier(GItem* pItem);
	void					EraseModifier(GItem* pItem );

	GItem*					GetLeftWeapon() const;				///< 현재 왼손에 장비한 무기 아이템의  인스턴스
	GItem*					GetRightWeapon() const;				///< 현재 오른손에 장비한 무기 아이템의  인스턴스	
	GItem*					GetArmor() const;					///< 장착중인 갑옷 반환
	GItem*					GetShield() const;					///< 현재 장비한 방패 아이템의  인스턴스
	GItem*					GetItemFromSubSlot(SH_ITEM_SLOT nSubSlot)  const;	

	void					CollectArmorSlotItem(vector<GItem*>& outvecEquipmentItem);	///< 현재 장비한 방패를 제외한 모든 방어구의 인스턴스	

	WEAPON_TYPE				GetWeaponType() const;
	WEAPON_TYPE				GetLeftWeaponType() const;			///< 현재 왼손에 장비한 무기 아이템의 타입
	WEAPON_TYPE				GetRightWeaponType() const;			///< 현재 오른손에 장비한 무기 아이템의 타입
	ARMOR_TYPE				GetArmorType() const;				///< 현재 장착중인 방어구 아이템의 타입
	ARMOR_TYPE				GetSlotMaterial(SH_ITEM_SLOT nSlot);

	int						GetTotalAP();						// 전체 장비의 AP. 이건 빈번하게 호출되니 나중에 따로 변수에 넣어놔야 한다. - birdkr
	SH_ITEM_SLOT			GetEquipedSlot(const GItem* pItem) const;
	SH_ITEM_SLOT			GetSubSlot(SH_ITEM_SLOT nSlot);
	
	void					SwitchingWeaponSet(SH_ITEM_SWITCH_WEAPON val);
	SH_ITEM_SWITCH_WEAPON	GetWeaponSet() const;	
	
	bool					IsEquipItem(SH_ITEM_SLOT nSlot);
	bool					IsEquipItem(int nItemID);	
	bool					IsEquipItem(GItem* pItem);
	bool					IsEquipShield() const;
	bool				 	IsUnarmed() const;
	bool				 	IsWeaponSwitchable(SH_ITEM_SWITCH_WEAPON nWeaponSet) const;

	GOverlappedEquipmentSlot& GetOverlapped();

private:
	void					SetWeaponSet(SH_ITEM_SWITCH_WEAPON val);
	bool				 	IsUnarmedByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const;
	WEAPON_TYPE				GetLeftWeaponTypeByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const;
	WEAPON_TYPE				GetRightWeaponTypeByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const;	
	GItem*					GetLeftWeaponByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const;
	GItem*					GetRightWeaponByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const;


private:		
	SH_ITEM_SWITCH_WEAPON	m_nWeaponSet;			// 무기 집합 번호 (주무기/보조무기)
	GModEffector			m_ModEffector;
	GOverlappedEquipmentSlot*	m_pOverlapped;
};
