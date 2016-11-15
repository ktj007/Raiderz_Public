#include "StdAfx.h"
#include "GTestForward.h"
#include "GTask_Idle.h"
#include "GTask_Broker.h"

SUITE(TaskIdle)
{
	TEST(CheckRunning)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(pField);
		GTaskBroker TaskBroker;
		GTask_Idle* pTask = static_cast<GTask_Idle*>(TaskBroker.New_Idle(pNPC, 12.0f));

		pTask->Start();
		CHECK_EQUAL(TR_RUNNING, pTask->Run(4.0f));
		CHECK_EQUAL(TR_RUNNING, pTask->Run(4.0f));
		CHECK_EQUAL(TR_COMPLETED, pTask->Run(4.0f));

		SAFE_DELETE(pTask);
		pField->Destroy();
	}
}