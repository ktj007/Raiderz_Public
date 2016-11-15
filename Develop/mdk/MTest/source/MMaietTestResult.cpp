#include "StdAfx.h"
#include "MMaietTestResult.h"

MMaietTestResult::MMaietTestResult()
: testName("")
, timeElapsed(0.0f)
{
}

MMaietTestResult::MMaietTestResult(const char* test)
: testName(test)
, timeElapsed(0.0f)
{
}