#include "stdafx.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "SUnitTestUtil.h"

SUITE(PlayerObjectManager)
{
	class FPlayerObjectManager
	{
	public:
		FPlayerObjectManager()
		{

		}
		~FPlayerObjectManager()
		{

		}
		GPlayerObjectManager		mgr;

		GPlayerObject* NewAndAdd(MUID uidNew)
		{
			GPlayerObject* pPlayerObject = mgr.NewPlayerObject(uidNew);
			mgr.AddPlayer(pPlayerObject);

			return pPlayerObject;
		}
	};
	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_NewPlayerObject)
	{
		MUID uidNew = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = mgr.NewPlayerObject(uidNew);

		CHECK_EQUAL(uidNew, pPlayerObject->GetUID());


		pPlayerObject->Destroy();
		delete pPlayerObject;
	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_AddPlayer)
	{
		GPlayerObject* pPlayerObject = NewAndAdd(SUnitTestUtil::NewUID());

		CHECK_EQUAL(1, mgr.GetPlayersCount());
	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_DeletePlayer)
	{
		const MUID uidPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = NewAndAdd(uidPlayer);

		CHECK_EQUAL(1, mgr.GetPlayersCount());

		mgr.DeletePlayer(uidPlayer);
		CHECK_EQUAL(0, mgr.GetPlayersCount());
	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_DeleteAllPlayer)
	{
		GPlayerObject* pPlayerObject = NewAndAdd(SUnitTestUtil::NewUID());
		GPlayerObject* pPlayerObject2 = NewAndAdd(SUnitTestUtil::NewUID());

		CHECK_EQUAL(2, mgr.GetPlayersCount());

		mgr.DeleteAllPlayer();
		CHECK_EQUAL(0, mgr.GetPlayersCount());
	}


	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_RegUIID)
	{
		// 첫번째 플레이어 오브젝트
		const MUID uidPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = NewAndAdd(uidPlayer);

		CHECK_EQUAL(1, pPlayerObject->GetUIID());

		// 두번째 플레이어 오브젝트
		const MUID uidPlayer2 = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject2 = NewAndAdd(uidPlayer2);

		CHECK_EQUAL(2, pPlayerObject2->GetUIID());

		// 두번째 플레이어 오브젝트 삭제
		mgr.DeletePlayer(uidPlayer2);

		// 세번째 플레이어 오브젝트
		const MUID uidPlayer3 = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject3 = NewAndAdd(uidPlayer3);

		CHECK_EQUAL(2, pPlayerObject3->GetUIID());

	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_GetEntity_UIID)
	{
		// 플레이어 오브젝트 등록
		const MUID uidPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = NewAndAdd(uidPlayer);

		const UIID nPlayerUIID = pPlayerObject->GetUIID();

		// UIID로 검색
		GEntityPlayer* pEntityPlayer = mgr.GetEntity(nPlayerUIID);

		ASSERT_CHECK(pEntityPlayer != NULL);

		CHECK_EQUAL(nPlayerUIID, pEntityPlayer->GetUIID());
		CHECK_EQUAL(uidPlayer, pEntityPlayer->GetUID());
	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_GetEntity_UID)
	{
		// 플레이어 오브젝트 등록
		const MUID uidPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = NewAndAdd(uidPlayer);

		const UIID nPlayerUIID = pPlayerObject->GetUIID();

		// uid로 검색
		GEntityPlayer* pEntityPlayer = mgr.GetEntity(uidPlayer);

		ASSERT_CHECK(pEntityPlayer != NULL);

		CHECK_EQUAL(nPlayerUIID, pEntityPlayer->GetUIID());
		CHECK_EQUAL(uidPlayer, pEntityPlayer->GetUID());
	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_GetEntityInWorld_UID_Failure_State_Is_Not_POS_INWORLD)
	{
		// 플레이어 오브젝트 등록
		const MUID uidPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = NewAndAdd(uidPlayer);
		CHECK_EQUAL(POS_SELECT_CHAR, pPlayerObject->GetState());

		// uid로 검색
		GEntityPlayer* pEntityPlayer = mgr.GetEntityInWorld(uidPlayer);
		ASSERT_CHECK(pEntityPlayer == NULL);
	}

	TEST_FIXTURE(FPlayerObjectManager, TestPlayerObjectManager_GetEntityInWorld_UID_Success)
	{
		// 플레이어 오브젝트 등록
		const MUID uidPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPlayerObject = NewAndAdd(uidPlayer);
		pPlayerObject->OnStartGame();

		// uid로 검색
		GEntityPlayer* pEntityPlayer = mgr.GetEntityInWorld(uidPlayer);

		ASSERT_CHECK(pEntityPlayer != NULL);
		CHECK_EQUAL(uidPlayer, pEntityPlayer->GetUID());
	}

}