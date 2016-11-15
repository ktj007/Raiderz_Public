#ifndef _WS_EFFECT_POST_EFFECT_H
#define _WS_EFFECT_POST_EFFECT_H

#pragma once
#include "../../../Game/GameCommon/XPostEffect.h"

class CORETOOL_SOULHUNT_API WS_EffectPostEffect : public XPostEffect
{
private:
	// 캐릭터툴은 효과가 최대 두개만 나온다.
	struct stPOSTEFFECT
	{
		float				fPlayTime;
		BYTE				byPostEffectType;
		bool				bUse;
		RActor *			pActor;

		stPOSTEFFECT()
		{
			fPlayTime		= -1.0f;
			byPostEffectType = 0;
			bUse			= false;
			pActor			= NULL;
		}
	};

	stPOSTEFFECT			m_PostEffect[PEK_MAX - 1];

private:
	void					CheckMotionBlurActive();
	void					CheckObjectAccumBlurActive();
	void					CheckPixelObjectMotionBlureActive();

	void					StartMotionBlur(float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType);
	void					StartObjectMotionBlur(RActor* pActor, float fPlayTime, float fSpeed, bool bActive);
	void					StartObjectAccumBlur(RActor* pActor, float fPlayTime, bool bActive);
	void					StartPixelObjectAccumBlur(RActor* pActor, float fPlayTime, float fSpeed, bool bActive);

protected:
	virtual RCameraSceneNode* GetPrimaryCamera();

public:
	WS_EffectPostEffect();
	virtual ~WS_EffectPostEffect();

	virtual void			UpdatePostEffect(float fDelta);

	void					StartPostEffect(BYTE byPostEffectType, RActor* pActor, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType);
	void					EndPostEffect(BYTE byPostEffectType, RActor* pActor);
	
	void					Clear();
};

#endif