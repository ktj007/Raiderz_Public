#pragma once

#include "MMemPool.h"
#include "GJob.h"


class GJob_WaitFor : public GJob, public MMemPool<GJob_WaitFor>
{
public:
	DECLARE_JOB_ID(GJOB_WAITFOR);
	virtual string GetParamString() const override;

	GJob_WaitFor(GModuleAI* pmAI, MUID uidTarget);
	virtual ~GJob_WaitFor(void);

	virtual bool OnStart() override;
	virtual GJobResult OnRun(float fDelta) override;

private:
	MUID m_uidTarget;
};
