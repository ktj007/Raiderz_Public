#pragma once
#ifndef _XSWORDTRAIL_EFFECT_CONTROLLER_H
#define _XSWORDTRAIL_EFFECT_CONTROLLER_H

#include "XSwordTrailEffect.h"

#define SWORDTRAIL_FRAME_GAP_CONTROL	0.08f
#define SWORDTRAIL_DISAPPEAR_TIME		100
#define SWORDTRAIL_MAX_FRAME_SEC		2.0f

class XSwordTrailEffectController : public XObject, public MMemPool<XSwordTrailEffectController>
{
	MDeclareRTTI;

private:
	XSwordTrailEffect		m_SwordTrailEffect;

	MUID					m_uidUser;

	bool					m_bShow;
	bool					m_bFadeHide;

	int						m_nFrameGapControl;
	int						m_nMaxFrame;
	int						m_nStartFrame;
	int						m_nEndFrame;
	float					m_fMaxFrameSec;
	float					m_fDisappearTime;
	DWORD					m_dwFadeValue;

private:
	void					UpdateAppearanceFadeHide(float fDelta);

	void					DestroySwordTrail();

protected:
	virtual void			OnRender();
	virtual void			OnUpdate(float fDelta);

public:
	XSwordTrailEffectController(MUID uid);
	virtual ~XSwordTrailEffectController();
	virtual XObjectID				GetType() { return XOBJ_SWORD_TRAIL; }

	void					CreateSwordTrail(MUID uidUser, SWORDTRAILSAMPLING_DATA* pSamplingData, wstring strSwordTrailTexture);
	void					DoneSwordTrail();

	bool					IsDone() { return (m_bShow == true && m_bFadeHide == false); }
	bool					IsStart() { return m_bShow; }
};


#endif