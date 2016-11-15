#pragma once

#include "XMotionLogic.h"

class XUpperMotionLogic : public XMotionLogic
{
protected:
	virtual bool _ChangeMotion(const wchar_t* szMotionName, XMotionTypes nType, bool bForced, bool bPreFrame = false );
	virtual bool SetAnimation(bool bForced=false, bool bPreFrame = false);

	virtual void PlayDone();

public:
	XUpperMotionLogic(XActor* pOwner, XGameTransformControllerManager* pAnimationController, XMotionTimeModifier* pMotionTimeModifier, XMotionIdleMemory* pMotionIdleMemory);
	virtual ~XUpperMotionLogic();

	virtual void Update(float fDelta);

	void		 StopUpperMotion();

	virtual bool IsCurrentMotion(wstring szMotionName);
};