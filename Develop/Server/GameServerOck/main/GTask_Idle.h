#ifndef _GTASK_DELAY_H
#define _GTASK_DELAY_H


#include "GTask.h"
#include "MTime.h"

/// 아무일도 안하고 대기상태 태스크
class GTask_Idle : public GTask, public MTestMemPool<GTask_Idle>
{
private:

protected:
	MRegulator	m_rgrWaiter;

	virtual bool OnStart();
	virtual GTaskResult OnRun(float fDelta);
	virtual void OnComplete() {}
	virtual void OnCancel() {}
public:
	DECLARE_TASK_ID(GTASK_IDLE);

	GTask_Idle(GEntityNPC* pEntity, float fIdleTime);
	virtual ~GTask_Idle();
	virtual const wchar_t* GetTaskName() { return L"Idle"; }
};







#endif