#ifndef _GJOB_H
#define _GJOB_H

#include <queue>
#include <list>
using namespace std;

#include "GJobID.h"
#include "GMessage.h"
#include "GTask_Broker.h"
#include "GTaskID.h"

class GModuleAI;
class GJobMgr;
class GEntityNPC; 

struct GJobAttribute
{
	bool bClearJobListWhenCanceled;
	bool bClearJobListWhenTimedout;
	bool bAutoDeletable;

	GJobAttribute()
	{
		 bClearJobListWhenCanceled = false;
		 bClearJobListWhenTimedout = false;
		 bAutoDeletable = true;
	}
};

/// GJob
//		#역활: 세부 Job들의 상위 클래스
class GJob 
{
private:
	double				m_fDurationTime;
	MUID				m_uidOwner;
	
	bool UpdateDuration(float fDelta);								///< 잡 유효시간 갱신 (유효시간이 만기되면 true를 반환)
	GJobResult UpdateTask(float fDelta);

protected:
	friend GJobMgr;

	GTask*				m_pRunningTask;
	GModuleAI*			m_pOwner;
	GTaskBroker			m_TaskBroker;
	bool				m_bSuspended;
	
protected:
	virtual bool OnStart() = 0;							///< Job 시작시
	virtual GJobResult OnRun(float fDelta);				///< Job 실행
	virtual void OnComplete()	{}						///< Job 완료시
	virtual void OnCancel()		{}						///< Job 취소시
	virtual void OnTimeout()	{}						///< Job 수행시간 만료
	virtual void OnSuspend();							///< Job 중단시
	virtual void OnFinished();							///< Job 종료됨
	virtual void OnResume();							///< 중단된 Job 재개시

	virtual void OnTaskComplete(GTaskID nTaskID)		{}	///< 태스크 완료시
	virtual void OnTaskCancel(GTaskID nTaskID)			{}	///< 태스크 취소시
	bool SetRunningTask(GTask* pTask);

public:
	GJobAttribute		Attr;
public:
	GJob(GModuleAI* pOwner);
	virtual ~GJob();

	bool Start();
	GJobResult Run(float fDelta);
	void Complete();
	void Cancel();
	void TimeOut();

	void SetDurationTime(double fTime);					///< 태스크 수행시간 설정 (second단위)
	double GetDurationTime()							{ return m_fDurationTime; }
	void Suspend();	
	void Resume();	

	virtual	 bool IsInstant()		{ return false; }
	virtual	 bool IsSessionJob()	{ return false; }
			 bool IsSuspended()		{ return m_bSuspended; }
	virtual	bool IsCancelable()		{ return true; }

			 MUID GetOwnerUID();

	GModuleAI* GetOwner() { return m_pOwner; }
	GEntityNPC* GetOwnerNPC();
	const GEntityNPC* GetOwnerNPC() const;
	virtual GJobID GetID() = 0;
	virtual string GetParamString() const = 0;

	virtual const wchar_t* GetJobName() const { return L"noname_job"; }
	const wchar_t* GetTaskName();
	GTask*		GetTask();
};


#define DECLARE_JOB_ID(_ID)	GJobID GetID() { return _ID; }	\
							virtual const wchar_t* GetJobName() const override { return L#_ID; }

#endif