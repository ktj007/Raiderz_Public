#ifndef _XLIGHT_EFFECT_BASE_H
#define _XLIGHT_EFFECT_BASE_H

#pragma once

#include "XEffectBase.h"
#include "RPointLightSceneNode.h"

using namespace rs3;

class XLightEffect : public XEffectBase
{
private:
	MUID				m_uidOwer;

protected:
	virtual void		UpdateFollow();
	//virtual void		StopXEffect();

public:
	XLightEffect();
	virtual ~XLightEffect();

	bool				Create(MUID& uid);

	virtual void		UpdateXEffect(float fDelta);

	RPointLightSceneNode*	GetEffect()			{ return static_cast<RPointLightSceneNode*>(m_pEffect); }

	void				SetOwner(MUID uidOwner)		{ m_uidOwer = uidOwner; }
	void				SetRadius(float fRadius);
	void				SetAttenuationStart(float fStartDist);
	void				SetIntensity(float fIntensity);
	void 				SetColorVector(RVector vColor);
	void 				SetColorRGB(DWORD dwColor);
	void				SetWorldMatrix(RMatrix& mat);

	virtual bool		IsEffectLive() { return true; }
/*	virtual bool		IsEffectLoadingComplete();
	virtual bool		IsEffectLoop()*/;

	virtual EFFECT_TYPE	GetType()	{ return ET_LIGHT; }
};



#endif