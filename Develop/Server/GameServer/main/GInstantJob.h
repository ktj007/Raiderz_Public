#pragma once

#include "GJob.h"

class GModuleAI;

class GInstantJob : public GJob
{
public:
	GInstantJob(GModuleAI* pmAI);

	virtual bool	IsInstant()		{ return true; }

	GEntityNPC*		GetNPC();
};
