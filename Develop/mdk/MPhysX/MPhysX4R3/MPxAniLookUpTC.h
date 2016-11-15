#pragma once

#include "MPxTransformController.h"

namespace physx
{

class MPxAniLookUpTC : public MPxTcImpl<MPxAniLookUpTC>
{
public:
						MPxAniLookUpTC(MPxTcEntity* pTcEntity);
	virtual				~MPxAniLookUpTC(){}

	virtual void		Update(float fDelta){}
	virtual void		Stop(){}

	bool				HookOnAnimationSetting(string& strAniName);
	void				SetEnableCollisionTcHooking(bool bEnable){ m_bCollisionTcHooking = bEnable; }

private:
	bool				m_bCollisionTcHooking;
};

}
