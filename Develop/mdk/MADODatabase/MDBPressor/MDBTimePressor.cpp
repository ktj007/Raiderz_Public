#include "StdAfx.h"
#include "MDBTimePressor.h"


MDBTimePressor* MDBTimePressor::m_pDBPressor	= NULL;

MDBTimePressor::MDBTimePressor()
{
	MDBTimePressor::m_pDBPressor	= this;
	this->Reset();
}

MDBTimePressor::~MDBTimePressor()
{
	MDBTimePressor::m_pDBPressor	= NULL;
}

BOOL MDBTimePressor::Init(	CString strDataSource,
							CString strCatalog,
							CString strID,
							CString strPassword,
							CString strPoolSize )
{
	if (!m_MDatabase.InitADODatabase(	strDataSource.GetString(),
										strCatalog.GetString(),
										strID.GetString(),
										strPassword.GetString(),
										MDBTimePressor::OnComplete ))
		return FALSE;

	return TRUE;
}

BOOL MDBTimePressor::StartPress()
{
	UINT nThreadID;

	m_hExecuteThreadHandle	= (HANDLE)_beginthreadex( NULL, 0, MDBTimePressor::ExecuteThread, (void*)this, 0, &nThreadID );
	if (m_hExecuteThreadHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	m_hExecuteEvent	= CreateEvent(NULL, FALSE, TRUE, NULL);

	return TRUE;
}

void MDBTimePressor::EndPress()
{
	CloseHandle(m_hExecuteEvent);
}

void MDBTimePressor::SetTimer(unsigned int nWholeTIme, unsigned int nTerm)
{
	m_nWholeTime	= nWholeTIme;
	m_nTerm			= nTerm;
}

void MDBTimePressor::CatchEndTest()
{
	MDBPressor::CatchEndTest();

	m_nCompleteCount	= 0;
	m_nFailCount		= 0;

	SetEvent(m_hExecuteEvent);	// Execute Event 깨움
}

void MDBTimePressor::Reset()
{
	MDBPressor::Reset();

	m_nWholeTime			= 0;
	m_nTerm					= 0;
	m_hExecuteEvent			= INVALID_HANDLE_VALUE;
}

void MDBTimePressor::StartTimer()
{
	MDBPressor::StartTimer();
}

void MDBTimePressor::EndTimer()
{
	QueryPerformanceCounter(&m_lnEnd);
	m_dElapsedTime	= ((double)m_lnEnd.QuadPart - (double)m_lnStart.QuadPart) / (float)m_lnFreq.QuadPart;

	_strtime( m_szEndTestTime );

	m_bEndTest			= TRUE;
}

unsigned __stdcall MDBTimePressor::ExecuteThread( void* pArg )
{
	MDBTimePressor*	pMDBPressor	= (MDBTimePressor*)pArg;
	int	nExecuteCount		= pMDBPressor->GetExecuteCount();

	int nWholeTime			= pMDBPressor->GetWholeTime();
	int nWholeStartTime		= timeGetTime();
	int nWholeEndTime		= 0;

	int nTerm				= pMDBPressor->GetTerm();
	int nTermStartTime		= 0;
	int nTermEndTime		= 0;

	while (1)
	{
		/// 실행 이벤트
		DWORD dwRet	= WaitForSingleObject(pMDBPressor->m_hExecuteEvent, 1000);
		if (WAIT_TIMEOUT == dwRet)
		{
			if (pMDBPressor->IsStopExecThread())
				_endthreadex(-1);

			continue;
		}
		if (WAIT_FAILED == dwRet)
			_endthreadex(-1);

		/// 전체 경과시간을 체크
		nWholeEndTime = timeGetTime();
		if (0 != nWholeTime &&
			nWholeEndTime - nWholeStartTime > nWholeTime)
		{
			pMDBPressor->m_bIsEndPress	= TRUE;
			_endthreadex(-1);
		}		

		pMDBPressor->StartTimer();

		for( int i=0;i<nExecuteCount;++i )
		{
			pMDBPressor->AsyncExecute();
		}

		/// Term 시간동안 대기
		nTermStartTime	= timeGetTime();
		while(1)
		{
			nTermEndTime	= timeGetTime();
			if (0 == nTerm)
				break;
			else if (nTermEndTime - nTermStartTime > nTerm)
				break;
			Sleep(1);
		}
	}

	return 0;
}

void MDBTimePressor::OnComplete(unsigned int nKey, MRecordset* pRs, char* szError)
{
	if ((strlen(szError) != 0) || (pRs == NULL))
	{
		MGetDBTimePressor()->IncreaseFailCount();
		OutputDebugString(szError);
		return;
	}

	MGetDBTimePressor()->IncreaseCompleteCount();

	int nExecuteCount	= MGetDBTimePressor()->GetExecuteCount();
	int nCompleteCount	= MGetDBTimePressor()->GetCompleteCount();
	int nFailCount		= MGetDBTimePressor()->GetFailCount();

	if (nExecuteCount <= nCompleteCount+nFailCount)
	{
		MGetDBTimePressor()->EndTimer();
	}
}
