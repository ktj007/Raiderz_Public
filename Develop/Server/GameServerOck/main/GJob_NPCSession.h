#pragma once

#include "MMemPool.h"
#include "GJob.h"

class GJob_NPCSession : public GJob, public MMemPool<GJob_NPCSession>
{
public:
	DECLARE_JOB_ID(GJOB_SESSION);
	virtual string GetParamString() const override;

	GJob_NPCSession(GModuleAI* pmAI, MUID uidSession);
	virtual ~GJob_NPCSession(void);

	virtual GJobResult OnRun(float fDelta) override;
private:
	MUID m_uidSession;
};
