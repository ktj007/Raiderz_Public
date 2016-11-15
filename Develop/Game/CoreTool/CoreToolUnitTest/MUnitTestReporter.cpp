#include "stdafx.h"
#include "MUnitTestReporter.h"
#include "TestDetails.h"
#include "MDebug.h"

MUnitTestReporter::MUnitTestReporter( std::ostream& ostream, bool bBreakOnFailure/*=false*/ ) 
						: UnitTest::XmlTestReporter(ostream), m_bBreakOnFailure(bBreakOnFailure)
{

}

BOOL MUnitTestReporter::_IsDebuggerPresent()
{
	// IsDebuggerPresent() 함수도 있지만 
	// 프로세스의 PEB구조체에 BeingDebuged의 값을 가져오는 인라인 어셈을 직접 만듦. - birdkr

	BOOL Retval = 0;
	__asm
	{
		push eax
		mov eax,dword ptr fs:[0x18]
		mov eax,dword ptr ds:[eax+0x30]
		movzx eax,byte ptr ds:[eax+0x2]
		mov Retval,eax
		pop eax
	}
	return Retval;
}

void MUnitTestReporter::ReportFailure(UnitTest::TestDetails const& details, char const* failure)
{
	if (m_bBreakOnFailure && _IsDebuggerPresent() == TRUE)
	{
		// 실패시 브레이크 걸도록 설정되어 있으면 명시적으로 뻗게 만든다.
		*static_cast<int*>(NULL) = 1;
	}

	UnitTest::XmlTestReporter::ReportFailure(details, failure);

	char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";

	mlog("\n\n >>> UnitTest Failure -----\n");
	mlog(errorFormat, details.filename, details.lineNumber, details.testName, failure);
	mlog("\n\n");
}

void MUnitTestReporter::ReportTestStart(UnitTest::TestDetails const& details)
{
	XmlTestReporter::ReportTestStart(details);
}

void MUnitTestReporter::ReportTestFinish(UnitTest::TestDetails const& details, float secondsElapsed)
{
	XmlTestReporter::ReportTestFinish(details, secondsElapsed);
}

void MUnitTestReporter::ReportSummary(int const totalTestCount, int const failedTestCount,
	int const failureCount, float secondsElapsed)
{
	UnitTest::XmlTestReporter::ReportSummary(totalTestCount, failedTestCount, failureCount, secondsElapsed);

	if (failureCount > 0)
		mlog("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
	else
	{
		mlog("\n★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆\n\n");
		mlog("Success: %d tests passed.\n", totalTestCount);
		mlog("\n★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆\n\n");
	}
	mlog("Test time: %.2f seconds.\n\n", secondsElapsed);
}

