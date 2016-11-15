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
bool SCharacterInfoChecker::IsValidHair(SEX eSex, int8 nHair)
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
bool SCharacterInfoChecker::IsValidFace(SEX eSex, int8 nFace)
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
bool SCharacterInfoChecker::IsValidSex(int8 nSex)
{
	if (SEX_NA < nSex && nSex < SEX_MAX) return true;
	return false;
}

bool SCharacterInfoChecker::IsValidVoice( SEX eSex, int8 nVoice )
{
	if (0 > nVoice) return false;

	if (SEX_MALE == eSex)
	{
		if (MAX_CREATION_PLAYER_VOICE_MALE <= nVoice)
			return false;
	}
	else if (SEX_FEMALE == eSex)
	{
		if (MAX_CREATION_PLAYER_VOICE_FEMALE <= nVoice)
			return false;
	}
	return true;
}

bool SCharacterInfoChecker::IsValidTalentStyle( TALENT_STYLE nTalentStyle )
{
	if (TS_NONE > nTalentStyle) return false;
	if (TS_MAX <= nTalentStyle) return false;

	return true;
}

bool SCharacterInfoChecker::CheckAllParam_InsertCharacter(const TD_INSERT_CHARACTER_INFO& info)
{
	SEX eSex = static_cast<SEX>(info.nSex);

	if (false == IsValidSex(eSex))						return false;
	if (false == IsValidHair(eSex, info.nHair))			return false;
	if (false == IsValidFace(eSex, info.nFace))			return false;
	if (false == IsValidVoice(eSex, info.nVoice))		return false;
	if (false == IsValidTalentStyle(info.nTalentStyle))	return false;
	if (false == IsValidMakeUp(eSex, info.nFace))		return false;
	if (false == IsValidTattooType(eSex, info.nVoice))	return false;
	if (false == IsValidTattooScale(info.nTalentStyle))	return false;

	return true;
}

bool SCharacterInfoChecker::IsValidMakeUp(SEX eSex, int8 nMakeUp)
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

bool SCharacterInfoChecker::IsValidTattooType(SEX eSex, int8 nTattooType)
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

bool SCharacterInfoChecker::IsValidTattooScale( int8 nTattooScale )
{
	if (0 > nTattooScale) return false;

	return true;
}
