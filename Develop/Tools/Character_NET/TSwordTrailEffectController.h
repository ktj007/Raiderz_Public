#pragma once
#ifndef _TSWORDTRAIL_EFFECT_CONTROLLER_H
#define _TSWORDTRAIL_EFFECT_CONTROLLER_H

#include "XSwordTrailEffect.h"

#define SWORDTRAIL_TEXTURE_NAME			"../../Data/Texture/SFX/trail_test1.tga"
#define SWORDTRAIL_FRAME_GAP_CONTROL	0.08f
#define SWORDTRAIL_DISAPPEAR_TIME		100
#define SWORDTRAIL_MAX_FRAME_SEC		2.0f

class TSwordTrailEffectController
{
private:
	vector<XSwordTrailEffect*> m_vecSwordTrailEffect;

	int						m_nFrameGapControl;
	int						m_nMaxFrame;
	int						m_nStartFrame;
	int						m_nEndFrame;

	int						m_nCurrentSwordTrailEffectIndex;

	string					m_strCurrentAnimationName;
private:

public:
	TSwordTrailEffectController();
	virtual ~TSwordTrailEffectController();

	void					CreateSwordTrailEffect(SWORDTRAIL_DATA* pData);
	void					RenderSwordTrailEffect();
	void					UpdateSwordTrailEffect(float fDelta, int nCurrFrame, SWORDTRAIL_DATA* pData);
	void					DestroySwordTrailEffect();

	bool					CheckSwordTrailEffectAnimation(string& strAnimation);

	void					TestRender();
};


#endif