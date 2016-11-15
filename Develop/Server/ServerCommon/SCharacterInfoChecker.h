#pragma once

#include "CSChrInfo.h"

struct TD_INSERT_CHARACTER_INFO;

class SCharacterInfoChecker
{
public:
	bool IsRemainCharSlot(int nPlayerCharacterCount);
	bool IsValidHair(SEX eSex, int8 nHair);
	bool IsValidFace(SEX eSex, int8 nFace);
	bool IsValidSex(int8 nSex);
	bool IsValidVoice(SEX eSex, int8 nVoice);
	bool IsValidTalentStyle(TALENT_STYLE nTalentStyle);
	bool IsValidMakeUp(SEX eSex, int8 nMakeUp);
	bool IsValidTattooType(SEX eSex, int8 nTattooType);
	bool IsValidTattooScale(int8 nTattooScale);

	bool CheckAllParam_InsertCharacter(const TD_INSERT_CHARACTER_INFO& info);
};
