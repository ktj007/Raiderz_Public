#include "StdAfx.h"
#include "MAutoTestRunner.h"
#include "MAutoTestInfo.h"
#include "MAutoTest.h"
#include "MTypes.h"
#include "MStatisticsProfiler.h"
#include "MMaietXmlTestReporter.h"

#define AUTO_TEST_RUNNER_PROFILE_NUMBER 1000

MAutoTestRunner::MAutoTestRunner()
: m_bExit(false)
, m_bFailure(false)
, m_pTest(NULL)
, m_nRepeatCount(1)
{
}

MAutoTestRunner::~MAutoTestRunner()
{

}

bool MAutoTestRunner::Create()
{
	m_pTest = NewTest();
	if (NULL == m_pTest) return false;
	m_pTest->Create();	

	MEnableStatisticsProfile(true);

	OnCreate();

	return true;
}

void MAutoTestRunner::Destroy()
{
	SAFE_DELETE(m_pTest);

	MEnableStatisticsProfile(false);

	OnDestroy();
}

bool MAutoTestRunner::Update(float fDelta)
{
	if (1 < m_pTest->GetInfo().nSpeed)
	{
		fDelta *= m_pTest->GetInfo().nSpeed;
	}

	m_pTest->Update(fDelta);
	
	// 프로파일 블럭
	PFI_B(AUTO_TEST_RUNNER_PROFILE_NUMBER+m_nRepeatCount, "");
	if (!OnUpdate(fDelta))
	{
		PFI_E(AUTO_TEST_RUNNER_PROFILE_NUMBER+m_nRepeatCount);
		return false;
	}	
	PFI_E(AUTO_TEST_RUNNER_PROFILE_NUMBER+m_nRepeatCount);

	if (m_pTest->GetInfo().dwTime*1000.0f <= PFI_GT(AUTO_TEST_RUNNER_PROFILE_NUMBER+m_nRepeatCount))
	{
		SaveTestResult();
		
		if (m_pTest->GetInfo().nRepeat <= m_nRepeatCount)
		{
			Report();
			m_bExit = true;

			return true;
		}

		m_nRepeatCount++;
	}

	return true;
}

void MAutoTestRunner::SaveTestResult()
{
	MMaietTestResult testResult;
	testResult.testName = m_pTest->GetName();
	testResult.timeElapsed = (float) PFI_GT(AUTO_TEST_RUNNER_PROFILE_NUMBER+m_nRepeatCount);

	float fFrame = PFI_GF(AUTO_TEST_RUNNER_PROFILE_NUMBER+m_nRepeatCount);
	if (fFrame < m_pTest->GetInfo().fLimitFrame)
	{
		testResult.failures.push_back(MMaietTestResult::Result::pair(0, "Low Frame"));
	}

	m_pTest->SaveTestResult(testResult);

	if (true == testResult.failures.empty())
	{
		testResult.successs.push_back(MMaietTestResult::Result::pair(0, "Success"));
	}

	m_TestResulList.push_back(testResult);

	DecisionSuccessOrFailure(testResult);
}

void MAutoTestRunner::Report()
{
	int nTotalTestCount = (int) m_TestResulList.size();
	int nFaieldTestCount = 0;
	int nFaiureCount = 0;
	float fElapsedTime = 0.0f;

	for (size_t i = 0; i < m_TestResulList.size(); i++)
	{
		if (false == m_TestResulList[i].failures.empty())
		{
			nFaieldTestCount++;
			nFaiureCount += (int) m_TestResulList[i].failures.size();
			fElapsedTime += m_TestResulList[i].timeElapsed;
		}
	}

	ofstream f(GetOutputFileName());
	MMaietXmlTestReporter reporter(f);
	reporter.ReportSummary(nFaieldTestCount, nFaieldTestCount, nFaiureCount, fElapsedTime, m_TestResulList);
}

void MAutoTestRunner::DecisionSuccessOrFailure(MMaietTestResult& refTestResult)
{
	if (refTestResult.failures.size() == 0)
	{
		m_bFailure = false;
	}
	else
	{
		m_bFailure = true;
	}
}