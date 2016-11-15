#ifndef _XMODULE_EFFECT_RESERVE_H
#define _XMODULE_EFFECT_RESERVE_H

#include "XModuleEffect.h"

//////////////////////////////////////////////////////////////////////////
/// 이펙트 대기 관리
/// 이후 XModuleEffect Update에서 처리
class XModuleEffectReserve
{
protected:
	//XEffectID				m_eID;

public:
	XModuleEffectReserve() {}
	virtual ~XModuleEffectReserve() {}
};

// Caption
class XCaptionEffectReserve : public XModuleEffectReserve
{
public:
	GUARD_TYPE				m_eGuardType;

	int						m_nParam1;

	wstring					m_string1;

	bool					m_bIsDebuff;
	uint32					m_nColor;

public:
	XCaptionEffectReserve() {}
	virtual ~XCaptionEffectReserve() {}
};

// Damage
class XDamageEffectReserve : public XModuleEffectReserve
{
public:
	//_DamageInfo *			m_pInfo; 
	XObject *				m_pAttacker;

	vec3					m_vOffset;

public:
	XDamageEffectReserve() {}
	virtual ~XDamageEffectReserve() {}
};

#endif // #ifndef _XMODULE_EFFECT_RESERVE_H