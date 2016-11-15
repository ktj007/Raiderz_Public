#pragma once

#include "CSChrInfo.h"

struct XPlayerInfoFeature
{
	SEX		nSex;
	int		nHair;
	int		nFace;
	short	nHairColor;
	short	nSkinColor;
	short	nEyeColor;
	int		nMakeUp;
	int		nEquipedItemID[FEATURE_ITEMSLOT_MAX];
	int		nEquipedItemColor[FEATURE_ITEMSLOT_MAX];
	int		nEquipedItemEnchantBuff[FEATURE_ITEMSLOT_MAX];
	int8	nWeaponSet;
	int		nTattoo;
	float	fTattooScale;
	vec2	vTattooPos;
	wchar_t	szGuildName[GUILD_NAME_LEN+1];

	static SH_FEATURE_ITEMSLOT TransItemSlotToNetSlot(SH_ITEM_SLOT nSlot)
	{
		switch (nSlot)
		{
		case ITEMSLOT_NONE: return FEATURE_ITEMSLOT_NONE;
		case ITEMSLOT_HEAD: 
		case ITEMSLOT_LOOK_HEAD: return FEATURE_ITEMSLOT_HEAD; 
		case ITEMSLOT_HANDS: 
		case ITEMSLOT_LOOK_HANDS: return FEATURE_ITEMSLOT_HANDS; 
		case ITEMSLOT_FEET:
		case ITEMSLOT_LOOK_FEET: return FEATURE_ITEMSLOT_FEET; 
		case ITEMSLOT_BODY:
		case ITEMSLOT_LOOK_BODY: return FEATURE_ITEMSLOT_BODY; 
		case ITEMSLOT_LEG:
		case ITEMSLOT_LOOK_LEG: return FEATURE_ITEMSLOT_LEG; 
		case ITEMSLOT_FACE: return FEATURE_ITEMSLOT_FACE; 
		case ITEMSLOT_LWEAPON: return FEATURE_ITEMSLOT_LWEAPON; 
		case ITEMSLOT_RWEAPON: return FEATURE_ITEMSLOT_RWEAPON; 
		case ITEMSLOT_LWEAPON2: return FEATURE_ITEMSLOT_LWEAPON2; 
		case ITEMSLOT_RWEAPON2: return FEATURE_ITEMSLOT_RWEAPON2; 
		}
		_ASSERT(0);
		return FEATURE_ITEMSLOT_HEAD; 
	}

	XPlayerInfoFeature()
	{
		Init();
	}

	XPlayerInfoFeature(TD_PLAYER_FEATURE& td_feature)
	{
		this->nSex			= td_feature.nSex;
		this->nHair			= td_feature.nHair;		
		this->nFace			= td_feature.nFace;
		this->nHairColor	= td_feature.nHairColor;
		this->nSkinColor	= td_feature.nSkinColor;
		this->nEyeColor		= td_feature.nEyeColor;
		this->nMakeUp		= td_feature.nMakeUp;
		this->nWeaponSet	= td_feature.nWeaponSet;
		this->nTattoo		= 0;

		wcsncpy_s(szGuildName, td_feature.szGuildName, _TRUNCATE);

		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		{
			this->nEquipedItemID[i] = td_feature.nItemID[i];
			this->nEquipedItemColor[i] = td_feature.nItemID_DyedColor[i];
			this->nEquipedItemEnchantBuff[i] = td_feature.nItemID_EnchantBuff[i];
		}
	}

	void SetTattooInfo( TD_PLAYER_FEATURE_TATTOO& ti)
	{
		this->nTattoo = ti.nTattooType;
		this->fTattooScale = (float)ti.nTattooScale * 0.02f;
		this->vTattooPos.x = (float)ti.nTattooPosX * 0.01f;
		this->vTattooPos.y = (float)ti.nTattooPosY * 0.01f;
	}

	void Init()
	{
		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		{
			nEquipedItemID[i] = 0;
			nEquipedItemColor[i] = -1;
			nEquipedItemEnchantBuff[i] = ENCHANT_UNUSED_SLOT;
		}
		nHair = 0;
		nFace = 0;
		nHairColor = 0;
		nSkinColor = 0;
		nEyeColor = 0;
		nMakeUp = 0;
		nWeaponSet = 0;
		nTattoo = 0;
	}

	TD_PLAYER_FEATURE AsTransData()
	{
		TD_PLAYER_FEATURE ret_td_feature;
		ret_td_feature.nSex = this->nSex;
		ret_td_feature.nHair = this->nHair;
		ret_td_feature.nFace = this->nFace;
		ret_td_feature.nHairColor = this->nHairColor;
		ret_td_feature.nSkinColor = this->nSkinColor;
		ret_td_feature.nEyeColor = this->nEyeColor;
		ret_td_feature.nMakeUp = this->nMakeUp;
		ret_td_feature.nWeaponSet = this->nWeaponSet;

		wcsncpy_s(ret_td_feature.szGuildName, this->szGuildName, _TRUNCATE);

		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		{
			ret_td_feature.nItemID[i] = this->nEquipedItemID[i];
			ret_td_feature.nItemID_DyedColor[i] = 0;
			ret_td_feature.nItemID_EnchantBuff[i] = this->nEquipedItemEnchantBuff[i];
		}
		return ret_td_feature;
	}

	TD_PLAYER_FEATURE_TATTOO AsTransTattooData()
	{
		TD_PLAYER_FEATURE_TATTOO ret;
		ret.nTattooType = this->nTattoo;
		ret.nTattooScale = (short)( this->fTattooScale * 50.0f);
		ret.nTattooPosX = (short)( this->vTattooPos.x * 100.0f);
		ret.nTattooPosY = (short)( this->vTattooPos.y * 100.0f);
		return ret;
	}
};
