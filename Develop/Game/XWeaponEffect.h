#ifndef _XWEAPON_EFFECT_BASE_H
#define _XWEAPON_EFFECT_BASE_H

#pragma once

#include "XModelEffect.h"
#include "REffectSceneNode.h"

using namespace rs3;

class XWeaponEffect : public XModelEffect
{
protected:
	MUID				m_uidOwer;
	int					m_nPartsSlotType;

public:
	XWeaponEffect();
	virtual ~XWeaponEffect();

	bool				Create(MUID& uid, const wstring& str);
	void				UpdateXEffect(float fDelta);
	void				UpdateWeaponEffect();

	void				SetOwner(MUID uidOwner) { m_uidOwer = uidOwner; }
	void				SetPartsSlotType(int slotType) { m_nPartsSlotType = slotType; }

	virtual EFFECT_TYPE	GetType()	{ return ET_WEAPON; }
};



#endif