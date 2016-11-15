#pragma once

#include "CSTalentInfo.h"

class GEntityActor;

class GBuffRequirement
{
public:
	bool ProcessRequirement(GEntityActor* pEntity, RequireBuff InfoRequirement);

private:
	void LostBuffs( GEntityActor* pEntity, const vector<int>& vecBuffs );
};
