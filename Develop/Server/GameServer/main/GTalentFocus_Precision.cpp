#include "StdAfx.h"
#include "GTalentFocus_Precision.h"
#include "GConst.h"
#include "GEntityPlayer.h"

#include "GBuffInfo.h"
#include "GModuleBuff.h"

GTalentFocus_Precision::GTalentFocus_Precision(GEntityPlayer* pOnwer)
: GTalentFocus(pOnwer)
{
	AddEnableWeapon(WEAPON_ARCHERY);
}

GTalentFocus_Precision::~GTalentFocus_Precision(void)
{
}

bool GTalentFocus_Precision::CheckActivate()
{
	return true;
}
