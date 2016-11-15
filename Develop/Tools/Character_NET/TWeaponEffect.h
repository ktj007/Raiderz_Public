#ifndef _TWEAPON_EFFECT_BASE_H
#define _TWEAPON_EFFECT_BASE_H

#pragma once

#include "XModelEffect.h"
#include "REffectSceneNode.h"

using namespace rs3;

class TWeaponEffect : public XModelEffect
{
private:
	int					m_nWeaponEffectIndex;
	TCharacter*			m_pCharacter;

public:
	TWeaponEffect();
	virtual ~TWeaponEffect();

	bool				Create(MUID& uid, const string& str, int nIndex);
	void				UpdateXEffect(float fDelta);
	void				UpdateWeaponEffect();

	void				SetActor(TCharacter* pCharacter) { m_pCharacter = pCharacter;}

	virtual EFFECT_TYPE	GetType()	{ return ET_WEAPON; }
};



#endif