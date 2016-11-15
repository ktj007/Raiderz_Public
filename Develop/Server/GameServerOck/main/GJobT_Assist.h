#ifndef _GJobT_Assist_h
#define _GJobT_Assist_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Assist: public GInstantJob, public MMemPool<GJobT_Assist>
{
private:
	MUID m_uidTarget;

protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_ASSIST);
	virtual string GetParamString() const override;

	GJobT_Assist(GModuleAI* pmAI, MUID uidTarget);
	virtual ~GJobT_Assist();
};


#endif
