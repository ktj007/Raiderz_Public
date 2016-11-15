#pragma once

#include "XMotion.h"
#include "RAnimationResource.h"
#include "RMesh.h"
#include "XMotionTimeModifier.h"

class XGameTransformControllerManager;

class XUpperMotionTimeModifier : public XMotionTimeModifier
{
public:
	XUpperMotionTimeModifier(RActor* pActor, XGameTransformControllerManager* pAnimationController);
	~XUpperMotionTimeModifier() {}

	virtual void SetAnimationSpeed(int nCurrAniSequence);
	virtual void SetSpeed( float fSpeed, int nCurrAniSequence );
	virtual float GetSpeed();
	virtual float GetMotionTime();
	virtual void ResetAniTime();
};
