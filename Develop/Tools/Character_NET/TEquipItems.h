#ifndef _TEQUIPITEM_H
#define _TEQUIPITEM_H

#pragma once
#include "TCloth.h"

class TEquipItems
{
private:
	TCharacter*		m_pActor;

	TItemManager*	m_pItemManager;
	TTabModel*		m_pTabModel;
	TEquipment		m_TEquipment;
	TClothMgr*		m_pClothMgr;	//옷관련 속성창 관리

	tstring			m_strItemParentName;
	SH_ITEM_SLOT	m_ItemSlotnum;
	WEAPON_TYPE		m_WeaponType;

protected:
	void			DefaultFaceLoad();	///< 기본 얼굴 로드
	void			DefaultHairLoad();

	void			EquipSimulatedCloth( int itemid, int nSlot);

	void			ReviewWeaponItem(tstring MeshName);
	void			ReviewEquipItem(tstring MeshName, tstring NodeName, tstring ParentName, SH_ITEM_SLOT islot, WEAPON_TYPE itype);
	void			ReviewEquipCloth(tstring MeshName, tstring NodeName, SH_ITEM_SLOT islot);

	CSItemData*		MakeItem( tstring NodeName, tstring MeshName, SH_ITEM_SLOT islot, WEAPON_TYPE itype, int nBitNodeList = 0 );

	SH_ITEM_SLOT	GetClothItemSlot(string& strNodeName, SH_ITEM_SLOT islot);
	bool			CheckClothItem(string& strNodeName);

public:
	TEquipItems();
	virtual ~TEquipItems() {}

	bool			SaveEquipItems();
	void			LoadEquipItems(TCharacter* pActor);

	void			SetEquipItemData(TItemManager* pItemManager, TTabModel* pTabModel, TClothMgr* pClothMgr);
	void			SetItemParentName(string strName)		{ m_strItemParentName = strName; }
	void			SetItemSlotNum(SH_ITEM_SLOT nSlot)		{ m_ItemSlotnum = nSlot; }

	void			AllUnEquipItem();
	void			UnEquipItem(string ItemCategory);
	void			UnEquipItem(int nSlot, bool bResetToDefault = true);

	void			EquipReviewItems(System::String^ MeshName, System::String^ NodeName, SH_ITEM_TYPE iType = ITEMTYPE_WEAPON);
	void			EquipReviewItems(const char* MeshName, const char* NodeName, SH_ITEM_TYPE iType = ITEMTYPE_WEAPON);

	bool			EquipItemID(int itemid, int nSlot);
	bool			EquipItemSlotIndex(int index);

	void			CreateClothMenuForNPC(void);

	void			SetActor(TCharacter *pActor)	{ m_pActor = pActor;	}

};



#endif