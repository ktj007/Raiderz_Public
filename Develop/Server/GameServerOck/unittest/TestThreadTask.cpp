#include "stdafx.h"
#include "GThreadTask.h"
#include "GThreadTaskMgr.h"

SUITE(ThreadTask)
{
	struct TestThreadResultData
	{
		int		nReq;
		wstring	strResult;
	};

	struct TestThreadResultData2
	{
		int		nReq;
	};

	class TestThreadTask : public GThreadTask<TestThreadResultData>
	{
	protected:
		int	m_nReq;

		virtual void OnRun()
		{
			m_pRealResult->GetData()->strResult = L"Done";

			SetResultDone();
		}
		void Init(int nReq)
		{
			m_pRealResult->GetData()->nReq = nReq;
		}
	public:
		using GThreadTask<TestThreadResultData>::SetFutureResult;

		TestThreadTask() : m_nReq(0) 
		{
			Init(0);
		}
		TestThreadTask(int nReq) : m_nReq(nReq)
		{
			Init(nReq);
		}
		virtual ~TestThreadTask() 
		{
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
		TestThreadTask2() {}
		virtual ~TestThreadTask2() {}
	};

	class FThreadTask
	{
	public:
		FThreadTask()
		{

		}
		virtual ~FThreadTask()
		{

		}
	};

	TEST_FIXTURE(FThreadTask, TestThreadTask_WorkingCount)
	{
		TestThreadTask::InitWorkerCount();

		CHECK_EQUAL(0, TestThreadTask::GetWorkingCount());

		TestThreadTask* a = new TestThreadTask();

		CHECK_EQUAL(1, TestThreadTask::GetWorkingCount());

		delete a;

		CHECK_EQUAL(0, TestThreadTask::GetWorkingCount());

		vector<TestThreadTask*>	vecTasks;
		for (int i = 0; i < 10; i++)
		{
			TestThreadTask* pNewTask = new TestThreadTask();
			vecTasks.push_back(pNewTask);
		}

		CHECK_EQUAL(10, TestThreadTask::GetWorkingCount());

		for (size_t i = 0; i < vecTasks.size(); ++i)
		{
			CHECK_EQUAL(10-i, TestThreadTask::GetWorkingCount());
			delete vecTasks[i];
		}

		CHECK_EQUAL(0, TestThreadTask::GetWorkingCount());
	}

	TEST_FIXTURE(FThreadTask, TestThreadTask_WorkingCount2)
	{
		TestThreadTask::InitWorkerCount();
		TestThreadTask2::InitWorkerCount();

		CHECK_EQUAL(0, TestThreadTask::GetWorkingCount());
		CHECK_EQUAL(0, TestThreadTask2::GetWorkingCount());

		TestThreadTask a(10);

		TestThreadTask2 b;
		TestThreadTask2 c;

		CHECK_EQUAL(1, TestThreadTask::GetWorkingCount());
		CHECK_EQUAL(2, TestThreadTask2::GetWorkingCount());

		TestThreadTask2::InitWorkerCount();

		CHECK_EQUAL(1, TestThreadTask::GetWorkingCount());
		CHECK_EQUAL(0, TestThreadTask2::GetWorkingCount());
	}

	TEST_FIXTURE(FThreadTask, TestThreadTask_Complete)
	{
		GFutureResult<TestThreadResultData>* pFutureResult = new GFutureResult<TestThreadResultData>();

		TestThreadTask* pThreadTask = new TestThreadTask();
		pThreadTask->SetFutureResult(pFutureResult);

		CHECK_EQUAL(false, pFutureResult->IsReady());

		// 쓰레드 실행
		pThreadTask->Run();

		CHECK_EQUAL(true, pFutureResult->IsReady());
		CHECK(L"Done" == pFutureResult->GetData()->strResult);

		// ThreadTask가 FutureResult의 레퍼런스를 하나 올렸다.
		CHECK_EQUAL(2, pFutureResult->GetReferenceCount());
		delete pThreadTask;

		CHECK_EQUAL(1, pFutureResult->GetReferenceCount());
		pFutureResult->Drop();
	}


	TEST(TestFutureResult_ReferenceCounter)
	{
		GFutureResult<TestThreadResultData>* pFutureResult = new GFutureResult<TestThreadResultData>();

		CHECK(pFutureResult->GetData() == NULL);

		CHECK_EQUAL(1, pFutureResult->GetReferenceCount());

		pFutureResult->AddRef();
		CHECK_EQUAL(2, pFutureResult->GetReferenceCount());

		pFutureResult->AddRef();
		CHECK_EQUAL(3, pFutureResult->GetReferenceCount());

		pFutureResult->Drop();
		CHECK_EQUAL(2, pFutureResult->GetReferenceCount());

		pFutureResult->Drop();
		CHECK_EQUAL(1, pFutureResult->GetReferenceCount());

		pFutureResult->Drop();
	}


	TEST(TestFutureResult_IsReady)
	{
		GFutureResult<TestThreadResultData>* pFutureResult = new GFutureResult<TestThreadResultData>();

		CHECK_EQUAL(false, pFutureResult->IsReady());

		GRealResult<TestThreadResultData>* pRealResult = new GRealResult<TestThreadResultData>();
		pFutureResult->SetRealData(pRealResult);

		CHECK_EQUAL(true, pFutureResult->IsReady());

		pFutureResult->Drop();
	}

	TEST(TestFutureResult_GetData)
	{
		GFutureResult<TestThreadResultData>* pFutureResult = new GFutureResult<TestThreadResultData>();

		GRealResult<TestThreadResultData>* pRealResult = new GRealResult<TestThreadResultData>();

		pRealResult->GetData()->strResult = L"YES";

		pFutureResult->SetRealData(pRealResult);

		ASSERT_EQUAL(true, pFutureResult->IsReady());
		CHECK(L"YES" == pFutureResult->GetData()->strResult);

		pFutureResult->Drop();
	}

}