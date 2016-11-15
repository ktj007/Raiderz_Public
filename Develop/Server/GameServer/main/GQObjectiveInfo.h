#pragma once

#include "CSQObjectiveInfo.h"

class GQObjectiveInfo : public CSQObjectiveInfo, public MTestMemPool<GQObjectiveInfo>
{
public:
	GQObjectiveInfo();
	virtual ~GQObjectiveInfo();
};