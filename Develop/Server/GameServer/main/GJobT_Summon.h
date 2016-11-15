#pragma once

#include "MMemPool.h"
#include "GInstantJob.h"

class GJobT_Summon : public GInstantJob, public MMemPool<GJobT_Summon>
{
private:
	int m_nNPCID;
	vec3 m_vStartPos;
protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_SUMMON);
	string GetParamString() const;

	GJobT_Summon(GModuleAI* pmAI, int nNPCID, vec3 vStartPos);
	virtual ~GJobT_Summon();
	
};