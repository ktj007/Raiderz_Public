#pragma once

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Flee : public GInstantJob, public MMemPool<GJobT_Flee>
{
protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_FLEE);
	virtual string GetParamString() const override;

	GJobT_Flee(GModuleAI* pmAI, FleeType nFleeType, float fDurationTime);
	virtual ~GJobT_Flee();

private: 
	FleeType m_nFleeType;
	float	m_fDurationTime;
};