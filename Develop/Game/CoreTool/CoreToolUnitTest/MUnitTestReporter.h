#ifndef _M_UNITTEST_REPORTER_H
#define _M_UNITTEST_REPORTER_H

#include <windows.h>
#include "XmlTestReporter.h"


class MUnitTestReporter : public UnitTest::XmlTestReporter
{
public:
	explicit MUnitTestReporter(std::ostream& ostream, bool bBreakOnFailure=false);
protected:
	bool m_bBreakOnFailure;
	BOOL _IsDebuggerPresent();
	virtual void ReportTestStart(UnitTest::TestDetails const& test);
	virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed);
	virtual void ReportFailure(UnitTest::TestDetails const& test, char const* failure);
	virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
};







#endif // _M_UNITTEST_REPORTER_H