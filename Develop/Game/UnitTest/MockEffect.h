#pragma once

#include "XEffect.h"

class MockEffect : public XEffect
{
public:
	bool				m_bLoadingComplete;

public:
	MockEffect();
	virtual ~MockEffect();

	virtual bool		Create(MUID& uid, const wstring& str);
	virtual void		Stop();
	virtual void		UpdateXEffect(float fDelta);

	virtual bool		IsEffectLive();
	virtual bool		IsEffectLoadingComplete();
};
