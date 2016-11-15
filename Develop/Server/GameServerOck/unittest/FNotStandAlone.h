#pragma once

#include "GConfig.h"

struct FNotStandAlone
{
	FNotStandAlone() { GConfig::m_bStandAlone = false; }
	~FNotStandAlone() { GConfig::m_bStandAlone = true; }
};