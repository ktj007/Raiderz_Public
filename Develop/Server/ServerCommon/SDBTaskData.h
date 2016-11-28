#pragma once

#include "CSChrInfo.h"

struct SDBTASK_CHAR_FETATURE_ITEM
{
	SDBTASK_CHAR_FETATURE_ITEM()
	: nItemID(-1)
	, nItemDyedColor(-1)
	, nDefEnchItemID(0)
	{
	}
	int nItemID;
	int nItemDyedColor;
	int nDefEnchItemID;	
};


struct SDBTASK_ACCOUNT_CAHR_FEATURE_LIST
{
	SDBTASK_ACCOUNT_CAHR_FEATURE_LIST() : nCID(0) {}

	CID							nCID;
	RACE						Race;
	SEX							Sex;
	int8						nLevel;
	int							nPlayTime;
	int8						nFeatureHair;
	int8						nFeatureFace;
	int16						nFeatureHairColor;
	int16						nFeatureSkinColor;
	uint8						nEyeColor;
	uint8						nMakeUp;
	uint8						nVoice;
	uint8						nTattooType;
	uint8						nTattooColor;
	short						nTattooPosX;
	short						nTattooPosY;
	uint8						nTattooScale;
	wstring						strName;
	int							nSharedFieldID;
	MUID						uidDynamicFieldGroup;
	int							nDynamicFieldID;
	SDBTASK_CHAR_FETATURE_ITEM	FeatureItem[FEATURE_ITEMSLOT_MAX];	
};
