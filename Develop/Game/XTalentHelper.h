#pragma once

#include "XTalentInfo.h"

class XObject;

class XTalentHelper
{
public:
	static XTalentInfo* FindTalentInfo(int nTalentID, XObject* pObject=NULL);
	static wstring MakeFullPlayerTalentAniName(const wchar_t* szAniName, CHAR_STANCE nStance, WEAPON_TYPE nCurrWeaponType);
	static MCapsule GetTalentHitCapule(int nTalentID, int nSegIndex, int nCapsuleIndex);
	static MCapsule GetTalentHitCapule(XTalentInfo* pTalentInfo, int nSegIndex, int nCapsuleIndex, MMatrix& matWorld);
	static bool IsTalentCancelableMotionFactor(MF_STATE nMotionFactor);

	static bool CheckPortalTalentUsable(XTalentInfo* pTalentInfo);
};