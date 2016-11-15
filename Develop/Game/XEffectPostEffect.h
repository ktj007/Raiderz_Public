#pragma once

#include "XPostEffect.h"

class XEffectPostEffect : public XPostEffect
{
private:
	struct stPOSTEFFECT
	{
		MUID				uidUser;

		float				fPlayTime;
		BYTE				byPostEffectType;

		bool				bLoop;

		stPOSTEFFECT()
		{
			uidUser			= MUID::ZERO;
			fPlayTime		= -1.0f;
			byPostEffectType = 0;
			bLoop			= false;
		}
	};

	vector<stPOSTEFFECT>	m_vecPostEffect;

private:
	void					StartMotionBlur(float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType);
	void					StartObjectMotionBlur(XObject* pObject, float fPlayTime, float fSpeed, bool bActive);
	void					StartObjectAccumBlur(XObject* pObject, float fPlayTime, bool bActive);
	void					StartPixelObjectMotionBlur(XObject* pObject, float fPlayTime, float fSpeed, int nBlurType);

	bool					CheckUsePostEffect(POST_EFFECT_KIND eType);

	void					CheckMotionBlurActive();
	void					CheckObjectAccumBlurActive();
	void					CheckPixelObjectMotionBlureActive();

protected:
	virtual RCameraSceneNode* GetPrimaryCamera();

public:
	XEffectPostEffect(void);
	~XEffectPostEffect(void);

	void					StartPostEffect(BYTE byPostEffectType, XObject* pObject, float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType, int nBlurType);
	void					EndPostEffect(BYTE byPostEffectType, XObject* pObject);
	virtual void			UpdatePostEffect(float fDelta);
};
