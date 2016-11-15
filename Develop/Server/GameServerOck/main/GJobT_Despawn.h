#ifndef _GJobT_Despawn_h
#define _GJobT_Despawn_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Despawn: public GInstantJob, public MMemPool<GJobT_Despawn>
{
private:
	bool m_bSpawnEnable;

protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_DESPAWN);
	virtual string GetParamString() const override;

	GJobT_Despawn(GModuleAI* pmAI, bool bSpawnEnable);
	virtual ~GJobT_Despawn();
};

#endif
