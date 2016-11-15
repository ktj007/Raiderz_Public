#include "stdafx.h"
#include "GUnitTestTimeElapsedReporter.h"
#include <numeric>


GUnitTestTimeElapsedReporter::GUnitTestTimeElapsedReporter( std::ostream& ostream, bool bBreakOnFailure/*=false*/ )
: MUnitTestReporter(ostream, bBreakOnFailure)
, m_fWorstTime(0.0f)
{
}

void GUnitTestTimeElapsedReporter::ReportTestStart( UnitTest::TestDetails const& test )
{
	m_timerTotal.Start();
	__super::ReportTestStart(test);
}

void GUnitTestTimeElapsedReporter::ReportTestFinish( UnitTest::TestDetails const& test, float secondsElapsed )
{
	mlog("%s.%s ... total elapsed: %.6f\n", 
		test.suiteName, 
		test.testName, 
		m_timerTotal.Check());
	 
	if (!m_vecCaseTimes.empty())
	{
		double fMaxTime = m_fWorstTime;
		double fMinTime = *std::min_element(m_vecCaseTimes.begin(), m_vecCaseTimes.end());
		double fAvrTime = std::accumulate(m_vecCaseTimes.begin(), m_vecCaseTimes.end(), 0.0) / m_vecCaseTimes.size();
		mlog("\tcnt(%d), avr(%.6f), max(%.6f), min(%.6f)\n", 
			m_vecCaseTimes.size(), fAvrTime, fMaxTime, fMinTime);
		if (!m_strWorstInput.empty())
		{
			mlog("\tworst_input: %s\n", m_strWorstInput.c_str());
		}
	}
		
	__super::ReportTestFinish(test, secondsElapsed);

	m_fWorstTime = 0.f;
	m_strWorstInput = L"";
	m_vecCaseTimes.clear();
}

void GUnitTestTimeElapsedReporter::BeginCaseTimer(wstring strInput)
{
	m_timerCase.Start();
	m_strInputTemp = strInput;
}

void GUnitTestTimeElapsedReporter::EndCaseTimer()
{
	double fCurTime = m_timerCase.Check();
	if (m_fWorstTime == 0.0f ||
		m_fWorstTime < fCurTime)
	{
		m_fWorstTime = fCurTime;
		m_strWorstInput = m_strInputTemp;
	}
	m_vecCaseTimes.push_back(fCurTime);
}