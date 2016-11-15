#pragma once

#include "GTask.h"

class GEntityActor;

class GTask_WaitFor : public GTask, public MTestMemPool<GTask_WaitFor>
{
public:
	DECLARE_TASK_ID(GTASK_WAITFOR);

	GTask_WaitFor(GEntityNPC* pEntity, MUID uidTarget);
	virtual ~GTask_WaitFor(void);

	virtual const wchar_t* GetTaskName() { return L"WaitFor"; }

private:
	virtual bool OnStart() override;
	virtual GTaskResult OnRun(float fDelta) override;
	virtual void OnComplete() override {}
	virtual void OnCancel() override {}

private:
	MUID m_uidTarget;
	MRegulator	m_rgrIntervalChecker;
};
