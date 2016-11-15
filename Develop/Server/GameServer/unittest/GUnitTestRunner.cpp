#include "stdafx.h"
#include "GUnitTestRunner.h"
#include "GUnitTestStage1.h"
#include "GUnitTestStage2.h"
#include "GUnitTestStage3.h"
#include "GUnitTestStage4.h"
#include "SUnitTestStageAccountDB.h"
#include "SUnitTestStageGameDB.h"
#include "SUnitTestStageLogDB.h"
#include "GUnitTestTimeElapsedReporter.h"

bool GUnitTestRunner::BeDoTest()
{
	return MUnitTestRunner::BeDoTest();
}

bool GUnitTestRunner::RunStage1()
{
	return Run<GUnitTestStage1>();
}

bool GUnitTestRunner::RunStage2()
{
	return Run<GUnitTestStage2>();
}

bool GUnitTestRunner::RunStage3()
{
	return true;
	return Run<GUnitTestStage3>();
}

bool GUnitTestRunner::RunStage4()
{
	return Run<GUnitTestStage4>();
}

bool GUnitTestRunner::RunAccDBUnitTest()
{
	return Run<SUnitTestStageAccountDB>();
}

bool GUnitTestRunner::RunGameDBUnitTest()
{
	return Run<SUnitTestStageGameDB>();
}

bool GUnitTestRunner::RunLogDBUnitTest()
{
	return Run<SUnitTestStageLogDB>();
}

GUnitTestRunner::GUnitTestRunner() 
: MUnitTestRunner()
, m_pTimeElapsedReporter(NULL)
{
	InitCodeCoverage();
}

GUnitTestRunner::~GUnitTestRunner()
{
	CC_End();
}

void GUnitTestRunner::InitCodeCoverage()
{
	if (m_Config.m_bCheckCodeCoverage == false) return;

	char* CodeCoverageItems[] = {

#define CC_Add(e) #e,

#include "CodeCoverages.inc"

	};

#undef CC_Add

	CC_Begin(CodeCoverageItems, m_Config.m_strCodeCoverageFilter.c_str());
}

MUnitTestReporter* GUnitTestRunner::NewUnitTestReporter( std::ostream& ostream, bool bBreakOnFailure/*=false*/ )
{
	if (m_Config.m_bLogTimeElapsed)
	{
		m_pTimeElapsedReporter = new GUnitTestTimeElapsedReporter(ostream, bBreakOnFailure);
		return m_pTimeElapsedReporter;
	}

	return __super::NewUnitTestReporter(ostream, bBreakOnFailure);
}

GUnitTestRunner& GUnitTestRunner::GetInst()
{
	static GUnitTestRunner inst;
	return inst;
}

AutoTestCaseTimer::AutoTestCaseTimer(wstring strInput)
{
	GUnitTestTimeElapsedReporter* pReporter = 
		GUnitTestRunner::GetInst().GetTimeElapsedReporter();

	if (pReporter)
	{
		pReporter->BeginCaseTimer(strInput);
	}
}

AutoTestCaseTimer::~AutoTestCaseTimer()
{
	GUnitTestTimeElapsedReporter* pReporter = 
		GUnitTestRunner::GetInst().GetTimeElapsedReporter();

	if (pReporter)
	{
		pReporter->EndCaseTimer();
	}
}

wstring __cdecl AutoTestCaseTimer::SFormat( const wchar_t *pFormat, ... )
{
	static wchar_t temp[512];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	return temp;
}