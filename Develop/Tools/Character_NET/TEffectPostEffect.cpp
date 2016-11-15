#include "stdafx.h"
#include "TEffectPostEffect.h"

TEffectPostEffect::TEffectPostEffect()
{

}

TEffectPostEffect::~TEffectPostEffect()
{

}

RCameraSceneNode* TEffectPostEffect::GetPrimaryCamera()
{
	return g_pMainApp->GetCamera();
}

void TEffectPostEffect::UpdatePostEffect( float fDelta )
{
	CheckMotionBlurActive();
	CheckObjectAccumBlurActive();
	CheckPixelObjectMotionBlureActive();

	// 업데이트
	for(int i = 0; i < PEK_MAX - 1; i++)
	{
		if(m_PostEffect[i].bUse)
		{
			m_PostEffect[i].fPlayTime -= fDelta;

			if(m_PostEffect[i].fPlayTime <= 0.0f)
			{
				// 종료
				m_PostEffect[i].fPlayTime = 0;
				m_PostEffect[i].bUse = false;
				EndPostEffect(m_PostEffect[i].byPostEffectType, g_pMainApp->GetActor());
			}
		}
	}
}

void TEffectPostEffect::StartPostEffect( BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType )
{
	switch ((POST_EFFECT_KIND)byPostEffectType)
	{
	case PEK_MOTION_BLUR:
		StartMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);
		break;
	case PEK_OBJECT_MOTION_BLUR:
		StartObjectMotionBlur(pActor, fPlayTime, fSpeed, true);
		break;
	case PEK_OBJECT_ACCUM_BLUR:
		StartObjectAccumBlur(pActor, fPlayTime, true);
		break;
	case PEK_PIXEL_OBJECT_MOTION_BLUR:
		StartPixelObjectAccumBlur(pActor, fPlayTime, fSpeed, nBlurType);
		break;
	}
}

void TEffectPostEffect::EndPostEffect( BYTE byPostEffectType, RActor* pActor )
{
	switch ((POST_EFFECT_KIND)byPostEffectType)
	{
	case PEK_MOTION_BLUR:
		//SetMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);
		break;
	case PEK_OBJECT_MOTION_BLUR:
		{
			if(pActor == NULL) 
				return;

			SetObjectMotionBlur(pActor, OBJECTMOTIONBLUR_MAX_SPEED, false);
		}
		break;
	case PEK_OBJECT_ACCUM_BLUR:
		{
			if(pActor == NULL) 
				return;

			SetObjectAccumMotionBlur(pActor, false);
		}
		break;
	case PEK_PIXEL_OBJECT_MOTION_BLUR:
		{
			if(pActor == NULL)
				return;

			SetPixelObjectMotionBlur(pActor, OBJECTMOTIONBLUR_MAX_SPEED, false);
		}
		break;
	}
}

void TEffectPostEffect::CheckMotionBlurActive()
{
	if( rs3::REngine::GetSceneManagerPtr())
	{
		rs3::REngine::GetSceneManagerPtr()->SetScreenMotionBlurActive(false);
	
		if(m_PostEffect[PEK_MOTION_BLUR - 1].bUse)
		{
			if(rs3::REngine::GetSceneManagerPtr()->IsEnableScreenMotionBlur() == false)
				rs3::REngine::GetSceneManagerPtr()->SetScreenMotionBlurEnable(true);

			rs3::REngine::GetSceneManagerPtr()->SetScreenMotionBlurActive(true);
		}
	}
}

void TEffectPostEffect::StartMotionBlur( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType )
{
	SetMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);

	m_PostEffect[PEK_MOTION_BLUR - 1].bUse = true;
	m_PostEffect[PEK_MOTION_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_MOTION_BLUR - 1].byPostEffectType = PEK_MOTION_BLUR;
}

void TEffectPostEffect::StartObjectMotionBlur( RActor* pActor, float fPlayTime, float fSpeed, bool bActive )
{
	if(pActor == NULL) 
		return;

	SetObjectMotionBlur(pActor, fSpeed, bActive);

	m_PostEffect[PEK_OBJECT_MOTION_BLUR - 1].bUse = true;
	m_PostEffect[PEK_OBJECT_MOTION_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_OBJECT_MOTION_BLUR - 1].byPostEffectType = PEK_OBJECT_MOTION_BLUR;
}

void TEffectPostEffect::CheckObjectAccumBlurActive()
{
	if(rs3::REngine::GetSceneManagerPtr())
	{
		rs3::REngine::GetSceneManagerPtr()->SetAccumMotionBlurActive(false);

		if(rs3::REngine::GetSceneManagerPtr()->IsAccumMotionBlurBeing() ||
			m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].bUse)
		{
			if(rs3::REngine::GetSceneManagerPtr()->IsEnableAccumMotionBlur() == false)
				rs3::REngine::GetSceneManagerPtr()->SetAccumMotionBlurEnable(true);

			rs3::REngine::GetSceneManagerPtr()->SetAccumMotionBlurActive(true);
		}
	}
}

void TEffectPostEffect::StartObjectAccumBlur( RActor* pActor, float fPlayTime, bool bActive )
{
	if(pActor == NULL) 
		return;

	SetObjectAccumMotionBlur(pActor, bActive);

	m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].bUse = true;
	m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].byPostEffectType = PEK_OBJECT_ACCUM_BLUR;
}

void TEffectPostEffect::Clear()
{
	for(int i = 0; i < PEK_MAX - 1; i++)
	{
		if(m_PostEffect[i].bUse)
		{
			// 종료
			m_PostEffect[i].fPlayTime = 0.0f;
			m_PostEffect[i].bUse = false;
			EndPostEffect(m_PostEffect[i].byPostEffectType, g_pMainApp->GetActor());
		}
	}
}

void TEffectPostEffect::StartPixelObjectAccumBlur( RActor* pActor, float fPlayTime, float fSpeed, int nBlurType )
{
	if(pActor == NULL) 
		return;

	SetPixelObjectMotionBlur(pActor, fSpeed, nBlurType);

	m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].bUse = true;
	m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].byPostEffectType = PEK_PIXEL_OBJECT_MOTION_BLUR;
}

void TEffectPostEffect::CheckPixelObjectMotionBlureActive()
{
	if(rs3::REngine::GetSceneManagerPtr())
	{
		rs3::REngine::GetSceneManagerPtr()->SetPixelMotionBlurActive(false);

		if(m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].bUse)
		{
			if(rs3::REngine::GetSceneManagerPtr()->IsEnablePixelMotionBlur() == false)
				rs3::REngine::GetSceneManagerPtr()->SetPixelMotionBlurEnable(true);

			rs3::REngine::GetSceneManagerPtr()->SetPixelMotionBlurActive(true);
		}
	}
}