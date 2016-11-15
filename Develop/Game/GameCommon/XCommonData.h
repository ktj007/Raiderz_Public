#pragma once

#include "stdafx.h"
#include "XCharacter.h"

using namespace rs3;

struct EFFECT_CREATE_DATA
{
	tstring			strEffectName;
	tstring			strBoneName;

	MUID			uidActor;

	float			fDurationTime;
	float			fStartVisibilityTime; 
	float			fEndVisibilityTime;
	float			fWaterHeightMin;
	float			fWaterHeightMax;

	bool			bLastFramePause; 
	bool			bIgnoreDir; 
	bool			bImmediatlyDelete;
	bool			bAutoScale;
	bool			bFollow;
	bool			bTerrainAlign;
	bool			bTerrainPos;
	bool			bWaterPos;
	bool			bCancelEnabled;

	vec3			vLocal;
	vec3			vScale;
	MMatrix			matPostMatrix;

	int				nPartsSlotType;

	EFFECT_CREATE_DATA()
	{
		strEffectName.clear();
		strBoneName.clear();

		uidActor				= MUID::ZERO;

		fDurationTime			= 0.0f;
		fStartVisibilityTime	= 0.0f;
		fEndVisibilityTime		= 0.0f;
		fWaterHeightMin			= 0.0f;
		fWaterHeightMax			= 0.0f;

		bLastFramePause			= false; 
		bIgnoreDir				= false; 
		bImmediatlyDelete		= false;
		bAutoScale				= false;
		bFollow					= false;
		bTerrainAlign			= false;
		bTerrainPos				= false;
		bWaterPos				= false;
		bCancelEnabled			= false;

		vLocal					= vec3::ZERO;
		vScale					= vec3(1.0f, 1.0f, 1.0f);

		nPartsSlotType			= 0;

		matPostMatrix.MakeIdentity();
	}

	EFFECT_CREATE_DATA(tstring effectName, tstring boneName, 
						MUID uid, MMatrix matPost, vec3 vLocalPos, 
						float durationTime, vec3 scale, float startVisibilityTime, float endVisibilitiTime, 
						bool lastFramePause, bool ignoreDir, bool immediatlyDelete, bool autoScale, bool follow, bool terrainAlign, bool terrainPos, bool waterPos, bool cancelEnabled)
	{
		strEffectName			= effectName;
		strBoneName				= boneName;

		uidActor				= uid;

		fDurationTime			= durationTime;
		fStartVisibilityTime	= startVisibilityTime;
		fEndVisibilityTime		= endVisibilitiTime;

		bLastFramePause			= lastFramePause;
		bIgnoreDir				= ignoreDir;
		bImmediatlyDelete		= immediatlyDelete;
		bAutoScale				= autoScale;
		bFollow					= follow;
		bTerrainAlign			= terrainAlign;
		bTerrainPos				= terrainPos;
		bWaterPos				= waterPos;
		bCancelEnabled			= cancelEnabled;

		vLocal					= vLocalPos;
		vScale					= scale;
		matPostMatrix			= matPost;
	}

	void SetWaterHeight(float fMin, float fMax) { fWaterHeightMin = fMin, fWaterHeightMax = fMax; }
};

struct ENCHANT_DATA
{
	int				nItemSlotID;		// 강화 할 아이템
	int				nAgentSlotID;		// 강화제

	bool			bEquip;

	ENCHANT_DATA(int itemSlotID, int agentSlotID, bool equip)
	{
		nItemSlotID		= itemSlotID;
		nAgentSlotID	= agentSlotID;
		bEquip			= equip;
	}
};

struct ATTUNE_DATA
{
	int				nItemSlotID;
};

struct DYE_DATA
{
	SH_ITEM_SLOT_TYPE		m_nItemSlotType;	///< 염색할 아이템이 존재하는 슬롯타입
	int						m_nItemSlotIndex;	///< 염색할 아이템의 슬롯ID
	int						m_nDyeItemIndex;	///< 염색아이템의 슬롯ID

	DYE_DATA()
	{
		m_nItemSlotType	= SLOTTYPE_NONE;
		m_nItemSlotIndex= -1;
		m_nDyeItemIndex = -1;
	}

	DYE_DATA(int nItemType, int nItemIndex, int nDyeItemIndex)
	{
		m_nItemSlotType	= (nItemType == 1 ? SLOTTYPE_INVENTORY : SLOTTYPE_EQUIP);
		m_nItemSlotIndex= nItemIndex;
		m_nDyeItemIndex = nDyeItemIndex;
	}

	DYE_DATA& operator = (const DYE_DATA& dye_data)
	{
		m_nItemSlotType	= dye_data.m_nItemSlotType;
		m_nItemSlotIndex= dye_data.m_nItemSlotIndex;
		m_nDyeItemIndex	= dye_data.m_nDyeItemIndex;
		return *this;
	}
};


struct BPART_ACTOR_NODE_INFO
{
	std::string		strActorNodeName;
	vec3			vTargetPos;
};