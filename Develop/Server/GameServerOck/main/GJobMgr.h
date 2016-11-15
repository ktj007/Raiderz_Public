#ifndef _GJOB_MGR_H
#define _GJOB_MGR_H

#include "GJobID.h"
#include "GMessage.h"

class GEntityNPC;
class GJob;
class GTask;
class GModuleAI;

/// GJobMgr
//		#역활: AI를 통해 부여받은 Job을 처리한다.
//				Job은 Task들을 통해 목표를 완수한다.
//		#주의: TaskMgr와 직접 관계되지 않는다.  필요하다면 ModuleJob을 통해 이용한다.
class GJobMgr : public MTestMemPool<GJobMgr>
{
	friend GModuleAI;
public:
	GJobMgr(GModuleAI* pOwner=NULL);
	~GJobMgr();

	void Clear(bool bCancelCurJob=true);
	void Push(GJob* pJob);
	void PushFront(GJob* pJob);
	void Cancel();						///< 현재 태스크를 취소한다.
	GJobResult Run(float fDelta);

	// 잡 클리어 기능을 막음
	void EnableClearJob(bool bEnable)	{ m_bEnableClearJob = bEnable; }

	void CancelCurrJob();

	// interface functions
	bool IsEmpty() { return (m_pCurrJob==NULL && m_Jobs.empty()); }
	int GetCount() { return (int)m_Jobs.size(); }
	GJob* GetCurrJob();
	GJob* GetJob(size_t nIndex);
	GTask* GetCurrTask();
	GJobID GetCurrJobID();
	deque<GJob*> GetJobs() const;
	void Suspend();
	void Resume();

private:
	bool IsFinished(GJobResult nResult);
	void OnJobFinished(GJob* pJob, GJobResult nResult );
	void PostMessage(GMessage& m);

protected:
	bool PopJob();
	bool PopAndStartJob();
	void CompleteCurrJob();
	void TimeoutCurrJob();
private:
	list<GJob*>				m_Jobs;
	GJob*					m_pCurrJob;
	bool					m_bEnableClearJob;
	bool					m_bLazyDeleteCurJob;
	GModuleAI*				m_pOwner;
};

#endif // _XTASK_MGR_H
