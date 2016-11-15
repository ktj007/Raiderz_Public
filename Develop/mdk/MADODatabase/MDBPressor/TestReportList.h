#pragma once

/*
 *	@brief	측정 결과를 파일 로그로 출력하며,
 *			로그 파일은 엑셀로 옮겨서 ,(콤마)로 컬럼을 분류할 수 있다.
 */

class TestReportList
{
public:
	TestReportList( CListCtrl* pListCtrl );
	virtual ~TestReportList();

	BOOL				Init();

	void				OutputTestSummaryHeader();
	void				OutputTestSummary(CString strDBIP, CString strCatalog, CString strPoolSize, CString strQueryString);

	void				OutputTimerInfoHeader();
	void				OutputTimerInfo(CString& strHour, CString& strMinute, CString& strSecond, CString& strTerm);

	void				OutputTestResultHeader();
	void				OutputTestResult(	int nTestNumber, const char* szStartTime, const char* szEndTime,
											double dElapsedTime, int nSuccess, int nFail, int nCountPerSecond );

protected:

private:
	CListCtrl*			m_pListCtrl;

	void				DumpTestResult();

};
