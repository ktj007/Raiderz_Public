#pragma once

#include "MTest.h"

class GUnitTestTimeElapsedReporter;

class GUnitTestRunner : public MUnitTestRunner
{
private:
	GUnitTestTimeElapsedReporter* m_pTimeElapsedReporter;
private:
	virtual MUnitTestReporter* NewUnitTestReporter(std::ostream& ostream, bool bBreakOnFailure=false);
protected:
	virtual bool BeDoTest();
	void InitCodeCoverage();
	
public:
	GUnitTestRunner();
	virtual ~GUnitTestRunner();
	bool RunStage1();
	bool RunStage2();
	bool RunStage3();
	bool RunStage4();
	bool RunAccDBUnitTest();
	bool RunGameDBUnitTest();
	bool RunLogDBUnitTest();

	static GUnitTestRunner& GetInst();
	GUnitTestTimeElapsedReporter* GetTimeElapsedReporter() { return m_pTimeElapsedReporter; }
};

class AutoTestCaseTimer
{
public:
	AutoTestCaseTimer(wstring strInput=L"");
	~AutoTestCaseTimer();

	static wstring __cdecl SFormat(const wchar_t *pFormat, ...);
};
