#include "StdAfx.h"
#include "GTalentFocus_Sneak.h"
#include "GConst.h"

GTalentFocus_Sneak::GTalentFocus_Sneak(GEntityPlayer* pOnwer)
: GTalentFocus(pOnwer)
{
	AddEnableWeapon(WEAPON_1H_PIERCE);
	AddEnableWeapon(WEAPON_DUAL_WIELD);
	AddEnableWeapon(WEAPON_ARCHERY);
}

GTalentFocus_Sneak::~GTalentFocus_Sneak(void)
{
}

bool GTalentFocus_Sneak::CheckActivate()
{
	return true;
}
