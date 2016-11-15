#pragma once

#include "MMemPool.h"
#include "GJob.h"

class GJob_FleeToHome : public GJob, public MMemPool<GJob_FleeToHome>
{
public:
	GJob_FleeToHome(GModuleAI* pmAI);
	virtual ~GJob_FleeToHome(void);

	DECLARE_JOB_ID(GJOB_FLEE_TO_HOME);
	virtual string GetParamString() const override;

private:
	virtual bool OnStart() override;
	virtual GJobResult OnRun(float fDelta) override;
	virtual void OnTaskComplete(GTaskID nTaskID) override;
	virtual void OnCancel() override;

	void InitFleeDir();
	bool Flee();
	vec3 GetTarPos();

private:
	vec3 m_vFleeDir;
};
