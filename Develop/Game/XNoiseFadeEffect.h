#ifndef _XNOSE_FADE_EFFECT_H
#define _XNOSE_FADE_EFFECT_H

#pragma once

#include "XAlphaTestFadeEffect.h"


class XNoiseFadeEffect : public XAlphaTestFadeEffect
{
private:
	void				UpdateFadeFfect(XObject* pObject);
	void				UpdateInverseFadeFfect(XObject* pObject);

public:
	XNoiseFadeEffect();
	virtual ~XNoiseFadeEffect();

	virtual void		UpdateXEffect(float fDelta);

	virtual void		SetFadeEffectColor(DWORD& dwColor1, DWORD& dwColor2, float& fSize1, float& fSize2);
};

#endif // _XNOSE_FADE_EFFECT_H