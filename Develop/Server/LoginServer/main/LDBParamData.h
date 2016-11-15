#pragma once

#include "STransData.h"
#include "MLocale.h"

struct DBP_CHAR_INSERT
{
	MUID uidPlayer;
	int64 nAID;
	wchar_t szName[PLAYER_NAME_LEN+1];
	uint8 nLevel;
	int nXP;
	int nSilver;
	int nRace;
	int nSex;
	int nFeatureHair;
	int nFeatureFace;
	short nFeatureHairColor;
	short nFeatureSkinColor;
	uint8 nEyeColor;
	uint8 nMakeUp;
	uint8 nTattooType;
	short nTattooPosX;
	short nTattooPosY;
	uint8 nTattooScale;
	int nTalentStyle;
	int nEquipmentIndex;
	int nEquipmentColorIndex;	
	int nHP;
	int nEN;
	int nSTA;
	int8 nTotalTP;
	int nFatigue;
	int nSoulBindingID;
	int nInnRoomID;
	int nPrimaryPalettedID;
	int nSecondaryPalettedID;
	int nSlotOrder;	// °èÁ¤ÀÇ Ä³¸¯ ½½·Ô ÀÎµ¦½º

	void Assign(TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
	{
		wcsncpy_s(this->szName, pInsertCharInfo->szName, _TRUNCATE); 

		this->nSex					= pInsertCharInfo->nSex;
		this->nFeatureHair			= pInsertCharInfo->nHair;
		this->nFeatureFace			= pInsertCharInfo->nFace;
		this->nFeatureHairColor		= pInsertCharInfo->nHairColor;
		this->nFeatureSkinColor		= pInsertCharInfo->nSkinColor;
		this->nEyeColor				= pInsertCharInfo->nEyeColor;

		this->nMakeUp				= pInsertCharInfo->nMakeUp;
		this->nTattooType			= pInsertCharInfo->nTattooType;
		this->nTattooPosX			= pInsertCharInfo->nTattooPosX;
		this->nTattooPosY			= pInsertCharInfo->nTattooPosY;
		this->nTattooScale			= pInsertCharInfo->nTattooScale;

		this->nTalentStyle			= pInsertCharInfo->nTalentStyle;
		this->nEquipmentIndex		= pInsertCharInfo->nEquipmentIndex;
		this->nEquipmentColorIndex	= pInsertCharInfo->nEquipmentColorIndex;		
	}

	DBP_CHAR_INSERT()
	: uidPlayer(MUID::ZERO)
	, nAID(0)
	, nLevel(0)
	, nXP(0)
	, nSilver(0)
	, nRace(0)
	, nSex(0)
	, nFeatureHair(0)
	, nFeatureFace(0)
	, nFeatureHairColor(0)
	, nFeatureSkinColor(0)
	, nEyeColor(0)
	, nMakeUp(0)
	, nTattooType(0)
	, nTattooPosX(0)
	, nTattooPosY(0)
	, nTattooScale(0)
	, nTalentStyle(0)
	, nEquipmentIndex(0)
	, nEquipmentColorIndex(0)	
	, nHP(0)
	, nEN(0)
	, nSTA(0)
	, nTotalTP(0)
	, nFatigue(0)
	, nSoulBindingID(0)
	, nInnRoomID(0)
	, nPrimaryPalettedID(0)
	, nSecondaryPalettedID(0)
	, nSlotOrder(0)
	{
	}
};