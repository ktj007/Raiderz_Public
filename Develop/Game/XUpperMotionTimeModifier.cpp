#include "stdafx.h"
#include "XUpperMotionTimeModifier.h"
#include "XGameTransformControllerManager.h"

////
XUpperMotionTimeModifier::XUpperMotionTimeModifier( RActor* pActor, XGameTransformControllerManager* pAnimationController ) 
: XMotionTimeModifier(pActor, pAnimationController)
{

}

void XUpperMotionTimeModifier::SetSpeed( float fSpeed, int nCurrAniSequence )
{
	m_bAdjustPlayTime = true;
	m_nAdjustAniIndex = 0;
	m_nAdjustPlayTime = 0;
	m_fAdjustAniSpeed = fSpeed;
	m_nAdjustMotionPlayTime = 0;

//	SetAnimationSpeed(nCurrAniSequence);
	m_pAnimationController->SetUpperAnimationSpeed( fSpeed);
}

float XUpperMotionTimeModifier::GetSpeed()
{
	return m_pAnimationController->GetUpperAnimationSpeed();
}

float XUpperMotionTimeModifier::GetMotionTime()
{
	if ( m_bAdjustPlayTime)
		return (float)m_nAdjustMotionPlayTime;

	return m_pAnimationController->GetUpperMaxFrameToSecond();
}

void XUpperMotionTimeModifier::ResetAniTime()
{
	if (m_bAdjustPlayTime)
	{
		m_pAnimationController->SetUpperAnimationSpeed(1.0f);	//1.0f 로 세팅한것은 초기화란 의미. 각 애니메이션의 속도를 넣어줘야한다.

		m_bAdjustPlayTime = false;
		m_nAdjustAniIndex = 0;
		m_nAdjustPlayTime = 0;
		m_fAdjustAniSpeed = 1.0f;
		m_nAdjustMotionPlayTime = 0;
	}
}

void XUpperMotionTimeModifier::SetAnimationSpeed( int nCurrAniSequence )
{
	if (m_nAdjustMotionPlayTime != 0)
	{
		float fSpeed = 1.0f;

		if ( m_pAnimationController  &&  CalcAnimationSpeed( fSpeed, m_pAnimationController->GetUpperAniMaxFrame(), nCurrAniSequence))
		{
			m_pAnimationController->SetUpperAnimationSpeed( fSpeed);
		}
	}
}