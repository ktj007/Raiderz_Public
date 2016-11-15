#ifndef _GStateFactorCalculator_h
#define _GStateFactorCalculator_h

#include "CSMotionFactorInfo.h"

class GStateFactorCalculator
{
public:
	float CalcStateFactor(MF_STATE state, bool isDirectionBack);
	float CalcMotionFactor(MF_STATE state, bool isDirectionBack);
	virtual float GetStateFactor(MF_STATE state);
	virtual float GetMotionFactor(MF_STATE state);
	virtual float GetStateFactorByBack(void);
	virtual float GetMotionFactorByBack(void);
};

#endif
