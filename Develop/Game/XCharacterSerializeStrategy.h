#pragma once

#include "XResourceManager.h"
#include "XCharacter.h"

class XCharacter;
class XNetPlayer;
class XNPCInfo;

enum XCharacterSerializeStrategyType
{
	CSST_MY_PLAYER,
	CSST_NET_PLAYER,
	CSST_NPC
};

class XCharacterSerializeStrategy
{
public:
	struct XCharacterEquipment
	{
		const CSItemData*	pItemData;		// 현재 장착된 아이템
		unsigned int		nColor;

		XCharacterEquipment()
		{
			pItemData	= NULL;
			nColor		= 0xffffffff;
		}
	};

	struct _ActorFeature
	{
		// 공통 --
		wstring strMeshName;
		XCharacterEquipment	m_equipments[ITEMSLOT_MAX];

		// PC ----
		SEX		nSex;
		wstring strHairMeshName;
		wstring strFaceMeshName;
		unsigned int	nHairColor;
		unsigned int	nSkinColor;
		unsigned int	nEyeColor;

		// NPC ---
		XNPCInfo*	pNPCInfo;

		_ActorFeature() : pNPCInfo(NULL), nSex(SEX_NA), nHairColor(0), nSkinColor(0), nEyeColor(0)
		{
			for(int i=0;i<ITEMSLOT_MAX;i++)
			{
				m_equipments[i].pItemData = NULL;
			}
		}
	};
protected:
	XCharacter*				m_pActor;
	_ActorFeature			m_Feature;
	XResourceLoadingPolicy	m_nLoadingPolicy;
	bool					m_bResourceLoading;
	bool					m_bLoadingCompleted;

	void _SetMesh( const wstring& strMeshName, float fScale, XCharacterSerializeStrategyType eType );

	struct XCreatePartsNodeParam
	{
		int nPartsID;
		std::wstring strItemMeshName;
		CSItemData* pItemData;
		bool bWeaponOrShield;
		bool bLeftWeapon;
		bool bEye;
		unsigned int nDyedColor;
		XCreatePartsNodeParam() : nPartsID(-1), pItemData(NULL), bWeaponOrShield(false), bLeftWeapon(false), nDyedColor(0), bEye(false) {}
	};

	void CreatePartsNodeAsync( XCreatePartsNodeParam& param );
public:
	XCharacterSerializeStrategy(XCharacter* pActor, bool bResourceLoading);
	virtual ~XCharacterSerializeStrategy() {}

	virtual void OnLoadingComplete() {}
	void SetLoadingPolicy(XResourceLoadingPolicy policy) { m_nLoadingPolicy = policy; }
	wstring GetMeshName() { return m_Feature.strMeshName; }

	virtual XCharacterSerializeStrategyType GetType() = 0;

	const _ActorFeature& GetFeatureForTest() { return m_Feature; }	// ForTest
};


