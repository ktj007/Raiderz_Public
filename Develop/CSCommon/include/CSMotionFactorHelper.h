#pragma once

#include "CSMotionFactorInfo.h"


class CSCOMMON_API CSMotionFactorHelper
{
private:
	static int g_nPriority[MF_PRIORITY_SIZE];
public:
	static int GetMFPriority(MF_STATE nState);
	static bool IsMovingMotionFactor(MF_STATE nState);
	static bool HasMotionFactor( MF_STATE nState );

	static bool IsFakeMotionFactor(MF_STATE nState);

	static bool IsNoneOrFakes(MF_STATE nState);
	static bool IsNoneOrFakeBeaten(MF_STATE nState);
};
