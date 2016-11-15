#include "stdafx.h"
#include "GThreadTask.h"
#include "GThreadTaskMgr.h"

SUITE(ThreadTaskMgr)
{
	struct TestThreadResultData
	{
		int		nReq;
		wstring	strResult;
	};

	struct TestThreadResultData2
	{
		int		nReq;
		wstring	strResult;
	};

	class TestThreadTask : public GThreadTask<TestThreadResultData>
	{
	protected:
		int	m_nReq;

		void DoSomething(int n)
		{
			//for (int i = 0; i < n; ++i)
			//{
			//	static volatile int x;
			//	for (int j = 0; j < 20000000; ++j)
			//	{
			//		++x;
			//	}
			//}
			//mlog("TestThreadTask - DoingSomething (%u)\n", ::GetCurrentThreadId());

			m_pRealResult->GetData()->strResult = L"Done";
		}
		virtual void OnRun()
		{
			DoSomething(1);

			SetResultDone();
		}
		void Init(int nReq)
		{
			m_pRealResult->GetData()->nReq = nReq;
		}
	public:
		TestThreadTask(int nReq) : m_nReq(nReq)
		{
			Init(nReq);
		}
		virtual ~TestThreadTask() 
		{
		}
		void ShortRun()
		{
			mlog("TestThreadTask - ShortRun (%u)\n", ::GetCurrentThreadId());

			SetResultDone();
		}
	};

	class TestThreadTask2 : public GThreadTask<TestThreadResultData2>
	{
	protected:
		virtual void OnRun()
		{
			SetResultDone();
		}
	public:
		TestThreadTask2()
		{
		}
		virtual ~TestThreadTask2() 
		{
		}
	};

	class FThreadTaskMgr
	{
	public:
		FThreadTaskMgr()
		{
			TestThreadTask::InitWorkerCount();
			TestThreadTask2::InitWorkerCount();
		}
		virtual ~FThreadTaskMgr()
		{

		}

		GThreadTaskMgr		m_ThreadTaskMgr;
	};

	TEST_FIXTURE(FThreadTaskMgr, TestThreadTask_Request_And_WorkDone)
	{
		TestThreadTask* pNewTask = new TestThreadTask(100);

		GFutureResult<TestThreadResultData>* pFutureResult = 
			m_ThreadTaskMgr.Request<TestThreadResultData>(pNewTask);

		// 쓰레드 작업이 완료될 때까지 기다린다.
		while (TestThreadTask::GetWorkingCount() > 0) {}

		CHECK_EQUAL(true, pFutureResult->IsReady());
		CHECK_EQUAL(100, pFutureResult->GetData()->nReq);
		CHECK(L"Done" == pFutureResult->GetData()->strResult);

		pFutureResult->Drop();
	}

	TEST_FIXTURE(FThreadTaskMgr, TestThreadTask_Request_Flood)
	{
		vector< GFutureResult<TestThreadResultData>* > vecFutureResults;

		const int CNT = 10000;
		for (int i = 0; i < CNT; i++)
		{
			TestThreadTask* pNewTask = new TestThreadTask(i);
			GFutureResult<TestThreadResultData>* pFutureResult = 
				m_ThreadTaskMgr.Request<TestThreadResultData>(pNewTask);

			vecFutureResults.push_back(pFutureResult);
		}

		// 쓰레드 작업이 완료될 때까지 기다린다.
		while (TestThreadTask::GetWorkingCount() > 0) {}

		for (size_t i = 0; i < CNT; i++)
		{
			GFutureResult<TestThreadResultData>* pFutureResult = vecFutureResults[i];

			CHECK_EQUAL(true, pFutureResult->IsReady());
			CHECK_EQUAL(i, pFutureResult->GetData()->nReq);
			CHECK(L"Done" == pFutureResult->GetData()->strResult);
			pFutureResult->Drop();
		}
	}


}