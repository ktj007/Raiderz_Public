#pragma once

#include "XMotion.h"
#include "RAnimationResource.h"
#include "RMesh.h"

class XGameTransformControllerManager;

class XMotionTimeModifier
{
protected:
	XGameTransformControllerManager*	m_pAnimationController;
	RActor*					m_pActor;
protected:
	bool					m_bAdjustPlayTime;
	int						m_nAdjustAniIndex;
	unsigned int			m_nAdjustPlayTime;
	float					m_fAdjustAniSpeed;
	unsigned int			m_nAdjustMotionPlayTime;
	unsigned int			m_nCurrentMotionLength;

	bool CalcAnimationSpeed(float& foutSpeed, float fAniMaxFrame, int nCurrAniSequence);
public:
	XMotionTimeModifier(RActor* pActor, XGameTransformControllerManager* pAnimationController);
	~XMotionTimeModifier() {}

	virtual void SetAnimationSpeed(int nCurrAniSequence);

	void SetAnimationTime( int nAnimationIndex, unsigned int nPlayTime, int nCurrAniSequence );
	virtual void SetSpeed( float fSpeed, int nCurrAniSequence );
	virtual float GetSpeed();
	virtual float GetOrgSpeed();
	void SetMotionTime( unsigned int nPlayTime, int nCurrAniSequence );
	virtual float GetMotionTime();
	virtual void ResetAniTime();

	void CalcMotionLength(XMotion* pCurrMotion, XMotionTypes nCurrType, bool bUseWeaponAni, WEAPON_TYPE	nWeaponType);
};
