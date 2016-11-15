#pragma once

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_NextAction: public GInstantJob, public MMemPool<GJobT_NextAction>
{
public:
	DECLARE_JOB_ID(GJOB_NEXTACTION);
	string GetParamString() const;

	GJobT_NextAction(GModuleAI* pmAI);
	virtual ~GJobT_NextAction(void);

private:
	virtual bool OnStart() override;
};
