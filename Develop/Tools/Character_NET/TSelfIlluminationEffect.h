#ifndef _XSELF_ILLUMINATION_EFFECT_H
#define _XSELF_ILLUMINATION_EFFECT_H

#pragma once

#include "XGlowEffect.h"


class TSelfIlluminationEffect : public XGlowEffect
{
protected:
	virtual void					SetSelfIlluminationUpdate(float fScale);

public:
	TSelfIlluminationEffect();
	virtual ~TSelfIlluminationEffect();

	bool							Create(float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax);

	virtual bool					IsEffectLive();

};

#endif // _XSELF_ILLUMINATION_EFFECT_H