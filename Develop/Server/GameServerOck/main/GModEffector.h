#pragma once

#include "CSModEffector.h"

class GEntityActor;

class GModEffector : public CSModEffector
{
public:
	void ApplyInstantModifier(GEntityActor* pTarget, CSInstantModifier& Modifier);
	void RefreshStatus(GEntityActor* pTarget);
};
