#include "stdafx.h"
#include "GAsyncTask.h"
#include "GUnitTestUtil.h"

SUITE(AsyncTask)
{
	class GTestAsyncTask : public GAsyncTask
	{
	public:
		GTestAsyncTask(MUID uid, uint32 nNowTime) : GAsyncTask(uid, nNowTime) {}
		virtual ~GTestAsyncTask() {}

		virtual GAsyncTaskID GetID() { return ASYNC_TASK_GATE_TO_SHAREDFIELD; }
	};

	TEST(TestAsyncTaskManager_AddAndDelete)
	{
		GAsyncTaskManager asyncTaskManager;

		MUID uidTask = SUnitTestUtil::NewUID();
		GTestAsyncTask* pAsyncTask = new GTestAsyncTask(uidTask, gsys.pSystem->GetNowTime());
		asyncTaskManager.Add(pAsyncTask);

		CHECK_EQUAL(1, pAsyncTask->GetReferenceCount());
		CHECK_EQUAL(1, asyncTaskManager.GetTaskCount());

		CHECK(asyncTaskManager.FindTask(uidTask) != NULL);

		asyncTaskManager.Delete(uidTask);

		CHECK_EQUAL(0, asyncTaskManager.GetTaskCount());
	}
}