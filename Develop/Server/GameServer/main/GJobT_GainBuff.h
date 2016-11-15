#ifndef _GJobT_InstantBuff_h
#define _GJobT_InstantBuff_h

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_GainBuff: public GInstantJob, public MMemPool<GJobT_GainBuff>
{
private:
	int m_nBuffID;

protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_INSTANTBUFF);
	virtual string GetParamString() const override;

	GJobT_GainBuff(GModuleAI* pmAI, int nBuffID);
	virtual ~GJobT_GainBuff();
};

#endif
