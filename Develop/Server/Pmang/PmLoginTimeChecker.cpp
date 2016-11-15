#include "stdafx.h"
#include "PmLoginTimeChecker.h"

PmLoginTimeChecker::PmLoginTimeChecker(uint32 nToleranceSeconds)
: m_nToleranceSeconds(nToleranceSeconds)
{
	// do nothing
}

bool PmLoginTimeChecker::Verify(time_t timeLogin, time_t timeCurrent) const
{
	if (timeCurrent <= timeLogin + m_nToleranceSeconds)
	{
		return true;
	}

	return false;
}
