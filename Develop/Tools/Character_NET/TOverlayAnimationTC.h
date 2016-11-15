#pragma once

#include "XOverlayAnimationTC.h"

class TOverlayAnimationTC : public XOverlayAnimationTC
{
	bool								m_bTestPlay;
public:
	TOverlayAnimationTC(TCharacter* pCharacter);
	virtual ~TOverlayAnimationTC(void);

	virtual void						SetAnimation(string& strAniName);
	void								SetPlayTest(bool bTest) { m_bTestPlay = bTest; }

	virtual void						Update(float fDelta);
};
