#pragma once

#include "MMemPool.h"
#include "GInstantJob.h"

class GEntityActor;

class GJobT_FaceTo : public GInstantJob, public MMemPool<GJobT_FaceTo>
{
private:
	MUID m_uidTarget;
protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_FACETO);
	virtual string GetParamString() const override;

	GJobT_FaceTo(GModuleAI* pmAI, MUID uidTarget);
	virtual ~GJobT_FaceTo();
};
