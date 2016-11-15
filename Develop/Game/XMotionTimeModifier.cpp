#include "stdafx.h"
#include "XMotionTimeModifier.h"
#include "XGameTransformControllerManager.h"

////
XMotionTimeModifier::XMotionTimeModifier( RActor* pActor, XGameTransformControllerManager* pAnimationController ) 
: m_pActor(pActor), m_pAnimationController(pAnimationController)
, m_bAdjustPlayTime(false), m_nAdjustPlayTime(0)
, m_nAdjustAniIndex(0), m_fAdjustAniSpeed(1.0f), m_nAdjustMotionPlayTime(0)
, m_nCurrentMotionLength(0)
{

}

bool XMotionTimeModifier::CalcAnimationSpeed( float& foutSpeed, float fAniMaxFrame, int nCurrAniSequence )
{
	// 속도 조절
	if (m_bAdjustPlayTime)
	{
		// 속도로 설정
		if (fabs(m_fAdjustAniSpeed - 1.0f) > FLT_EPSILON)
		{
			foutSpeed = m_fAdjustAniSpeed;
			return true;
		}
		else if (m_nAdjustMotionPlayTime != 0)
		{
			if (m_nCurrentMotionLength != 0)
			{
				foutSpeed = (float)m_nCurrentMotionLength / (float)m_nAdjustMotionPlayTime;
				return true;
			}
		}
		else // 시간으로 설정
		{
			float fMaxTime = fAniMaxFrame / (float)ANIMATION_FRAME_PER_SEC * 1000.0f;
			float fSpeed = (fMaxTime) / (float)m_nAdjustPlayTime;

			if (nCurrAniSequence == m_nAdjustAniIndex)
			{
				foutSpeed = fSpeed;
				return true;
			}
		}
	}

	return false;
}

void XMotionTimeModifier::SetAnimationTime( int nAnimationIndex, unsigned int nPlayTime, int nCurrAniSequence )
{
	m_bAdjustPlayTime = true;
	m_nAdjustAniIndex = nAnimationIndex;
	m_nAdjustPlayTime = nPlayTime;
	m_fAdjustAniSpeed = 1.0f;
	m_nAdjustMotionPlayTime = nPlayTime * 4800 / 1000;

	SetAnimationSpeed(nCurrAniSequence);
}

void XMotionTimeModifier::SetSpeed( float fSpeed, int nCurrAniSequence )
{
	m_bAdjustPlayTime = true;
	m_nAdjustAniIndex = 0;
	m_nAdjustPlayTime = 0;
	m_fAdjustAniSpeed = fSpeed;
	m_nAdjustMotionPlayTime = 0;

//	SetAnimationSpeed(nCurrAniSequence);
	m_pAnimationController->SetAnimationSpeed( fSpeed);
}

float XMotionTimeModifier::GetSpeed()
{
	return m_pAnimationController->GetAnimationSpeed();
}

void XMotionTimeModifier::SetMotionTime( unsigned int nPlayTime, int nCurrAniSequence )
{
	m_bAdjustPlayTime = true;
	m_nAdjustAniIndex = 0;
	m_nAdjustPlayTime = 0;
	m_fAdjustAniSpeed = 1.0f;
	m_nAdjustMotionPlayTime = nPlayTime * 4800 / 1000;

	SetAnimationSpeed(nCurrAniSequence);
}

float XMotionTimeModifier::GetMotionTime()
{
	if ( m_bAdjustPlayTime)
		return (float)m_nAdjustMotionPlayTime;

	return m_pAnimationController->GetMaxFrameToSecond();
}

void XMotionTimeModifier::ResetAniTime()
{
	if (m_bAdjustPlayTime)
	{
		m_pAnimationController->SetAnimationSpeed(1.0f);	//1.0f 로 세팅한것은 초기화란 의미. 각 애니메이션의 속도를 넣어줘야한다.

		m_bAdjustPlayTime = false;
		m_nAdjustAniIndex = 0;
		m_nAdjustPlayTime = 0;
		m_fAdjustAniSpeed = 1.0f;
		m_nAdjustMotionPlayTime = 0;
	}
}

void XMotionTimeModifier::SetAnimationSpeed( int nCurrAniSequence )
{
	if (m_nAdjustMotionPlayTime != 0)
	{
		float fSpeed = 1.0f;

		if ( m_pAnimationController  &&  CalcAnimationSpeed( fSpeed, m_pAnimationController->GetAniMaxFrame(), nCurrAniSequence))
		{
			m_pAnimationController->SetAnimationSpeed( fSpeed);
		}
	}
}

void XMotionTimeModifier::CalcMotionLength( XMotion* pCurrMotion, XMotionTypes nCurrType, bool bUseWeaponAni, WEAPON_TYPE nWeaponType )
{
	if (m_pActor == NULL ||
		m_pActor->m_pMesh == NULL) return;

	m_nCurrentMotionLength = 0;

	for (size_t i=0; i<pCurrMotion->m_vecAniSequence[nCurrType].size(); i++)
	{
		vector<XMotionAniSequence*>& v = pCurrMotion->m_vecAniSequence[nCurrType];

		const wchar_t* ani_name = (bUseWeaponAni) ? v[i]->GetAniName(nWeaponType) : v[i]->GetAniName();
		RAnimation* pAni = m_pActor->m_pMesh->m_AniMgr.GetAnimation(MLocale::ConvUTF16ToAnsi(ani_name).c_str());

		if (pAni != NULL)
			m_nCurrentMotionLength += pAni->GetResource()->GetMaxFrame();
	}
}

float XMotionTimeModifier::GetOrgSpeed()
{
	return m_pAnimationController->GetOrgAniamtionSpeed();
}
