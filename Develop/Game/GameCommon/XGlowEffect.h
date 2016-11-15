#ifndef _XGLOW_EFFECT_H
#define _XGLOW_EFFECT_H

#pragma once

#include "XEffectBase.h"

class XGlowEffect : public XEffectBase
{
	enum GLOW_SATE
	{
		ILL_READY = 0,
		ILL_START,
		ILL_START_DURATION,
		ILL_END,
		ILL_END_DURATION,
		
		ILL_MAX
	};

private:
	GLOW_SATE						m_eState;

	float							m_fStartTime;
	float							m_fStartDurationTime;
	float							m_fEndTime;
	float							m_fEndDurationTime;
	float							m_fMin;
	float							m_fMax;

	bool							m_bStop;
	bool							m_bLoop;

protected:
	MUID							m_uidOwer;

protected:
	virtual void					StopXEffect();

	virtual void					SetSelfIlluminationUpdate(float fScale) {}

public:
	XGlowEffect();
	virtual ~XGlowEffect();

	bool							Create(MUID& uid, float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax);
	virtual void					UpdateXEffect(float fDelta);

	void							SetOwner(MUID uidOwner);
	void							SetLoop(bool bLoop);

	virtual bool					IsEffectLoop()		{ return true; }
	bool							IsDuplicationEffect(MUID& uidOwner, tstring strActorNodeName);

	virtual EFFECT_TYPE	GetType()	{ return ET_GLOW; }
};

#endif // _XGLOW_EFFECT_H