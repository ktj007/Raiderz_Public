#include "StdAfx.h"
#include "MDBPressor.h"
#include "MDBPressorDlg.h"
#include "MDBPressorApp.h"

int MDBPressor::m_nCurrentTestNum		= 0;
MDBPressor* MDBPressor::m_pDBPressor	= NULL;

MDBPressor::MDBPressor()
{
	MDBPressor::m_pDBPressor	= this;

	this->Reset();
}

MDBPressor::~MDBPressor()
{
	MDBPressor::m_pDBPressor	= NULL;
}

BOOL MDBPressor::Init(	CString strDataSource,
					  	CString strCatalog,
					  	CString strID,
					  	CString strPassword,
					  	CString strPoolSize )
{
	if (!m_MDatabase.InitADODatabase(	strDataSource.GetString(),
										strCatalog.GetString(),
										strID.GetString(),
										strPassword.GetString(),
										MDBPressor::OnComplete ))
		return FALSE;

	return TRUE;
}

void MDBPressor::Destroy()
{
	m_bDestroyExecuteThread	= TRUE;

	DWORD dwRet	= WaitForSingleObject(m_hExecuteThreadHandle, INFINITE);
	if (dwRet == WAIT_OBJECT_0)
	{
		CloseHandle(m_hExecuteThreadHandle);
		m_hExecuteThreadHandle = INVALID_HANDLE_VALUE;
	}

	m_nCurrentTestNum++;
}

BOOL MDBPressor::StartPress()
{
	UINT nThreadID;

	m_hExecuteThreadHandle	= (HANDLE)_beginthreadex( NULL, 0, MDBPressor::ExecuteThread, (void*)this, 0, &nThreadID );
	if (m_hExecuteThreadHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}

void MDBPressor::EndPress()
{
	// this->Reset();
}

void MDBPressor::Reset()
{
	m_bDestroyExecuteThread	= FALSE;

	m_nExecuteCount = 0;

	memset(m_szStartTestTime, 0, MAX_STRING_LENGTH_TIME);
	memset(m_szEndTestTime, 0, MAX_STRING_LENGTH_TIME);
	
	m_nCompleteCount = 0;
	m_nFailCount = 0;

	memset( &m_lnFreq, 0, sizeof(LARGE_INTEGER) );
	memset( &m_lnStart, 0, sizeof(LARGE_INTEGER) );
	memset( &m_lnEnd, 0, sizeof(LARGE_INTEGER) );

	m_dElapsedTime = 0.f;
	m_nProcessCountPerSec = 0;

	m_bIsEndPress = FALSE;
	m_bEndTest = FALSE;
}

void MDBPressor::StartTimer()
{
	_strtime( m_szStartTestTime );

	if (!QueryPerformanceFrequency(&m_lnFreq))
	{
		ASSERT(0);
		mlog( "microsecond는 지원하지 않음!\n" );
	}
	QueryPerformanceCounter(&m_lnStart);
}

void MDBPressor::EndTimer()
{
	QueryPerformanceCounter(&m_lnEnd);
	m_dElapsedTime	= ((double)m_lnEnd.QuadPart - (double)m_lnStart.QuadPart) / (float)m_lnFreq.QuadPart;

	_strtime( m_szEndTestTime );

	m_bIsEndPress	= TRUE;
	m_bEndTest		= TRUE;
}

void MDBPressor::AsyncExecute()
{
	// static unsigned int nKey	= 0;

	char* pQuery	= const_cast<char* >( this->GetTestQuery() );
	if (NULL == pQuery)
		return;
	if (strlen(pQuery) == 0)
	{
		return;
	}

	m_MDatabase.PushQuery( pQuery, m_bDestroyExecuteThread );
}

unsigned __stdcall MDBPressor::ExecuteThread( void* pArg )
{
	MDBPressor*	pMDBPressor	= (MDBPressor*)pArg;
	int	nExecuteCount			= pMDBPressor->GetExecuteCount();

	pMDBPressor->StartTimer();

	for( int i=0;i<nExecuteCount;++i )
	{
		if (pMDBPressor->IsStopExecThread())
			return -1;

		pMDBPressor->AsyncExecute();
	}

	return 0;
}

void MDBPressor::OnComplete(unsigned int nKey, MRecordset* pRs, char* szError)
{
	if (1 == nKey)
		return;

	if ((strlen(szError) != 0) || (pRs == NULL))
	{
		MGetDBPressor()->IncreaseFailCount();
		OutputDebugString(szError);
		return;
	}

	MGetDBPressor()->IncreaseCompleteCount();

	int nExecuteCount	= MGetDBPressor()->GetExecuteCount();
	int nCompleteCount	= MGetDBPressor()->GetCompleteCount();
	int nFailCount		= MGetDBPressor()->GetFailCount();

	if (nExecuteCount <= nCompleteCount+nFailCount)
	{
		MGetDBPressor()->EndTimer();
	}

}
