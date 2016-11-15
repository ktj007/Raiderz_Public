#pragma once

#include "MUnitTestConfig.h"
#include "MBaseUnitTest.h"
#include "MUnitTestReporter.h"
#include <set>
#include <vector>
using namespace std;

class MUnitTestRunner
{
private:
	struct Filter
	{
		std::vector<std::string> vstrSelectedTestNames;
		std::vector<std::string> vstrSuiteNames;
		std::vector<std::string> vstrDisabledSuiteNames;
		std::vector<std::string> vstrSuiteAnyWords;
	};
	std::set<string>		m_setExcutedSuiteName;

	bool RunUnitTest(MBaseUnitTest* pUnitTest);

	int RunTests( MBaseUnitTest* pUnitTest, UnitTest::TestReporter& reporter, Filter& filter, int const maxTestTimeInMs );


	void MakeFilterFromConfig( Filter &outFilter );
	bool IsExistName(const char* szName, vector<string>& vstrNames);
	bool CheckFilter(const UnitTest::TestDetails* details, Filter& filter);
	void LogSuiteName(string& strSuiteName);
protected:
	MUnitTestConfig		m_Config;
	virtual bool BeDoTest();
	virtual MUnitTestReporter* NewUnitTestReporter(std::ostream& ostream, bool bBreakOnFailure=false);
public:
	MUnitTestRunner();
	virtual ~MUnitTestRunner() {}

	template<typename T1>
	bool Run()
	{
		if (!BeDoTest()) return true;

		bool ret = true;
		int repeat_count = max(1, m_Config.m_nRepeat);

		for (int i = 0; i < repeat_count; i++)
		{
			T1* pUnitTest = new T1();
			if (m_Config.CheckStageFilter(pUnitTest->GetName()) == false)
			{
				delete pUnitTest;
				return true;
			}

			pUnitTest->Create(m_Config);
			bool this_run_ret = RunUnitTest(pUnitTest);
			pUnitTest->Destroy();

			delete pUnitTest;

			if (this_run_ret == false) ret = false;
		}

		return ret;
	}
	MUnitTestConfig&	GetConfig() { return m_Config; }

	// ccnet에서의 결과값
	enum _CCNet_Result
	{
		CCNET_UNITTEST_SUCCESS	= 0,
		CCNET_UNITTEST_FAILED	= 1
	};
};
