#include "stdafx.h"
#include "MiNetUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include <string>
#include "MMemoryLeak.h"

void MiNetUnitTestStage1::Create(MUnitTestConfig& config)
{

}

void MiNetUnitTestStage1::Destroy()
{
	ClearCommandDescs();
}

void MiNetUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}