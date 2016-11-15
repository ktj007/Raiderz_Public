#include "stdafx.h"
#include "GModuleAI.h"
#include "MockJobMgr.h"
#include "MockJobTask.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GJobT_Script.h"
#include "GFieldMgr.h"
	
SUITE(JobMgr)
{
	struct FJobMgr
	{
		FJobMgr() {}
		virtual ~FJobMgr() {}

		GJobMgr		m_JobMgr;
		GModuleAI	m_ModuleAI;
	};

	namespace Push
	{
		// 입력된 job이 NULL인 경우
		TEST_FIXTURE(FJobMgr, PushNull)
		{			
			m_JobMgr.Push(NULL);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(true, m_JobMgr.IsEmpty());
		}

		// 이미 job을 가지고 있는 경우
		TEST_FIXTURE(FJobMgr, AlreadyJobExist)
		{
			MockJob* pExistJob = new MockJob(&m_ModuleAI);
			MockJob* pNewJob = new MockJob(&m_ModuleAI);
			m_JobMgr.Push(pExistJob);
			m_JobMgr.Push(pNewJob);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(false, m_JobMgr.IsEmpty());
			CHECK_EQUAL(1, m_JobMgr.GetCount());
			CHECK_EQUAL(m_JobMgr.GetCurrJob(), pExistJob);
			CHECK_EQUAL(TES_RUN, pExistJob->GetEndedStatus());
			CHECK_EQUAL(TES_NONE, pNewJob->GetEndedStatus());
		}

		TEST_FIXTURE(FJobMgr, InstantJob)
		{
			GJobT_Script* pInstantJob1 = new GJobT_Script(&m_ModuleAI, NULL, L"", L"");
			GJobT_Script* pInstantJob2 = new GJobT_Script(&m_ModuleAI, NULL, L"", L"");
			MockJob*	 pJob = new MockJob(&m_ModuleAI);
			
			CHECK_EQUAL(pInstantJob1->IsInstant(), true);

			m_JobMgr.Push(pInstantJob1);
			m_JobMgr.Push(pInstantJob2);
			m_JobMgr.Push(pJob);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob);
		}

		namespace JobStart
		{
			// 입력된 job의 시작이 실패한 경우
			TEST_FIXTURE(FJobMgr, Fail)
			{
				MockStartFailJob* pJob = new MockStartFailJob(&m_ModuleAI);
				m_JobMgr.Push(pJob);
				m_JobMgr.Run(0.0f);

				CHECK_EQUAL(true, m_JobMgr.IsEmpty());
			}

			// 입력된 job의 시작이 성공한 경우
			TEST_FIXTURE(FJobMgr, Success)
			{
				MockJob* pJob = new MockJob(&m_ModuleAI);
				m_JobMgr.Push(pJob);
				m_JobMgr.Run(0.0f);

				CHECK_EQUAL(false, m_JobMgr.IsEmpty());
				CHECK_EQUAL(0, m_JobMgr.GetCount());
				CHECK_EQUAL(TES_RUN, pJob->GetEndedStatus());
				CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob);
			}
		}		
	}

	namespace PushFront
	{
		// 입력된 job이 NULL인 경우
		TEST_FIXTURE(FJobMgr, PushNull)
		{			
			m_JobMgr.PushFront(NULL);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(true, m_JobMgr.IsEmpty());
		}

		// 이미 job을 가지고 있는 경우
		TEST_FIXTURE(FJobMgr, AlreadyJobExist)
		{
			MockJob* pExistJob = new MockJob(&m_ModuleAI);
			MockJob* pNewJob = new MockJob(&m_ModuleAI);
			m_JobMgr.Push(pExistJob);
			m_JobMgr.Run(0.0f);
			m_JobMgr.PushFront(pNewJob);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(false, m_JobMgr.IsEmpty());
			CHECK_EQUAL(TES_CANCEL, pExistJob->GetEndedStatus());
			CHECK_EQUAL(TES_RUN, pNewJob->GetEndedStatus());
			CHECK_EQUAL(m_JobMgr.GetCurrJob(), pNewJob);
		}

		namespace JobStart
		{
			// 입력된 job의 시작이 실패한 경우
			TEST_FIXTURE(FJobMgr, Fail)
			{
				MockJob* pExistJob = new MockJob(&m_ModuleAI);
				MockStartFailJob* pNewJob = new MockStartFailJob(&m_ModuleAI);

				m_JobMgr.Push(pExistJob);
				m_JobMgr.Run(0.0f);
				m_JobMgr.PushFront(pNewJob);
				m_JobMgr.Run(0.0f);

				CHECK_EQUAL(0, m_JobMgr.GetCount());
				CHECK_EQUAL(TES_RUN, pExistJob->GetEndedStatus());
				CHECK_EQUAL(m_JobMgr.GetCurrJob(), pExistJob);
			}

			// 입력된 job의 시작이 성공한 경우
			TEST_FIXTURE(FJobMgr, Success)
			{
				MockJob* pExistJob = new MockJob(&m_ModuleAI);
				MockJob* pNewJob = new MockJob(&m_ModuleAI);

				m_JobMgr.Push(pExistJob);
				m_JobMgr.Run(0.0f);
				m_JobMgr.PushFront(pNewJob);
				m_JobMgr.Run(0.0f);

				CHECK_EQUAL(1, m_JobMgr.GetCount());
				CHECK_EQUAL(TES_CANCEL, pExistJob->GetEndedStatus());
				CHECK_EQUAL(TES_RUN, pNewJob->GetEndedStatus());
				CHECK_EQUAL(m_JobMgr.GetCurrJob(), pNewJob);
			}
		}		
	}

	// job을 취소한 경우
	TEST_FIXTURE(FJobMgr, Cancel)
	{
		MockJob* pJob1 = new MockJob(&m_ModuleAI);
		MockJob* pJob2 = new MockJob(&m_ModuleAI);
		MockJob* pJob3 = new MockJob(&m_ModuleAI);
		m_JobMgr.Push(pJob1);
		m_JobMgr.Push(pJob2);
		m_JobMgr.Push(pJob3);
		m_JobMgr.Run(0.0f);

		m_JobMgr.Cancel();

		CHECK_EQUAL(2, m_JobMgr.GetCount());
		CHECK(NULL == m_JobMgr.GetCurrJob());
	}

	// jobmgr를 정지한 경우
	TEST_FIXTURE(FJobMgr, Suspend)
	{
		MockJob* pJob = new MockJob(&m_ModuleAI);
		m_JobMgr.Push(pJob);
		m_JobMgr.Run(0.0f);

		m_JobMgr.Suspend();

		CHECK_EQUAL(TES_SUSPEND, pJob->GetEndedStatus());
		CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob);
	}

	// jobmgr를 정지후 다시 시작한 경우
	TEST_FIXTURE(FJobMgr, Resume)
	{
		MockJob* pJob = new MockJob(&m_ModuleAI);
		m_JobMgr.Push(pJob);
		m_JobMgr.Run(0.0f);

		m_JobMgr.Suspend();
		m_JobMgr.Resume();

		CHECK_EQUAL(TES_RESUME, pJob->GetEndedStatus());
		CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob);
	}

	namespace Clear
	{
		// 현제 job을 취소하면서 Clear한 경우
		TEST_FIXTURE(FJobMgr, CancelCurJob)
		{
			MockJob* pJob1 = new MockJob(&m_ModuleAI);
			MockJob* pJob2 = new MockJob(&m_ModuleAI);
			MockJob* pJob3 = new MockJob(&m_ModuleAI);
			m_JobMgr.Push(pJob1);
			m_JobMgr.Push(pJob2);
			m_JobMgr.Push(pJob3);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Clear(true);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(true, m_JobMgr.IsEmpty());
		}

		// 현제 job을 취소하지 않고 Clear한 경우
		TEST_FIXTURE(FJobMgr, NoCancelCurJob)
		{
			MockJob* pJob1 = new MockJob(&m_ModuleAI);
			MockJob* pJob2 = new MockJob(&m_ModuleAI);
			MockJob* pJob3 = new MockJob(&m_ModuleAI);
			m_JobMgr.Push(pJob1);
			m_JobMgr.Push(pJob2);
			m_JobMgr.Push(pJob3);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Clear(false);
			m_JobMgr.Run(0.0f);

			CHECK_EQUAL(0, m_JobMgr.GetCount());			
			CHECK_EQUAL(TES_RUN, pJob1->GetEndedStatus());
			CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob1);
		}
	}

	namespace Run
	{
		// 정지된 경우
		TEST_FIXTURE(FJobMgr, Suspended)
		{
			MockJob* pJob = new MockJob(&m_ModuleAI);
			m_JobMgr.Push(pJob);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Suspend();
			m_JobMgr.Run(1);

			CHECK_EQUAL(TES_SUSPEND, pJob->GetEndedStatus());
			CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob);
		}

		// 현제 job이 NULL인 경우
		TEST_FIXTURE(FJobMgr, CurrJobIsNULL)
		{
			MockJob* pJob1 = new MockJob(&m_ModuleAI);
			MockJob* pJob2 = new MockJob(&m_ModuleAI);
			m_JobMgr.Push(pJob1);
			m_JobMgr.Push(pJob2);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Cancel();
			m_JobMgr.Run(1);

			CHECK_EQUAL(0, m_JobMgr.GetCount());
			CHECK_EQUAL(TES_RUN, pJob2->GetEndedStatus());
			CHECK_EQUAL(m_JobMgr.GetCurrJob(), pJob2);
		}

		// 현제 job의 실행 결과가 완료인 경우
		TEST_FIXTURE(FJobMgr, CurrJobIsCompleted)
		{
			MockRun_CompletedJob* pJob = new MockRun_CompletedJob(&m_ModuleAI);
			pJob->Attr.bAutoDeletable = false;
			m_JobMgr.Push(pJob);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Run(1);

			CHECK_EQUAL(true, m_JobMgr.IsEmpty());
			CHECK_EQUAL(TES_COMPLETE, pJob->GetEndedStatus());

			SAFE_DELETE(pJob);
		}

		// 현제 job의 실행 결과가 취소인 경우
		TEST_FIXTURE(FJobMgr, CurrJobIsCancled)
		{
			MockRun_CanceledJob* pJob = new MockRun_CanceledJob(&m_ModuleAI);
			pJob->Attr.bAutoDeletable = false;
			m_JobMgr.Push(pJob);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Run(1);

			CHECK_EQUAL(true, m_JobMgr.IsEmpty());
			CHECK_EQUAL(TES_CANCEL, pJob->GetEndedStatus());

			SAFE_DELETE(pJob);
		}

		// 잡이 취소될때 Clear옵션을 갖고 있다면 잡 스케쥴러를 초기화한다.
		//TEST_FIXTURE(FJobMgr, CurrJobIsCanceledWithJobClearOption)
		//{
		//	MockRun_CanceledJob* pJob = new MockRun_CanceledJob(&m_ModuleAI);
		//	pJob->Attr.bClearJobListWhenCanceled = true;
		//	pJob->Attr.bAutoDeletable = false;
		//	MockJob* pJob2 = new MockJob(&m_ModuleAI);
		//	MockJob* pJob3 = new MockJob(&m_ModuleAI);
		//	
		//	m_JobMgr.Push(pJob);
		//	m_JobMgr.Push(pJob2);
		//	m_JobMgr.Push(pJob3);
		//	m_JobMgr.Run(0.0f);

		//	CHECK_EQUAL(2, m_JobMgr.GetCount());

		//	m_JobMgr.Run(1);
		//
		//	CHECK_EQUAL(TES_CANCEL, pJob->GetEndedStatus());
		//	CHECK_EQUAL(0, m_JobMgr.GetCount());
		//	CHECK_EQUAL(true, m_JobMgr.IsEmpty());

		//	SAFE_DELETE(pJob);
		//}

		// 현제 job의 실행 결과가 시간 초과인 경우
		TEST_FIXTURE(FJobMgr, CurrJobIsTimeOut)
		{
			MockRun_TimeOutJob* pJob = new MockRun_TimeOutJob(&m_ModuleAI);
			pJob->Attr.bAutoDeletable = false;
			m_JobMgr.Push(pJob);
			m_JobMgr.Run(0.0f);

			m_JobMgr.Run(1);

			CHECK_EQUAL(true, m_JobMgr.IsEmpty());
			CHECK_EQUAL(TES_TIMEOUT, pJob->GetEndedStatus());

			SAFE_DELETE(pJob);			
		}
	}	
}