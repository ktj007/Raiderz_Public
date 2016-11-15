#include "StdAfx.h"
#include "TestReportList.h"

const int MAX_DATA_STRING_LENGTH		= 128;
const int MAX_COLUMN_COUNT_TESTSUMMARY	= 5;
const int MAX_COLUMN_COUNT_TIMERINFO	= 4;
const int MAX_COLUMN_COUNT_TESTRESULT	= 7;
const int SIZE_COLUMN_WIDTH				= 120;

const char szTestSummayColumn[MAX_COLUMN_COUNT_TESTSUMMARY][MAX_DATA_STRING_LENGTH]=
{{"테스트시작시간"},
{"DBIP"},
{"Catalog"},
{"PoolSize"},
{"QueryString"}};

const char szTimerInfoColumnp[MAX_COLUMN_COUNT_TIMERINFO][MAX_DATA_STRING_LENGTH]=
{{"시"},
{"분"},
{"초"},
{"간격"}};

/// 테스트번호,시작시간,종료시간,걸린시간,성공,실패,초당처리
const char szTestResultColumn[MAX_COLUMN_COUNT_TESTRESULT][MAX_DATA_STRING_LENGTH]=
{{"No."},
{"시작시간"},
{"종료시간"},
{"걸린시간"},
{"성공"},
{"실패"},
{"초당처리"}};

TestReportList::TestReportList( CListCtrl* pListCtrl )
{
	m_pListCtrl	= pListCtrl;
}

TestReportList::~TestReportList()
{
}

BOOL TestReportList::Init()
{
	/// List Control의 Column을 추가
	for (int i=0;i<MAX_COLUMN_COUNT_TESTRESULT;++i)
	{
		m_pListCtrl->InsertColumn( i, szTestResultColumn[i],	LVCFMT_CENTER, SIZE_COLUMN_WIDTH );
	}

	return TRUE;
}

void TestReportList::OutputTestSummaryHeader()
{
	mlog("\n\n");
	for (int i=0;i<MAX_COLUMN_COUNT_TESTSUMMARY;++i)
	{
		mlog("%s,", szTestSummayColumn[i]);
	}
	mlog("\n");
}

void TestReportList::OutputTestSummary(CString strDBIP, CString strCatalog, CString strPoolSize, CString strQueryString)
{
	time_t		tClock;
	struct tm	tmTime;

	time( &tClock );
	localtime_s( &tmTime,&tClock );

	char szDateTime[64]	= {0,};
	sprintf_s(szDateTime, 64, "%02d-%02d-%02d", 
			tmTime.tm_year+1900, tmTime.tm_mon+1, tmTime.tm_mday);

	mlog( "%s,%s,%s,%s,%s\n", szDateTime, strDBIP.GetString(), strCatalog.GetString(), strPoolSize.GetString(), strQueryString.GetString() );
}

void TestReportList::OutputTimerInfoHeader()
{
	mlog("\n");
	for (int i=0;i<MAX_COLUMN_COUNT_TIMERINFO;++i)
	{
		mlog("%s,", szTimerInfoColumnp[i]);
	}
	mlog("\n");
}

void TestReportList::OutputTimerInfo(CString& strHour, CString& strMinute, CString& strSecond, CString& strTerm)
{
	mlog( "%s,%s,%s,%s\n", strHour.GetString(), strMinute.GetString(), strSecond.GetString(), strTerm.GetString() );
}

void TestReportList::OutputTestResultHeader()
{
	mlog("\n");
	for (int i=0;i<MAX_COLUMN_COUNT_TESTRESULT;++i)
	{
		mlog("%s,", szTestResultColumn[i]);
	}
	mlog("\n");
}

void TestReportList::OutputTestResult(int nTestNumber, const char* szStartTime, const char* szEndTime,
									  double dElapsedTime, int nSuccess, int nFail, int nCountPerSecond)
{
	int nItemCount	= m_pListCtrl->GetItemCount();

	m_pListCtrl->InsertItem( nItemCount, "" );

	char szData[MAX_DATA_STRING_LENGTH];

	_itoa_s( nTestNumber,		szData, MAX_DATA_STRING_LENGTH, 10 );
	m_pListCtrl->SetItemText( nItemCount, 0, szData );

	char szTime[64]="";
	_strtime( szTime );
	m_pListCtrl->SetItemText( nItemCount, 1, szStartTime );

	m_pListCtrl->SetItemText( nItemCount, 2, szEndTime );

	sprintf_s( szData, MAX_DATA_STRING_LENGTH, "%f", dElapsedTime );
	m_pListCtrl->SetItemText( nItemCount, 3, szData );

	_itoa_s( nSuccess,	szData, MAX_DATA_STRING_LENGTH, 10 );
	m_pListCtrl->SetItemText( nItemCount, 4, szData );

	_itoa_s( nFail,	szData, MAX_DATA_STRING_LENGTH, 10 );
	m_pListCtrl->SetItemText( nItemCount, 5, szData );

	_itoa_s( nCountPerSecond,	szData, MAX_DATA_STRING_LENGTH, 10 );
	m_pListCtrl->SetItemText( nItemCount, 6, szData );

	CRect ref;
	m_pListCtrl->GetItemRect( 0, &ref, LVIR_BOUNDS );

	CSize sz( 0, ref.Height()*nItemCount );

	m_pListCtrl->Scroll( sz );

	this->DumpTestResult();
}

void TestReportList::DumpTestResult()
{
	int nLogCount	= m_pListCtrl->GetItemCount();
	CString strLog;

	for (int j=0;j<MAX_COLUMN_COUNT_TESTRESULT;++j)
	{
		strLog	+= m_pListCtrl->GetItemText( nLogCount-1, j );
		if (j == MAX_COLUMN_COUNT_TESTRESULT-1)	break;
		strLog	+= ",";
	}
	strLog	+= "\n";
	mlog( strLog.GetString() );
	strLog.Empty();
}
