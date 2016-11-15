#pragma once

#include "XCharacterSerializeStrategy.h"

class XMyEquipmentSlot;

class XPlayerSerializeStrategy : public XCharacterSerializeStrategy
{
protected:
	bool m_bChangeSkinColor;

	struct SerializeParam
	{
		SEX nSex;
		short nHairColor;
		short nSkinColor;
		short nEyeColor;
		int8 nHair;
		int8 nFace;
		int8 nWeaponSet;

		wstring strMeshName;
	};

	struct stEquipmentItem
	{
		CSItemData*		pItemData;
		unsigned int	nDyedColor;

		stEquipmentItem()
		{
			pItemData = NULL;
			nDyedColor = 0;
		}
	};

	void _Serialize(SerializeParam* pParam);

	void _SetPlayerHairMesh( SEX nSex, int nIndex );
	void _SetPlayerFaceMesh( SEX nSex, int nIndex );

	// Equip / UnEquip item
	// UnEquip은 SyncLoad로, Equip은 MeshLoading을 AsyncLoading으로
	void _EquipItemMesh( stEquipmentItem& stItemData, SH_ITEM_SLOT nEquipedSlot );

	// bResetToDefault : 장착해제이후 (옷등)기본모델로 돌아갈지 여부
	void _UnEquipItemMesh( int nSlot );

	void BuildCreatePartsNodeParam( XCreatePartsNodeParam &param, CSItemData* pItemData, SH_ITEM_SLOT nEquipedSlot );

	void SetPartsMeshToDefault( int nSlot );
	wstring GetDefaultPartsName(SEX nSex, SH_ITEM_SLOT nItemSlot);

	void SetEquipmentItemParts(SH_ITEM_SLOT nItemSlot, int8 nWeaponSet=0);
	virtual stEquipmentItem GetEquipmentItem(SH_ITEM_SLOT nItemSlot, int8 nWeaponSet) = 0;

	void AddQuiver();
	void RemoveQuiver();
public:
	XPlayerSerializeStrategy(XCharacter* pActor, bool bResourceLoading) : XCharacterSerializeStrategy(pActor, bResourceLoading), m_bChangeSkinColor(false) {}
	virtual ~XPlayerSerializeStrategy() {}

};

class XMyPlayerSerializeStrategy : public XPlayerSerializeStrategy
{
protected:
	XMyEquipmentSlot* m_pMyEquipmentSlot;
	virtual stEquipmentItem GetEquipmentItem(SH_ITEM_SLOT nItemSlot, int8 nWeaponSet);
public:
	XMyPlayerSerializeStrategy(XCharacter* pActor, bool bResourceLoading) : XPlayerSerializeStrategy(pActor, bResourceLoading), m_pMyEquipmentSlot(NULL) {}
	virtual ~XMyPlayerSerializeStrategy() {}
	void Serialize(PLAYER_INFO& playerInfo, XMyEquipmentSlot* pEquipmentSlot);

	virtual XCharacterSerializeStrategyType GetType() { return CSST_MY_PLAYER; }
};

class XNetPlayerSerializeStrategy : public XPlayerSerializeStrategy
{
private:
	int	m_nEquipedItemColor[FEATURE_ITEMSLOT_MAX];

protected:
	XNetPlayer* m_pOwner;
	virtual stEquipmentItem GetEquipmentItem(SH_ITEM_SLOT nItemSlot, int8 nWeaponSet);
public:
	XNetPlayerSerializeStrategy(XCharacter* pActor, bool bResourceLoading);
	virtual ~XNetPlayerSerializeStrategy() {}
	void Serialize( XNetPlayer* pNetPlayer, XPlayerInfoFeature& feature );

	virtual XCharacterSerializeStrategyType GetType() { return CSST_NET_PLAYER; }
};
