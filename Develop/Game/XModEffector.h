#pragma once

#include "CSModEffector.h"

class XModEffector : public CSModEffector
{
public:
	void ApplyInstantModidier(CSInstantModifier& Modifier);
	void RefreshStatus();
};
