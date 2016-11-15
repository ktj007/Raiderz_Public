#include "stdafx.h"
#include "SUnitTest.h"
#include "GDBAsyncTask.h"
#include "MDatabase.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "MockField.h"
#include "GDBTaskHeaderList.h"
#include "GUTHelper_Field.h"
#include "MockDBTask.h"

SUITE(DBTestGDBTask)
{

	struct FIXTURE
	{
		FIXTURE()
		{
			pField = GUTHelper_Field::DefaultMockField();			
			pPlayer = GUTHelper::NewEntityPlayer(pField);
		}

		virtual ~FIXTURE()
		{
			pField->Destroy();
		}

		MockField*		pField;	
		GEntityPlayer*	pPlayer;
		GUTHelper		helper;
	};

	TEST(CreateGDBTask)
	{
		const MUID uidReqPlayer(123, 456);
		MockDBTask t(uidReqPlayer);
		CHECK_EQUAL(uidReqPlayer, t.GetReqPlayer());
	}

	TEST_FIXTURE(FIXTURE, DBAsyncRefCount)
	{
		MockDBTask t(pPlayer->GetUID());
		CHECK_EQUAL(0, pPlayer->GetDBAsyncMonitor().GetRefCount());
		t.IncreaseRefCount(pPlayer->GetUID());
		CHECK_EQUAL(1, pPlayer->GetDBAsyncMonitor().GetRefCount());
		t.IncreaseRefCount(pPlayer->GetUID());
		CHECK_EQUAL(2, pPlayer->GetDBAsyncMonitor().GetRefCount());
		t.DecreaseRefCount(pPlayer->GetUID());
		CHECK_EQUAL(1, pPlayer->GetDBAsyncMonitor().GetRefCount());
		t.IncreaseRefCount(pPlayer->GetUID());
		CHECK_EQUAL(2, pPlayer->GetDBAsyncMonitor().GetRefCount());
		t.DecreaseRefCount(pPlayer->GetUID());
		CHECK_EQUAL(1, pPlayer->GetDBAsyncMonitor().GetRefCount());
		t.DecreaseRefCount(pPlayer->GetUID());
		CHECK_EQUAL(0, pPlayer->GetDBAsyncMonitor().GetRefCount());		
	}
}