#include "StdAfx.h"
#include "GTalentFocus_Advent.h"
#include "GConst.h"
#include "GEntityPlayer.h"

GTalentFocus_Advent::GTalentFocus_Advent(GEntityPlayer* pOnwer)
: GTalentFocus(pOnwer)
{
	AddEnableWeapon(WEAPON_1H_BLUNT);
	AddEnableWeapon(WEAPON_2H_BLUNT);
	AddEnableWeapon(WEAPON_STAFF);
}

GTalentFocus_Advent::~GTalentFocus_Advent(void)
{
}

bool GTalentFocus_Advent::CheckActivate()
{
	return true;
}

float GTalentFocus_Advent::GetDurationTime()
{
	return GConst::FOCUS_ADVENT_DURATION_TIME + (float)m_pOwner->GetPassiveValue(TEPT_FOCUS_ADVENT_EXTENDTIME);
}
