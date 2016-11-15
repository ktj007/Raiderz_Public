#include "StdAfx.h"
#include "XEffectPostEffect.h"
#include "XCameraManager.h"
#include "XCharacter.h"

XEffectPostEffect::XEffectPostEffect( void )
{
}

XEffectPostEffect::~XEffectPostEffect( void )
{

}

void XEffectPostEffect::StartPostEffect( BYTE byPostEffectType, XObject* pObject, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType )
{
	switch ((POST_EFFECT_KIND)byPostEffectType)
	{
	case PEK_MOTION_BLUR:
		StartMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);
		break;
	case PEK_OBJECT_MOTION_BLUR:
		StartObjectMotionBlur(pObject, fPlayTime, fSpeed, true);
		break;
	case PEK_OBJECT_ACCUM_BLUR:
		StartObjectAccumBlur(pObject, fPlayTime, true);
		break;
	case PEK_PIXEL_OBJECT_MOTION_BLUR:
		StartPixelObjectMotionBlur(pObject, fPlayTime, fSpeed, nBlurType);
		break;
	}
}

void XEffectPostEffect::EndPostEffect( BYTE byPostEffectType, XObject* pObject )
{
	switch ((POST_EFFECT_KIND)byPostEffectType)
	{
	case PEK_MOTION_BLUR:
		//SetMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);
		break;
	case PEK_OBJECT_MOTION_BLUR:
		{
			if(pObject == NULL) 
				return;

			SetObjectMotionBlur(pObject->GetActor(), OBJECTMOTIONBLUR_MAX_SPEED, false);
		}
		break;
	case PEK_OBJECT_ACCUM_BLUR:
		{
			if(pObject == NULL)
				return;

			SetObjectAccumMotionBlur(pObject->GetActor(), false);
		}
		break;
	case PEK_PIXEL_OBJECT_MOTION_BLUR:
		{
			if(pObject == NULL)
				return;

			SetPixelObjectMotionBlur(pObject->GetActor(), OBJECTMOTIONBLUR_MAX_SPEED, PMB_NONE);
		}
		break;
	}
}

void XEffectPostEffect::StartObjectMotionBlur( XObject* pObject, float fPlayTime, float fSpeed, bool bActive )
{
	if(pObject == NULL) 
		return;

	SetObjectMotionBlur(pObject->GetActor(), fSpeed, bActive);

	stPOSTEFFECT posteffectData;
	posteffectData.byPostEffectType = PEK_OBJECT_MOTION_BLUR;
	posteffectData.fPlayTime		= fPlayTime;
	posteffectData.uidUser			= pObject->GetUID();

	m_vecPostEffect.push_back(posteffectData);
}

void XEffectPostEffect::StartMotionBlur( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType )
{
	SetMotionBlur(fPlayTime, fScatTime, fSpeed, nScarFactorType);

	stPOSTEFFECT posteffectData;
	posteffectData.byPostEffectType = PEK_MOTION_BLUR;
	posteffectData.fPlayTime		= fPlayTime;

	// 정책이 필요합니다.
	//if(posteffectData.fPlayTime == -1.0f)
	//	posteffectData.bLoop		= true;

	m_vecPostEffect.push_back(posteffectData);

}

RCameraSceneNode* XEffectPostEffect::GetPrimaryCamera()
{
	if(global.camera == NULL)
		return NULL;

	return global.camera->GetPrimaryCamera();
}

void XEffectPostEffect::CheckMotionBlurActive()
{
	if(rs3::REngine::GetSceneManagerPtr())
	{
		rs3::REngine::GetSceneManagerPtr()->SetScreenMotionBlurActive(false);

		// 모션 블러가 사용중이라면 모션 블러 효과를 Active = true로 줍니다.
		if(CheckUsePostEffect(PEK_MOTION_BLUR))
		{
			rs3::REngine::GetSceneManagerPtr()->SetScreenMotionBlurActive(true);
		}
	}
}

void XEffectPostEffect::UpdatePostEffect( float fDelta )
{
	CheckMotionBlurActive();
	CheckObjectAccumBlurActive();
	CheckPixelObjectMotionBlureActive();

	// 업데이트
	vector<stPOSTEFFECT>::iterator itPost = m_vecPostEffect.begin();
	while(itPost != m_vecPostEffect.end())
	{
		itPost->fPlayTime -= fDelta;

		if(itPost->fPlayTime <= 0.0f && 
			itPost->bLoop == false)
		{
			// 종료
			XObject* pUser = gg.omgr->FindObject(itPost->uidUser);
			if(pUser)
			{
				EndPostEffect(itPost->byPostEffectType, pUser);
			}

			itPost = m_vecPostEffect.erase(itPost);
			continue;
		}

		itPost++;
	}
}

void XEffectPostEffect::CheckObjectAccumBlurActive()
{
	if(rs3::REngine::GetSceneManagerPtr())
	{
		rs3::REngine::GetSceneManagerPtr()->SetAccumMotionBlurActive(false);

		if(rs3::REngine::GetSceneManagerPtr()->IsAccumMotionBlurBeing() ||
			CheckUsePostEffect(PEK_OBJECT_ACCUM_BLUR))
		{
			rs3::REngine::GetSceneManagerPtr()->SetAccumMotionBlurActive(true);
		}
	}
}

void XEffectPostEffect::StartObjectAccumBlur( XObject* pObject, float fPlayTime, bool bActive )
{
	if(pObject == NULL) 
		return;

	SetObjectAccumMotionBlur(pObject->GetActor(), bActive);

	stPOSTEFFECT posteffectData;
	posteffectData.byPostEffectType = PEK_OBJECT_ACCUM_BLUR;
	posteffectData.fPlayTime		= fPlayTime;
	posteffectData.uidUser			= pObject->GetUID();

	m_vecPostEffect.push_back(posteffectData);
}

bool XEffectPostEffect::CheckUsePostEffect( POST_EFFECT_KIND eType )
{
	for(vector<stPOSTEFFECT>::iterator itPost = m_vecPostEffect.begin(); itPost != m_vecPostEffect.end(); itPost++)
	{
		if(itPost->byPostEffectType == eType)
		{
			return true;
		}
	}

	return false;
}

void XEffectPostEffect::CheckPixelObjectMotionBlureActive()
{
	if(rs3::REngine::GetSceneManagerPtr())
	{
		rs3::REngine::GetSceneManagerPtr()->SetPixelMotionBlurActive(false);

		if(CheckUsePostEffect(PEK_PIXEL_OBJECT_MOTION_BLUR))
		{
			rs3::REngine::GetSceneManagerPtr()->SetPixelMotionBlurActive(true);
		}
	}
}

void XEffectPostEffect::StartPixelObjectMotionBlur( XObject* pObject, float fPlayTime, float fSpeed, int nBlurType )
{
	if(pObject == NULL) 
		return;

	SetPixelObjectMotionBlur(pObject->GetActor(), fSpeed, nBlurType);

	stPOSTEFFECT posteffectData;
	posteffectData.byPostEffectType = PEK_PIXEL_OBJECT_MOTION_BLUR;
	posteffectData.fPlayTime		= fPlayTime;
	posteffectData.uidUser			= pObject->GetUID();

	m_vecPostEffect.push_back(posteffectData);
}