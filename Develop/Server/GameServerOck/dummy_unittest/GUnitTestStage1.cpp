#include "stdafx.h"
#include "GUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include "resource.h"
#include <string>
#include "MMemoryLeak.h"


void GUnitTestStage1::Create(MUnitTestConfig& config)
{

}

void GUnitTestStage1::Destroy()
{

}

void GUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}