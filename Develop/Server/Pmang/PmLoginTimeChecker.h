#pragma once

#include "PmLib.h"

class PMANG_API PmLoginTimeChecker
{
public:
	PmLoginTimeChecker(uint32 nToleranceSeconds);

	bool Verify(time_t timeLogin, time_t timeCurrent) const;

private:
	uint32 m_nToleranceSeconds;
};
