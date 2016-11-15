#include "stdafx.h"
#include "MUnitTestRunner.h"
#include "TestReporter.h"
#include "MDebug.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

MUnitTestRunner::MUnitTestRunner()
{
	m_Config.Load();
}


bool MUnitTestRunner::IsExistName( const char* szName, vector<string>& vstrNames )
{
	for (size_t i = 0; i < vstrNames.size(); i++)
	{
		if (!std::strcmp(szName, vstrNames[i].c_str())) return true;
	}
	return false;
}

bool MUnitTestRunner::CheckFilter( const UnitTest::TestDetails* details, Filter& filter )
{
	if (!filter.vstrSelectedTestNames.empty())
	{
		// seletected_test 가 있는 경우 오직 선택한 테스트만 수행하기 위해 suite는 체크하지 않는다.
		if (IsExistName(details->testName, filter.vstrSelectedTestNames)) return true;
		return false;
	}

	bool bAllTest = true;

	for (size_t i = 0; i < filter.vstrSuiteAnyWords.size(); i++)
	{
		bAllTest = false;

		string strSuiteNameLower = details->suiteName;
		string strAnyWordLower = filter.vstrSuiteAnyWords[i];

		std::transform(strSuiteNameLower.begin(), strSuiteNameLower.end(), strSuiteNameLower.begin(), std::tolower);
		std::transform(strAnyWordLower.begin(), strAnyWordLower.end(), strAnyWordLower.begin(), std::tolower);

		if (NULL != strstr(strSuiteNameLower.c_str(),  strAnyWordLower.c_str())) return true;
	}

	if (!IsExistName(details->suiteName, filter.vstrDisabledSuiteNames))
	{
		if ((bAllTest && filter.vstrSuiteNames.empty() )
			|| IsExistName(details->suiteName, filter.vstrSuiteNames ))
		{
			return true;
		}
	}

	return false;
}

void MUnitTestRunner::MakeFilterFromConfig( Filter &outFilter )
{
	for (int i = 0; i < UNITTEST_CONFIG_FILTER_ITEM_COUNT; i++)
	{
		if (m_Config.m_strTest[i].size() > 0) outFilter.vstrSelectedTestNames.push_back(m_Config.m_strTest[i]);
		if (m_Config.m_strSuite[i].size() > 0) outFilter.vstrSuiteNames.push_back(m_Config.m_strSuite[i]);
		if (m_Config.m_strDisabledSuite[i].size() > 0) outFilter.vstrDisabledSuiteNames.push_back(m_Config.m_strDisabledSuite[i]);
	}
	if (!m_Config.m_strSuiteAnyWords.empty())
	{
		outFilter.vstrSuiteAnyWords.push_back(m_Config.m_strSuiteAnyWords);
	}
}

bool MUnitTestRunner::BeDoTest()
{
	// 설정값에서 유닛테스트 실행안한다고 했으면 안함.
	if (m_Config.m_bExecuteUnitTest == false) return false;

	return true;
}

void MUnitTestRunner::LogSuiteName( string& strSuiteName )
{
	if (m_Config.m_bLogSuiteName == false) return;

	if (m_setExcutedSuiteName.find(strSuiteName) != m_setExcutedSuiteName.end()) return;

	m_setExcutedSuiteName.insert(strSuiteName);
	mlog("UnitTest SuiteName : %s\n", strSuiteName.c_str());
}

bool MUnitTestRunner::RunUnitTest( MBaseUnitTest* pUnitTest )
{
	UnitTest::TestList const& test_list = pUnitTest->GetTestList();

	mlog("\n=============== UnitTest(%s) ===============\n", pUnitTest->GetName());

	std::ofstream f(pUnitTest->GetOutputFileName());
	int failure_count = 0;

	MUnitTestReporter* pRepoerter = NewUnitTestReporter(f, m_Config.m_bBreakOnFailure);
	if (!pRepoerter)
		return false;
	
	Filter filter;
	MakeFilterFromConfig(filter);

	failure_count = RunTests(pUnitTest, *pRepoerter, filter, 0); 
	delete (pRepoerter);
	return (failure_count <= 0);
}

#include "AssertException.h"

namespace UnitTest {

	template< typename T >
	void MExecuteTest(const T& testObject, TestDetails const& details)
	{
		CurrentTest::Details() = &details;
#ifdef _DEBUG
		testObject.RunImpl();
#else
		try
		{
				testObject.RunImpl();
		}
		catch (AssertException const& e)
		{
			CurrentTest::Results()->OnTestFailure(
				TestDetails(details.testName, details.suiteName, e.Filename(), e.LineNumber()), e.what());
		}
		catch (std::exception const& e)
		{
			MemoryOutStream stream;
			stream << "Unhandled exception: " << e.what();
			CurrentTest::Results()->OnTestFailure(details, stream.GetText());
		}
		catch (...)
		{
			CurrentTest::Results()->OnTestFailure(details, "Unhandled exception: Crash!");
		}
#endif
	}

}
int MUnitTestRunner::RunTests( MBaseUnitTest* pUnitTest, UnitTest::TestReporter& reporter, Filter& filter, int const maxTestTimeInMs )
{
	UnitTest::TestList const& list = pUnitTest->GetTestList();

	unsigned int nTotalTestCount = 0;
	{
		UnitTest::Test const* pTest = list.GetHead();
		while (pTest != 0)
		{
			nTotalTestCount++;
			pTest = pTest->next;
		}
	}	

	UnitTest::TestResults result(&reporter);
	UnitTest::CurrentTest::Results() = &result;

	UnitTest::Timer overallTimer;
	overallTimer.Start();

	unsigned int nCurTestRunCount = 0;
	UnitTest::Test const* curTest = list.GetHead();
	while (curTest != 0)
	{
		if (CheckFilter(&curTest->m_details, filter))
		{
			UnitTest::Timer testTimer;
			testTimer.Start();
			result.OnTestStart(curTest->m_details);

			pUnitTest->OnTestStart();
			UnitTest::MExecuteTest(*curTest, curTest->m_details);
			int const testTimeInMs = testTimer.GetTimeInMs();
			pUnitTest->OnTestFinish();

			nCurTestRunCount++;
			
			if (maxTestTimeInMs > 0 && testTimeInMs > maxTestTimeInMs && !curTest->m_timeConstraintExempt)
			{
				UnitTest::MemoryOutStream stream;
				stream << "Global time constraint failed. Expected under " << maxTestTimeInMs <<
					"ms but took " << testTimeInMs << "ms.";
				result.OnTestFailure(curTest->m_details, stream.GetText());
			}
			result.OnTestFinish(curTest->m_details, testTimeInMs/1000.0f);

			UnitTest::MemoryOutStream streamTestSemiInfo;
			streamTestSemiInfo << curTest->m_details.suiteName << " (" << nCurTestRunCount << "/" << nTotalTestCount << ")";
			
			LogSuiteName(string(streamTestSemiInfo.GetText()));
			// LogSuiteName(string(curTest->m_details.suiteName));
		}

		curTest = curTest->next;
	}

	float const secondsElapsed = overallTimer.GetTimeInMs() / 1000.0f;
	reporter.ReportSummary(result.GetTotalTestCount(), result.GetFailedTestCount(), result.GetFailureCount(), secondsElapsed);

	return result.GetFailureCount();
}

MUnitTestReporter* MUnitTestRunner::NewUnitTestReporter(std::ostream& ostream, bool bBreakOnFailure)
{
	return new MUnitTestReporter(ostream, bBreakOnFailure);
}