#ifndef _XEQUIPMENT_SLOT_H
#define _XEQUIPMENT_SLOT_H

#include "CSItemData.h"
#include "XItem.h"
#include "XItemData.h"
#include "XModEffector.h"

class XEquipmentSlot
{
protected:
	XItem*			m_pItem[ITEMSLOT_MAX];
	XModEffector	m_ModEffector;
	int8			m_nWeaponSet;

public:
	XEquipmentSlot();
	virtual void	EquipItem(XItem* pItem, SH_ITEM_SLOT nSlot);			// 원래 장착된 아이템을 리턴.;	
	virtual XItem*	RemoveItem(SH_ITEM_SLOT nSlot);						// 역시 원래 장착된 아이템을 리턴.;	

	XItem*			GetItem(SH_ITEM_SLOT nSlot)  const;
	void			Clear();

	void			SwitchingWeaponSet(int8 nWeaponSet);
	void			SetWeaponSet(int8 nWeaponSet)			{ m_nWeaponSet = nWeaponSet; }
	int8			GetWeaponSet()							{ return m_nWeaponSet; }		

	bool			CheckHasPassiveTalent(XItem* pItem);	

	void			AddModifierByTrain(TALENT_EXTRA_PASSIVE_TYPE nTrainTEPT);		
	void			EraseModifierByUntrainAll();
	void			AddModifierByEquip( XItem* pItem );
	void			EraseModifierByUnequip(XItem* pItem);	

private:
	void			SetItem(SH_ITEM_SLOT nSlot, XItem* pItem);
	void			AddModifier( XItem* pItem );	
	void			EraseModifier( XItem* pItem );

};

class XOverlappedEquipmentSlot : public XEquipmentSlot
{
private:
	bool			m_bActive;

public:
	XOverlappedEquipmentSlot();
	~XOverlappedEquipmentSlot()	{}

	virtual void	EquipItem(XItem* pItem, SH_ITEM_SLOT nSlot);
	virtual XItem*	RemoveItem(SH_ITEM_SLOT nSlot);

	bool			GetActive()		{ return m_bActive; }
};

class XMyEquipmentSlot
{
private:
	XEquipmentSlot			m_EquipmentSlot;	

	XOverlappedEquipmentSlot	m_OverlappedEquipmentSlot;

protected:
	void					ClaimRequire(XItem* pItem);
	void					ChangeItemAttr(XItem* pItem);

public:
	XMyEquipmentSlot();
	~XMyEquipmentSlot() {}

	XItem*					GetItem(SH_ITEM_SLOT nSlot);
	XItem*					GetItem(const wchar_t* szSlot);
	int						GetItemID(SH_ITEM_SLOT nSlot);
	MUID					GetItemUID(SH_ITEM_SLOT nSlot);
	vector<XItem*>			GetAllItem();			///< Dummy_client에서 사용합니다.

	const wchar_t*				GetItemName(int nSlot);
	const wchar_t*				GetItemName(const wchar_t* slot);

	const wchar_t*				GetItemIconName(int nSlot);
	const wchar_t*				GetItemIconName(const wchar_t* slot);
	
	void					RefreshUI();
	WEAPON_TYPE				GetCurrWeaponType();					///< 현재 장비한 아이템의 무기 타입
	void					GetCurrWeapon(XItemData** pItemDataR, XItemData** pItemDataL);					///< 현재 장비한 아이템의 무기
	bool					IsEquipShield();

	void					EquipItem(XItem* pItem, SH_ITEM_SLOT nSlot);	
	XItem*					UnEquipItem(SH_ITEM_SLOT nSlot);

	bool					IsEquipItem(int nItemID);
	void					SwitchingWeaponSet(int8 nWeaponSet);
	int8					GetWeaponSet()							{ return m_EquipmentSlot.GetWeaponSet(); }

	float					GetSumOfCraftOptionRate();	///< 내구도 변경
	void					Clear();

	XItem*					GetCurrentWeaponSetWeaponRight();
	XItem*					GetCurrentWeaponSetWeaponLeft();

	int						GetEquipSlotID(const wchar_t* slot);
	bool					IsEquiped(SH_ITEM_SLOT nSlot);

	int						GetAP();
	int						GetItemQuantityByID(int nID);

	XItem*					GetItemByID(int nID);

	XItem*					GetRandomItem();

	// 오버랩 장비
	void					EquipOverlappedItem(XItem* pNewItem);
	XItem*					UnEquipOverlappedItem(CSItemData* pTempItem);
	bool					IsOverlappedItemActive();
	bool					IsEquipOverlappedItem(int nItemID);

	void					AddModifierByTrain(TALENT_EXTRA_PASSIVE_TYPE nTrainTEPT);		
	void					EraseModifierByUntrain();
};




#endif // _XEQUIPMENT_SLOT_H
