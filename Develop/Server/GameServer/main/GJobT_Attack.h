#ifndef _GJobT_Attack_h
#define _GJobT_Attack_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Attack: public GInstantJob, public MMemPool<GJobT_Attack>
{
private:	
	MUID m_uidTarget;
	int m_nDamage;

protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_ATTACK);
	virtual string GetParamString() const override;

	GJobT_Attack(GModuleAI* pmAI, MUID uidTarget, int nDamage);
	virtual ~GJobT_Attack();
};

#endif
