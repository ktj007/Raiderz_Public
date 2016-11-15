#include "stdafx.h"
#include "DummyClientUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include "MMemoryLeak.h"

void DummyClientUnitTestStage1::Create(MUnitTestConfig& config)
{

}

void DummyClientUnitTestStage1::Destroy()
{
}

void DummyClientUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}