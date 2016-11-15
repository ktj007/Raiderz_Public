#pragma once

#include "XFaceAnimationTC.h"

class TFaceAnimationTC : public XFaceAnimationTC
{
public:
	TFaceAnimationTC(TCharacter* pCharacter);
	virtual ~TFaceAnimationTC(void);

	void								StopFaceAnimation();

};
