#include "StdAfx.h"

#include "C_RubyTestSet.h"

//#include "CoreToolRS3.h"
//#include "MainFrm.h"

#include "C_ControlMediator.h"
#include "MProfiler.h"

CRubyTestSet::CRubyTestSet(void) : m_pSzTestTitleHeader_("ENGINE TEST TYPE :")
{
}

CRubyTestSet::~CRubyTestSet(void)
{
}

void CRubyTestSet::InitTestSetLogger_(const char* _pSzTestName)
{
	m_strLog_.clear();

	_ASSERT(FALSE); // 의존성 제거하느라 요거 기능 죽여놨습니다
//	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->CoreToolRS3MainLogger->SetCustomOutputCallback( CRubyTestSet::CustomMLogCallBack );
	mlog("%s %s\n", m_pSzTestTitleHeader_, _pSzTestName);
}

void CRubyTestSet::HaltTestSetLogger_()
{
	// m_strLog_ 를 지우지 말자.
	// GetLastTestLog에서 필로하다.

	_ASSERT(FALSE); // 의존성 제거하느라 요거 기능 죽여놨습니다
// 	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
// 	pMainFrame->CoreToolRS3MainLogger->SetCustomOutputCallback( CMainFrame::MLogOutputCallback );
}

void CRubyTestSet::CustomMLogCallBack(const char* szText, MLogLevel nLevel)
{
	CRubyTestSet::GetReference().m_strLog_.append(szText);
}

/////////////////////////////////////////////////////////////////////////////////////
////
//// AsyncJob Test Set
//// Job Class
//class CAsyncJobTest : public CAsyncJob
//{
//public:
//	virtual void OnEnterToWorkingQueue(){}
//	virtual void OnWorkInThread(){}
//
//	CAsyncJobTest(){}
//	virtual ~CAsyncJobTest(){}
//private:
//};
//
//// Listener (Client )
//class CAsyncJobListenClient
//{
//public:
//	CAsyncJobListenClient() : m_pJobHolder_(NULL), m_nIndex_(0)
//	{
//	}
//	~CAsyncJobListenClient()
//	{
//		if(m_pJobHolder_) m_pJobHolder_->ReleaseHolder();
//	}
//
//	void CreateJobHolder(int _nIndex, CAsyncJobWorkerBoss* _pWorkerBoss, int _nNumOfJob, CAsyncJobGroupHolder* _pGroupHolder)
//	{
//		m_nIndex_ = _nIndex;
//		m_pJobHolder_ = _pWorkerBoss->GetJobHolder<CAsyncJobListenClient>(this, _pGroupHolder);
//		for(int i = 0; i < _nNumOfJob; ++i)
//		{
//			m_pJobHolder_->AddJob( new CAsyncJobTest );
//		}
//		m_pJobHolder_->ExecuteJob();
//	}
//
//	void ListenCompleteAsyncJob()
//	{
//		int nTotalJobNum = m_pJobHolder_->GetAsyncJobList().size();
//		int nCompleteJobNum(0);
//		for(std::vector< CAsyncJob* >::const_iterator itr = m_pJobHolder_->GetAsyncJobList().begin();
//			itr != m_pJobHolder_->GetAsyncJobList().end(); ++itr)
//		{
//			if( (*itr)->IsCompleteJob() )
//				++nCompleteJobNum;
//		}
//		/*if( nTotalJobNum == nCompleteJobNum )
//		{
//			mlog(
//		}
//		else
//		{
//		}*/
//	}
//
//private:
//	CAsyncJobHolder* m_pJobHolder_;
//	int m_nIndex_;
//	
//};

bool CRubyTestSet::TestZoneEngine(int _nTestType, int _nTestIterationNum, const char* _pSzTestParameter)
{
	//CControlMediator* pMediator = ((CMainFrame*)AfxGetMainWnd())->GetControlMediator();
	//if(pMediator == NULL) return false;

	//// 0번 테스트 - 쓰레드 생성 테스트
	//if( _nTestType == 0 )
	//{
	//	std::string strParam(_pSzTestParameter);

	//	// 테스트에 필요한 값들
	//	std::string strTestName;
	//	int  nNumOfThread(0), nNumOfClient(0), nNumOfJob(0);
	//	bool bRandomNum(false);

	//	int nFirstToken = strParam.find(",");
	//	if( nFirstToken == std::string::npos )
	//	{
	//		bRandomNum = true;
	//		strTestName = strParam;
	//	}
	//	else
	//	{
	//		bRandomNum = false;
	//		int nSecondToken = strParam.find(",", nFirstToken  + 1);
	//		int nThirdToken  = strParam.find(",", nSecondToken + 1);
	//		if( nSecondToken == std::string::npos || nThirdToken == std::string::npos )
	//		{
	//			AfxMessageBox("Test Zone Engine 의 루비 스크립트 인자가 맞지 않습니다.");
	//			return false;
	//		}
	//		strTestName = strParam.substr(0, nFirstToken);
	//		nNumOfThread = atoi( strParam.substr( nFirstToken  + 1, nSecondToken - nFirstToken  - 1 ).c_str() );
	//		nNumOfClient = atoi( strParam.substr( nSecondToken + 1, nThirdToken  - nSecondToken - 1 ).c_str() );
	//		nNumOfJob    = atoi( strParam.substr( nThirdToken  + 1, strParam.length() - nThirdToken ).c_str() );
	//	}

	//	// 테스트 시작
	//	InitTestSetLogger_("Thread Pool Creation");

	//	mlog("** START TEST : \n", strTestName.c_str());
	//	mlog("Test will iterate %d times\n", _nTestIterationNum);
	//	for(int i = 0; i < _nTestIterationNum; ++i)
	//	{
	//		mlog("== Iter. %d Test Start ==\n", i);
	//		if(bRandomNum)
	//		{
	//			nNumOfThread = rand() % 10;
	//			if( nNumOfThread <= 0 ) nNumOfThread = 1;

	//			nNumOfClient = rand() % 1000;
	//			if( nNumOfClient <= 0 ) nNumOfClient = 1;
	//		}
	//		mlog("Num Of Thread are %d\n", nNumOfThread);
	//		mlog("Num Of Client are %d\n", nNumOfClient);

	//		// Create Job Worker Boss
	//		CAsyncJobWorkerBoss jobBoss;
	//		jobBoss.InitAsyncJobWorker(nNumOfThread);

	//		// Create Client
	//		CAsyncJobListenClient *jobClients = new CAsyncJobListenClient[nNumOfClient];

	//		// Create Client Jobs To Give Workers
	//		for(int j = 0; j < nNumOfClient; ++j)
	//		{
	//			if( bRandomNum )
	//			{
	//				nNumOfJob = rand() % 100;
	//				if( nNumOfJob <= 0 ) nNumOfJob = 1;
	//			}
	//			mlog("Num Of Jobs Owned Client %d : %d\n", j, nNumOfJob);
	//			jobClients[j].CreateJobHolder( j, &jobBoss , nNumOfJob, NULL);
	//		}

	//		jobBoss.UpdateCompleteJob();

	//		// 종료
	//		delete[] jobClients;
	//		jobBoss.DestroyAsyncJobWorker();
	//		mlog("== Iter. %d Test END ==\n", i);
	//	}
	//	mlog("END TEST **\n");
	//	const char* testCSTR = m_strLog_.c_str();
	//}
	//HaltTestSetLogger_();
	//return true;
	//
	///*if( pMediator->m_mapInfo.m_bLoaded )
	//{
	//	AfxMessageBox("이 테스트는 존 파일이 로딩되어 있지 않는 상황에서 테스트 되어야 합니다.");
	//	return false;
	//}*/
	////MInitProfile(true);
	////MBeginProfile(0, "Loding Time");

	////_pControlMediator->ResetResource();

	////MEndProfile(0);

	//////MBeginProfile(0, "error");
	////MBeginProfile(1, "another dumy");

	////_pControlMediator->LoadZoneObjAndEnvFromXML_(_pSzFileName);


	//////MEndProfile(0);
	////MEndProfile(1);
	////MSaveProfile("TestProfile.pf");

	return true;
}
