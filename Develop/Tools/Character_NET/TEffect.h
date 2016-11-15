#ifndef _TEFFECT_H
#define _TEFFECT_H

#pragma once

#include "XModelEffect.h"

class TEffect : public XModelEffect
{
private:
	bool				m_bFollowWaterPos;

	TCharacter*			m_pCharacter;

public:
	TEffect();
	virtual ~TEffect();

	bool				Create(MUID& uid, const string& str);
	void				UpdateXEffect(float fDelta);
	void				UpdateWaterPos();

	void				SetFollowWaterPos(bool bWaterPos) { m_bFollowWaterPos = bWaterPos; }
	void				SetActor(TCharacter* pCharacter) { m_pCharacter = pCharacter;}
};



#endif