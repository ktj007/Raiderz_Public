#ifndef _XALPHA_TEST_FADE_EFFECT_H
#define _XALPHA_TEST_FADE_EFFECT_H

#pragma once

#include "XEffectBase.h"

class XAlphaTestFadeEffect : public XEffectBase
{
protected:
	MUID				m_uidOwer;

	float				m_fStartDurationTime;
	float				m_fEndDurationTime;

	float				m_fCurrentTime;
	float				m_fD;

	DWORD				m_dwColor1;
	DWORD				m_dwColor2;
	
	float				m_fSize1;
	float				m_fSize2;

	bool				m_bInverse;

	vector<tstring>		m_vecMeshNodeName;	

private:
	float				FadeUpdate(float fDelta);

public:
	XAlphaTestFadeEffect();
	virtual ~XAlphaTestFadeEffect();

	bool				Create(MUID& uid, float fStartDurationTime, float fEndDurationTime);
	virtual void		UpdateXEffect(float fDelta);

	virtual bool		IsEffectLive() { return true; }

	void				SetOwner(MUID uidOwner) { m_uidOwer = uidOwner; }
	virtual void		SetFadeEffectColor(DWORD& dwColor1, DWORD& dwColor2, float& fSize1, float& fSize2);
	void				SetInverse(bool bInverse)	{ m_bInverse = bInverse; }
	void				SetMeshNodeName(vector<tstring>& list)		{ m_vecMeshNodeName = list; }


	virtual EFFECT_TYPE	GetType()	{ return ET_ALPHATESTFADE; }
};

#endif // _XALPHA_TEST_FADE_EFFECT_H