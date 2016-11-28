#pragma once

#include "CSChrInfo.h"

struct TD_INSERT_CHARACTER_INFO;

class SCharacterInfoChecker
{
public:
	bool IsRemainCharSlot(int nPlayerCharacterCount);
	bool IsValidHair(SEX eSex, int16 nHair);
	bool IsValidFace(SEX eSex, int16 nFace);
	bool IsValidHairColor( SEX eSex, int16 nHairColor );
	bool IsValidSkinColor( SEX eSex, int16 nSkinColor );
	bool IsValidEyeColor( SEX eSex, int16 nEyeColor );
	bool IsValidSex(int8 nSex);
	bool IsValidVoice(SEX eSex, int16 nVoice);
	bool IsValidTalentStyle(TALENT_STYLE nTalentStyle);
	bool IsValidMakeUp(SEX eSex, int16 nMakeUp);
	bool IsValidTattooType(SEX eSex, int16 nTattooType);
	bool IsValidTattooScale(uint8 nTattooScale);
	bool IsValidTattooColor(SEX eSex, int16 nTattooColor);
	bool IsValidEquipmentColor(int8 nEquipmentColor);

	bool CheckAllParam_InsertCharacter(const TD_INSERT_CHARACTER_INFO& info);
};
