#pragma once

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Yell : public GInstantJob, public MMemPool<GJobT_Yell>
{
private:
	int m_nRange;
protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_YELL);
	virtual string GetParamString() const override;

	GJobT_Yell(GModuleAI* pmAI, int nRange);
	virtual ~GJobT_Yell();
};