#include "stdafx.h"
#include "WS_EffectPostEffect.h"

WS_EffectPostEffect::WS_EffectPostEffect()
{

}

WS_EffectPostEffect::~WS_EffectPostEffect()
{

}

RCameraSceneNode* WS_EffectPostEffect::GetPrimaryCamera()
{
	return REngine::GetSceneManager().GetPrimaryCamera();
}

void WS_EffectPostEffect::UpdatePostEffect( float fDelta )
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
				EndPostEffect(m_PostEffect[i].byPostEffectType, m_PostEffect[i].pActor);
			}
		}
	}
}

void WS_EffectPostEffect::StartPostEffect( BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType )
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
		StartPixelObjectAccumBlur(pActor, fPlayTime, fSpeed, true);
		break;
	}
}

void WS_EffectPostEffect::EndPostEffect( BYTE byPostEffectType, RActor* pActor )
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

void WS_EffectPostEffect::CheckMotionBlurActive()
{
	if(REngine::GetSceneManagerPtr())
	{
		REngine::GetSceneManagerPtr()->SetScreenMotionBlurActive(false);
	
		if(m_PostEffect[PEK_MOTION_BLUR - 1].bUse)
		{
			if(REngine::GetSceneManagerPtr()->IsEnableScreenMotionBlur() == false)
				REngine::GetSceneManagerPtr()->SetScreenMotionBlurEnable(true);

			REngine::GetSceneManagerPtr()->SetScreenMotionBlurActive(true);
		}
	}
}

void WS_EffectPostEffect::StartMotionBlur( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType )
{
	SetMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);

	m_PostEffect[PEK_MOTION_BLUR - 1].bUse = true;
	m_PostEffect[PEK_MOTION_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_MOTION_BLUR - 1].byPostEffectType = PEK_MOTION_BLUR;
}

void WS_EffectPostEffect::StartObjectMotionBlur( RActor* pActor, float fPlayTime, float fSpeed, bool bActive )
{
	if(pActor == NULL) 
		return;

	SetObjectMotionBlur(pActor, fSpeed, bActive);

	m_PostEffect[PEK_OBJECT_MOTION_BLUR - 1].bUse = true;
	m_PostEffect[PEK_OBJECT_MOTION_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_OBJECT_MOTION_BLUR - 1].byPostEffectType = PEK_OBJECT_MOTION_BLUR;
}

void WS_EffectPostEffect::CheckObjectAccumBlurActive()
{

 	if(REngine::GetSceneManagerPtr())
 	{
 		REngine::GetSceneManagerPtr()->SetAccumMotionBlurActive(false);
 
 		if(REngine::GetSceneManagerPtr()->IsAccumMotionBlurBeing() ||
 			m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].bUse)
 		{
 			if(REngine::GetSceneManagerPtr()->IsEnableAccumMotionBlur() == false)
 				REngine::GetSceneManagerPtr()->SetAccumMotionBlurEnable(true);
 
 			REngine::GetSceneManagerPtr()->SetAccumMotionBlurActive(true);
 		}
 	}
}

void WS_EffectPostEffect::StartObjectAccumBlur( RActor* pActor, float fPlayTime, bool bActive )
{
	if(pActor == NULL) 
		return;

	SetObjectAccumMotionBlur(pActor, bActive);

	m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].bUse = true;
	m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_OBJECT_ACCUM_BLUR - 1].byPostEffectType = PEK_OBJECT_ACCUM_BLUR;
}

void WS_EffectPostEffect::Clear()
{
	for(int i = 0; i < PEK_MAX - 1; i++)
	{
		if(m_PostEffect[i].bUse)
		{
			// 종료
			m_PostEffect[i].fPlayTime = 0.0f;
			m_PostEffect[i].bUse = false;
			EndPostEffect(m_PostEffect[i].byPostEffectType, m_PostEffect[i].pActor);
		}
	}
}

void WS_EffectPostEffect::StartPixelObjectAccumBlur( RActor* pActor, float fPlayTime, float fSpeed, bool bActive )
{
	if(pActor == NULL) 
		return;

	SetPixelObjectMotionBlur(pActor, fSpeed, bActive);

	m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].bUse = true;
	m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].fPlayTime = fPlayTime;
	m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].byPostEffectType = PEK_PIXEL_OBJECT_MOTION_BLUR;
}

void WS_EffectPostEffect::CheckPixelObjectMotionBlureActive()
{
	if(REngine::GetSceneManagerPtr())
	{
		REngine::GetSceneManagerPtr()->SetPixelMotionBlurActive(false);

		if(m_PostEffect[PEK_PIXEL_OBJECT_MOTION_BLUR - 1].bUse)
		{
			if(REngine::GetSceneManagerPtr()->IsEnablePixelMotionBlur() == false)
				REngine::GetSceneManagerPtr()->SetPixelMotionBlurEnable(true);

			REngine::GetSceneManagerPtr()->SetPixelMotionBlurActive(true);
		}
	}
}