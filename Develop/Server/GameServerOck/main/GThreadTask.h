#pragma once

#include "tbb/task.h"
#include "tbb/atomic.h"
#include "MThread2.h"

class GThreadTaskMgr;


template <class T>
class GThreadTaskResult
{
public:
	GThreadTaskResult() {}
	virtual ~GThreadTaskResult() {}
	virtual T* GetData() = 0;
};

template <class T>
class GFutureResult : public GThreadTaskResult<T>, public MClassLevelLockable<T>
{
protected:
	mutable int m_nReferenceCounter;

	GThreadTaskResult*	m_pRealResult;
public:
	GFutureResult() : m_pRealResult(NULL), m_nReferenceCounter(1)
	{

	}
	virtual ~GFutureResult() 
	{
		if (m_pRealResult)
		{
			delete m_pRealResult;
			m_pRealResult = NULL;
		}
	}
	virtual T* GetData()
	{
		// guarded
		Lock lockThisFunc;

		if (m_pRealResult) return m_pRealResult->GetData();
		return NULL;
	}
	void SetRealData(GThreadTaskResult* pRealData)
	{
		// guarded
		Lock lockThisFunc;
		
		m_pRealResult = pRealData;
	}
	bool IsReady()
	{
		// guarded
		Lock lockThisFunc;

		if (m_pRealResult) return true;
		return false;
	}
	void AddRef() const
	{
		// guarded
		Lock lockThisFunc;

		++m_nReferenceCounter;
	}

	bool Drop() const
	{
		// guarded
		Lock lockThisFunc;

		assert(m_nReferenceCounter > 0);

		--m_nReferenceCounter;
		if (!m_nReferenceCounter)
		{
			delete this;
			return true;
		}

		return false;
	}

	int GetReferenceCount() const
	{
		return m_nReferenceCounter;
	}

};

template <class T>
class GRealResult : public GThreadTaskResult<T>
{
protected:
	T			m_Data;
public:
	GRealResult() {}
	virtual ~GRealResult() {}
	virtual T* GetData() { return &m_Data; }
};

class GBaseThreadTask
{
protected:
	virtual void OnRun()
	{

	}
public:
	GBaseThreadTask() {}
	virtual ~GBaseThreadTask() {}
	void Run()
	{
		OnRun();
	}
};


template <class T>
class GThreadTask : public GBaseThreadTask
{
	friend class GThreadTaskMgr;
protected:
	GRealResult<T>*		m_pRealResult;
	GFutureResult<T>*	m_pFutureResult;

	static tbb::atomic<int>		m_nWorkingCount;

	// 상속 받은 ThreadTask 클래스에서 작업이 다 끝났으면 이 함수를 불러줘야 한다.
	void SetResultDone()
	{
		if (m_pFutureResult)
		{
			m_pFutureResult->SetRealData(m_pRealResult);
			m_pRealResult = NULL;
		}
	}
	void SetFutureResult(GFutureResult<T>* pFutureResult)
	{
		m_pFutureResult = pFutureResult;
		m_pFutureResult->AddRef();
	}
public:
	GThreadTask() : GBaseThreadTask()
	{
		m_nWorkingCount++;

		m_pFutureResult = NULL;
		m_pRealResult = new GRealResult<T>();
	}
	virtual ~GThreadTask() 
	{
		if (m_pRealResult)
		{
			delete m_pRealResult;
			m_pRealResult = NULL;
		}

		if (m_pFutureResult)
		{
			m_pFutureResult->Drop();
			m_pFutureResult = NULL;
		}

		m_nWorkingCount--;
	}
	static int GetWorkingCount()
	{
		int ret = m_nWorkingCount;
		return ret;
	}
	static void InitWorkerCount()
	{
		m_nWorkingCount = 0;
	}
};

template<class T> tbb::atomic<int> GThreadTask<T>::m_nWorkingCount;

