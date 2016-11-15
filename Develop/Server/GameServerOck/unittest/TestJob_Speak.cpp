#include "StdAfx.h"
#include "GTestForward.h"
#include "GJob_Speak.h"
#include "GJob_Broker.h"

SUITE(JobSpeak)
{
	TEST(WaitingForScript)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(pField);
		GJob* pJob = GJobBroker::NewJob_Speak(pNPC->GetModuleAI(), L"shout", L"TEST", 12.0f) ;

		pJob->Start();
		CHECK_EQUAL(TR_RUNNING, pJob->Run(4.0f));
		CHECK_EQUAL(TR_RUNNING, pJob->Run(4.0f));
		CHECK_EQUAL(TR_COMPLETED, pJob->Run(4.0f));

		SAFE_DELETE(pJob);
		pField->Destroy();
	}

	TEST(NoWait)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(pField);
		GJob* pJob = GJobBroker::NewJob_Speak(pNPC->GetModuleAI(), L"shout", L"TEST", 0.0f) ;

		CHECK_EQUAL(false, pJob->Start());	// 즉시 종료

		SAFE_DELETE(pJob);
		pField->Destroy();
	}
}