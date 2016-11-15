#ifndef _GJobT_Die_h
#define _GJobT_Die_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GEntityActor;

class GJobT_Die: public GInstantJob, public MMemPool<GJobT_Die>
{
private:
	MUID m_uidRewarder;

protected:
	virtual bool OnStart();

public:
	DECLARE_JOB_ID(GJOB_DIE);
	virtual string GetParamString() const override;

	GJobT_Die(GModuleAI* pmAI, MUID uidRewarder);
	virtual ~GJobT_Die();
};

#endif
