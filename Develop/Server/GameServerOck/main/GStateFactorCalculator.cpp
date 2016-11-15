#include "stdafx.h"
#include "GStateFactorCalculator.h"
#include "GConst.h"
#include "MMath.h"

float GStateFactorCalculator::CalcStateFactor(MF_STATE state, bool isDirectionBack)
{
	if (isDirectionBack)
	{
		return GetStateFactorByBack();
	}

	return GetStateFactor(state);
}

float GStateFactorCalculator::CalcMotionFactor(MF_STATE state, bool isDirectionBack)
{
	if (isDirectionBack)
	{
		return GetMotionFactorByBack();
	}

	return GetMotionFactor(state);
}

float GStateFactorCalculator::GetStateFactorByBack(void)
{
	return GConst::STATE_FACTOR_FOR_DAMAGE_BY_BACK;
}

float GStateFactorCalculator::GetMotionFactorByBack(void)
{
	return GConst::STATE_FACTOR_FOR_MF_VALUE_BY_BACK;
}

float GStateFactorCalculator::GetStateFactor(MF_STATE state)
{
	if (0 <= state && state < MF_SIZE)
	{
		return GConst::STATE_FACTOR_FOR_DAMAGE[state];
	}
	
	return 1.0f;
}

float GStateFactorCalculator::GetMotionFactor(MF_STATE state)
{
	if (0 <= state && state < MF_SIZE)
	{
		return GConst::STATE_FACTOR_FOR_MF_VALUE[state];	
	}
	
	return 1.0f;
}
