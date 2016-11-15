#pragma once

#include "MUnitTestReporter.h"
#include "MTime.h"

class GUnitTestTimeElapsedReporter: public MUnitTestReporter
{
public:
	explicit GUnitTestTimeElapsedReporter(std::ostream& ostream, bool bBreakOnFailure=false);

	virtual void ReportTestStart(UnitTest::TestDetails const& test) override;
	virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed) override;

	void BeginCaseTimer(wstring strInput=L"");
	void EndCaseTimer();
private:
	MTimeChecker2 m_timerTotal;
	MTimeChecker2 m_timerCase;
	vector<double> m_vecCaseTimes;
	// 최악의 입력값 남기기 위한 변수들
	wstring		  m_strInputTemp;
	wstring		  m_strWorstInput;
	double		  m_fWorstTime;
};