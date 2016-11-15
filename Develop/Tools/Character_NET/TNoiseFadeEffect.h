#ifndef _XNOSE_FADE_EFFECT_H
#define _XNOSE_FADE_EFFECT_H

#pragma once

#include "XAlphaTestFadeEffect.h"


class TNoiseFadeEffect : public XAlphaTestFadeEffect
{
private:
	void				UpdateFadeFfect();
	void				UpdateInverseFadeFfect();

public:
	TNoiseFadeEffect();
	virtual ~TNoiseFadeEffect();

	virtual void		UpdateXEffect(float fDelta);

};

#endif // _XNOSE_FADE_EFFECT_H