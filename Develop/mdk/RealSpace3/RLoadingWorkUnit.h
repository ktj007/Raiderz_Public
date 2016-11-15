#pragma once

#include "RBackgroundWorkUnit.h"

namespace rs3 {

class RS_API RLoadingWorkUnit : public RBackgroundWorkUnit
{
public:
	RLoadingWorkUnit() : RBackgroundWorkUnit() {}
	virtual ~RLoadingWorkUnit() {}
};





} // namespace rs3