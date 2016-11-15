#include "StdAfx.h"
#include "GEquipmentLimiter.h"
#include "GDef.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"

GEquipmentLimiter::GEquipmentLimiter(GItemHolder* pItemHolder)
: m_pItemHolder(pItemHolder)
{
	assert(m_pItemHolder);
}

GEquipmentLimiter::~GEquipmentLimiter(void)
{
	// do nothing
}

void GEquipmentLimiter::AddEnableWeapon( WEAPON_TYPE nWeaponType )
{
	VALID(!IsRegisterEnableWeapon(nWeaponType));
	m_setEnableWeapons.insert(nWeaponType);
}

bool GEquipmentLimiter::IsEnable()
{
	if (!IsEnableWeapon())		return false;
	return true;
}

bool GEquipmentLimiter::IsEnableWeapon()
{
	if (m_setEnableWeapons.empty())	
		return true;	// 아무런 제약조건도 등록되어있지 않음

	WEAPON_TYPE nWeaponType = m_pItemHolder->GetEquipment().GetWeaponType();

	return IsRegisterEnableWeapon(nWeaponType);
}

bool GEquipmentLimiter::IsRegisterEnableWeapon( WEAPON_TYPE nWeaponType )
{
	return (m_setEnableWeapons.find(nWeaponType) != m_setEnableWeapons.end());
}
