#include "StdAfx.h"
#include "SCharacterInfoChecker.h"
#include "CSDef.h"
#include "CTransData.h"


bool SCharacterInfoChecker::IsRemainCharSlot(int nPlayerCharacterCount)
{
	if (nPlayerCharacterCount >= MAX_ACCOUNT_CHAR_COUNT)
		return false;
	return true;
}
bool SCharacterInfoChecker::IsValidHair(SEX eSex, int16 nHair)
{
	if (0 > nHair) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_HAIR_MALE <= nHair)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_HAIR_FEMALE <= nHair)
			return false;
	}
	return true;
}
bool SCharacterInfoChecker::IsValidFace(SEX eSex, int16 nFace)
{
	if (0 > nFace) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_FACE_MALE <= nFace)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_FACE_FEMALE <= nFace)
			return false;
	}
	return true;
}
bool SCharacterInfoChecker::IsValidHairColor(SEX eSex, int16 nHairColor)
{
	if (0 >= nHairColor) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_HAIR_COLOR_MALE < nHairColor)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_HAIR_COLOR_FEMALE < nHairColor)
			return false;
	}
	return true;
}
bool SCharacterInfoChecker::IsValidSkinColor(SEX eSex, int16 nSkinColor)
{
	if (0 >= nSkinColor) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_SKIN_COLOR_MALE < nSkinColor)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_SKIN_COLOR_FEMALE < nSkinColor)
			return false;
	}
	return true;
}
bool SCharacterInfoChecker::IsValidEyeColor(SEX eSex, int16 nEyeColor)
{
	if (0 >= nEyeColor) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_EYE_COLOR_MALE < nEyeColor)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_EYE_COLOR_FEMALE < nEyeColor)
			return false;
	}
	return true;
}
bool SCharacterInfoChecker::IsValidSex(int8 nSex)
{
	if (SEX_MALE == nSex || SEX_FEMALE == nSex) return true;
	return false;
}

bool SCharacterInfoChecker::IsValidVoice( SEX eSex, int16 nVoice )
{
	if (0 >= nVoice) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_VOICE_MALE < nVoice)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_VOICE_FEMALE < nVoice)
			return false;
	}
	return true;
}

bool SCharacterInfoChecker::IsValidTalentStyle( TALENT_STYLE nTalentStyle )
{
	if (TS_DEFENDER == nTalentStyle ||
		TS_BERSERKER == nTalentStyle ||
		TS_ASSASSIN == nTalentStyle ||
		TS_SORCERER == nTalentStyle ||
		TS_CLERIC == nTalentStyle)
	return true;

	return false;
}

bool SCharacterInfoChecker::CheckAllParam_InsertCharacter(const TD_INSERT_CHARACTER_INFO& info)
{
	SEX eSex = static_cast<SEX>(info.nSex);

	if (false == IsValidSex(eSex))						return false;
	if (false == IsValidHair(eSex, info.nHair))			return false;
	if (false == IsValidFace(eSex, info.nFace))			return false;
	if (false == IsValidHairColor(eSex, info.nHairColor))			return false;
	if (false == IsValidSkinColor(eSex, info.nSkinColor))			return false;
	if (false == IsValidEyeColor(eSex, info.nEyeColor))				return false;
	if (false == IsValidVoice(eSex, info.nVoice))		return false;
	if (false == IsValidTalentStyle(info.nTalentStyle))	return false;
	if (false == IsValidMakeUp(eSex, info.nMakeUp))					return false;
	if (false == IsValidTattooType(eSex, info.nTattooType))			return false;
	if (false == IsValidTattooScale(info.nTattooScale))				return false;
	//if (false == IsValidTattooColor(eSex, info.nTattooColor))		return false;
	if (false == IsValidEquipmentColor(info.nEquipmentColorIndex))	return false;

	return true;
}

bool SCharacterInfoChecker::IsValidMakeUp(SEX eSex, int16 nMakeUp)
{
	if (0 > nMakeUp) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_MAKEUP_MALE <= nMakeUp)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_MAKEUP_FEMALE <= nMakeUp)
			return false;
	}
	return true;
}

bool SCharacterInfoChecker::IsValidTattooType(SEX eSex, int16 nTattooType)
{
	if (0 > nTattooType) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_TATTO_MALE <= nTattooType)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_TATTO_FEMALE <= nTattooType)
			return false;
	}
	return true;
}

bool SCharacterInfoChecker::IsValidTattooScale( uint8 nTattooScale )
{
	if (MIN_CREATION_PLAYER_TATTOO_SCALE > nTattooScale || 
		MAX_CREATION_PLAYER_TATTOO_SCALE < nTattooScale) 
		return false;
	return true;
}

bool SCharacterInfoChecker::IsValidTattooColor( SEX eSex, int16 nTattooColor )
{
	if (0 >= nTattooColor) return false;

	if (SEX_MALE == eSex)
{
		if (MAX_CREATION_PLAYER_TATTO_COLOR_MALE < nTattooColor)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_TATTO_COLOR_FEMALE < nTattooColor)
			return false;
	}
	return true;
}

bool SCharacterInfoChecker::IsValidEquipmentColor(int8 nEquipmentColor)
{
	if (0 >= nEquipmentColor || MAX_CREATION_PLAYER_EQUIPMENT_COLOR < nEquipmentColor)
		return false;
	return true;
}
