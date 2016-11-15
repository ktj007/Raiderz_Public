#include "StdAfx.h"
#include "GTalentFocus_Enlighten.h"
#include "GConst.h"
#include "GEntityPlayer.h"

GTalentFocus_Enlighten::GTalentFocus_Enlighten(GEntityPlayer* pOnwer)
: GTalentFocus(pOnwer)
{
	AddEnableWeapon(WEAPON_STAFF);
	AddEnableWeapon(WEAPON_1H_SLASH);
}

GTalentFocus_Enlighten::~GTalentFocus_Enlighten(void)
{
}

bool GTalentFocus_Enlighten::CheckActivate()
{
	return true;
}
