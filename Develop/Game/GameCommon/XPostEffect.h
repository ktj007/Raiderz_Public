#ifndef _XPOST_EFFECT_H
#define _XPOST_EFFECT_H

#pragma once

#include "RActor.h"
#include "RCameraSceneNode.h"

#define MOTIONBLUR_MIN_SPEED 1.0f
#define MOTIONBLUR_MAX_SPEED 50.0f
#define OBJECTMOTIONBLUR_MIN_SPEED 5.0f
#define OBJECTMOTIONBLUR_MAX_SPEED 30.0f
#define PIXELOBJECTMOTIONBLUR_MIN_SAMCNT	1
#define PIXELOBJECTMOTIONBLUR_MAX_SAMCNT	4

using namespace rs3;

enum POST_EFFECT_KIND
{
	PEK_NONE,
	PEK_MOTION_BLUR,
	PEK_OBJECT_MOTION_BLUR,
	PEK_OBJECT_ACCUM_BLUR,
	PEK_PIXEL_OBJECT_MOTION_BLUR,

	PEK_MAX
};

class XPostEffect
{
protected:
	void					SetMotionBlur(float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType);
	void					SetObjectMotionBlur(RActor* pActor, float fSpeed, bool bActive);
	void					SetObjectAccumMotionBlur(RActor* pActor, bool bActive);
	void					SetPixelObjectMotionBlur(RActor* pActor, float fSpeed, int nBlurType);

	virtual RCameraSceneNode* GetPrimaryCamera() = 0;

public:
	XPostEffect();
	virtual ~XPostEffect();

	virtual void			UpdatePostEffect(float fDelta) {}
};

#endif // _XPOST_EFFECT_H