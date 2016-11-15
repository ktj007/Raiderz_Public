#ifndef _XSELF_ILLUMINATION_EFFECT_H
#define _XSELF_ILLUMINATION_EFFECT_H

#pragma once

#include "XGlowEffect.h"


class XSelfIlluminationEffect : public XGlowEffect
{
protected:
	virtual void					SetSelfIlluminationUpdate(float fScale);

public:
	XSelfIlluminationEffect();
	virtual ~XSelfIlluminationEffect();

	virtual bool					IsEffectLive();
	virtual void					CheckOwerChangeMotion(MUID uidOwner);

};

#endif // _XSELF_ILLUMINATION_EFFECT_H