#ifndef _MOCK_JOB_TASK_H_
#define _MOCK_JOB_TASK_H_

#include "GJob.h"
#include "GTask.h"

#define TEST_JOB_NAME L"Test"
#define TEST_TASK_NAME L"Test"

enum TEST_ENDED_STATUS
{
	TES_NONE=0,
	TES_RUN,
	TES_COMPLETE,
	TES_CANCEL,
	TES_SUSPEND, 
	TES_RESUME, 
	TES_TIMEOUT, 
	TES_TASK_COMPLETE, 
	TES_TASK_CANCEL, 

	TES_MAX
};

class MockJob : public GJob
{
protected:
	TEST_ENDED_STATUS m_nEndedStatus;

protected:
	virtual bool OnStart()					{ m_nEndedStatus = TES_RUN; return true; }
	virtual GJobResult OnRun(float fDelta)	{ m_nEndedStatus = m_bSuspended?TES_SUSPEND:TES_RUN; return JR_RUNNING; }
	virtual void OnComplete()				{ m_nEndedStatus = TES_COMPLETE;  }
	virtual void OnCancel()					{ m_nEndedStatus = TES_CANCEL; }
	virtual void OnSuspend()				{ m_nEndedStatus = TES_SUSPEND; }
	virtual void OnResume()					{ m_nEndedStatus = TES_RESUME; }
	virtual void OnTimeout()				{ m_nEndedStatus = TES_TIMEOUT; }
	virtual void OnTaskComplete(GTaskID nTaskID)	{ m_nEndedStatus = TES_TASK_COMPLETE; }
	virtual void OnTaskCancel(GTaskID nTaskID)		{ m_nEndedStatus = TES_TASK_CANCEL; }

public:
	MockJob(GModuleAI* pmAI): GJob(pmAI), m_nEndedStatus(TES_NONE) 
	{ 
	}
	TEST_ENDED_STATUS	GetEndedStatus()	{ return m_nEndedStatus; }

	GJobID GetID() { return GJOB_TEST; }
	virtual string GetParamString() const override { return ""; }
	const wchar_t* GetJobName() { return TEST_JOB_NAME; }
	bool SetRunningTask(GTask* pTask) { return __super::SetRunningTask(pTask); }
};

class MockStartFailJob : public MockJob
{
public:
	MockStartFailJob(GModuleAI* pmAI) : MockJob(pmAI) {}
	virtual bool OnStart() { m_nEndedStatus = TES_NONE; return false; }
};

class MockRun_CompletedJob : public MockJob
{
public:
	MockRun_CompletedJob(GModuleAI* pmAI) : MockJob(pmAI) {}
	virtual GJobResult OnRun(float fDelta)	{ return JR_COMPLETED; }
};

class MockRun_CanceledJob : public MockJob
{
public:
	MockRun_CanceledJob(GModuleAI* pmAI) : MockJob(pmAI) {}
	virtual GJobResult OnRun(float fDelta)	{ return JR_CANCELED; }
};

class MockRun_TimeOutJob : public MockJob
{
public:
	MockRun_TimeOutJob(GModuleAI* pmAI) : MockJob(pmAI) {}
	virtual GJobResult OnRun(float fDelta)	{ return JR_TIMEOUT; }
};

class MockTask : public GTask
{
private:
	TEST_ENDED_STATUS m_nEndedStatus;
protected:
	virtual bool OnStart()					{ m_nEndedStatus = TES_RUN; return true; }
	virtual GTaskResult OnRun(float fDelta)	{ return TR_COMPLETED; }
	virtual void OnComplete()				{ m_nEndedStatus = TES_COMPLETE; }
	virtual void OnCancel()					{ m_nEndedStatus = TES_CANCEL; }

public:
	DECLARE_TASK_ID(GTASK_TEST);

	MockTask(GEntityNPC* pOwner): GTask(pOwner), m_nEndedStatus(TES_NONE)	{}
	virtual ~MockTask()					{}
	virtual const wchar_t* GetTaskName()		{ return TEST_TASK_NAME; }
	TEST_ENDED_STATUS	GetEndedStatus()	{ return m_nEndedStatus; }	
};

class MockRun_CanceledTask : public MockTask
{
public:
	MockRun_CanceledTask(GEntityNPC* pNPC) : MockTask(pNPC) {}
	virtual GTaskResult OnRun(float fDelta)	{ return TR_CANCELED; }
};

class MockRun_RunningTask : public MockTask
{
public:
	MockRun_RunningTask(GEntityNPC* pNPC) : MockTask(pNPC) {}
	virtual GTaskResult OnRun(float fDelta)	{ return TR_RUNNING; }
};


#endif //_MOCK_JOB_TASK_H_