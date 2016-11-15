#include "stdafx.h"
#include "CSIndexedIDMap.h"

SUITE(IndexedIDMap)
{
	class FIndexedIDMap
	{
	public:
		FIndexedIDMap()
		{

		}
		virtual ~FIndexedIDMap()
		{

		}
	};

	TEST(IndexedIDFinder_Add_ReturnValue)
	{
		CSIndexedIDMap<MUID> finder;

		const MUID uid1 = MUID(1, 1);
		const MUID uid2 = MUID(1, 2);
		const MUID uid3 = MUID(3, 3);

		UIID idxUID1 = finder.Add(uid1);
		UIID idxUID2 = finder.Add(uid2);
		UIID idxUID3 = finder.Add(uid3);

		CHECK_EQUAL(1, idxUID1);
		CHECK_EQUAL(2, idxUID2);
		CHECK_EQUAL(3, idxUID3);
	}

	TEST(IndexedIDFinder_Add_ReturnValue2)
	{
		CSIndexedIDMap<MUID, 10> finder;

		for (int i = 1; i <= 1000; i++)
		{
			MUID uid = MUID(0, i);
			UIID idxID = finder.Add(uid);
			CHECK_EQUAL(i, idxID);
		}
	}

	TEST(IndexedIDFinder_Find)
	{
		CSIndexedIDMap<MUID> finder;

		for (int i = 1; i <= 1000; i++)
		{
			MUID uid = MUID(0, i);
			UIID idxUID = finder.Add(uid);
		}

		for (int i = 1; i <= 1000; i++)
		{
			MUID uid = finder.Find(i);
			CHECK_EQUAL(MUID(0, i), uid);
		}
	}

	TEST(IndexedIDFinder_Find_Negative)
	{
		CSIndexedIDMap<MUID> finder;

		MUID uid = finder.Find(0);
		CHECK_EQUAL(0, uid);	// 존재하지 않으므로 0 리턴

		uid = finder.Find(1);
		CHECK_EQUAL(0, uid);	// 존재하지 않으므로 0 리턴

		uid = finder.Find(10000);
		CHECK_EQUAL(0, uid);	// 존재하지 않으므로 0 리턴
	}

	TEST(IndexedIDFinder_AddDel)
	{
		CSIndexedIDMap<MUID> finder;

		MUID uid1 = MUID(1, 1);
		MUID uid2 = MUID(1, 2);
		MUID uid3 = MUID(1, 3);

		UIID idxID = finder.Add(uid1);
		CHECK_EQUAL(1, idxID);
		CHECK_EQUAL(uid1, finder.Find(1));

		idxID = finder.Add(uid2);
		CHECK_EQUAL(2, idxID);
		CHECK_EQUAL(uid2, finder.Find(2));

		// 첫번째 넣은 UID를 제거
		finder.Del(1);	

		idxID = finder.Add(uid3);
		CHECK_EQUAL(1, idxID);
		CHECK_EQUAL(uid3, finder.Find(1));
	}

	TEST(IndexedIDFinder_Del)
	{
		CSIndexedIDMap<MUID> finder;

		finder.Add(MUID(1,1));
		CHECK_EQUAL(MUID(1,1), finder.Find(1));

		finder.Del(1);
		CHECK_EQUAL(MUID(0,0), finder.Find(1));


		// 2차
		finder.Add(MUID(55,55));
		CHECK_EQUAL(MUID(55,55), finder.Find(1));
		finder.Add(MUID(66,66));
		CHECK_EQUAL(MUID(66,66), finder.Find(2));

		finder.Del(2);
		CHECK_EQUAL(MUID(55,55), finder.Find(1));
		CHECK_EQUAL(MUID(0,0), finder.Find(2));

		finder.Del(3);
		CHECK_EQUAL(MUID(55,55), finder.Find(1));
		CHECK_EQUAL(MUID(0,0), finder.Find(2));

		finder.Del(1);
		CHECK_EQUAL(MUID(0,0), finder.Find(1));
		CHECK_EQUAL(MUID(0,0), finder.Find(2));
	}

	TEST(IndexedIDFinder_Del_Negative)
	{
		CSIndexedIDMap<MUID> finder;

		finder.Del(10000);		// 엄한 값 입력해서 뻗으면 안된다.
	}

	TEST(IndexedIDFinder_Del_Negative2)
	{
		CSIndexedIDMap<MUID> finder;

		// 이미 삭제되어 있으면 NotUsedQueue에 넣지 않는다.

		finder.Add(MUID(0, 1));

		finder.Del(1);
		finder.Del(1);	// 중복 삭제

		UIID nIdxId = finder.Add(MUID(0, 2));
		CHECK_EQUAL(1, nIdxId);

		nIdxId = finder.Add(MUID(0, 3));
		CHECK_EQUAL(2, nIdxId);		// 새로 할당

	}

	TEST(IndexedIDFinder_NotUsedQueue)
	{
		CSIndexedIDMap<MUID, 4> finder;

		for (int i = 0; i < 10; i++)
		{
			finder.Add(MUID(0, i+1));
		}

		finder.Del(6);
		finder.Del(9);
		finder.Del(10);

		// 삭제했던 인덱스를 재활용한다.
		UIID nIdxID = finder.Add(MUID(11, 11));
		CHECK_EQUAL(6, nIdxID);

		nIdxID = finder.Add(MUID(11, 12));
		CHECK_EQUAL(9, nIdxID);

		nIdxID = finder.Add(MUID(11, 13));
		CHECK_EQUAL(10, nIdxID);

		nIdxID = finder.Add(MUID(11, 14));
		CHECK_EQUAL(11, nIdxID);
	}

	//////////////////////////////////////////////////////////////////////////
	// GIndexedNPCUIDMap

	TEST(GIndexedNPCUIDMap_Add_ReturnValue)
	{
		GIndexedNPCUIDMap finder;

		const MUID uid1 = MUID(1, 1);
		const MUID uid2 = MUID(1, 2);
		const MUID uid3 = MUID(3, 3);

		UIID idxUID1 = finder.Add(uid1);
		UIID idxUID2 = finder.Add(uid2);
		UIID idxUID3 = finder.Add(uid3);

		CHECK_EQUAL(NPC_UIID_SEED + 1, idxUID1);
		CHECK_EQUAL(NPC_UIID_SEED + 2, idxUID2);
		CHECK_EQUAL(NPC_UIID_SEED + 3, idxUID3);
	}

	TEST(GIndexedNPCUIDMap_Find)
	{
		GIndexedNPCUIDMap finder;

		for (int i = 1; i <= 1000; i++)
		{
			MUID uid = MUID(0, i);
			finder.Add(uid);
		}

		for (int i = 1; i <= 1000; i++)
		{
			MUID uid = finder.Find(NPC_UIID_SEED+i);
			CHECK_EQUAL(MUID(0, i), uid);
		}
	}

	TEST(GIndexedNPCUIDMap_Negative)
	{
		GIndexedNPCUIDMap finder;

		MUID uid = finder.Find(NPC_UIID_SEED+0);
		CHECK_EQUAL(0, uid);	// 존재하지 않으므로 0 리턴

		uid = finder.Find(NPC_UIID_SEED+1);
		CHECK_EQUAL(0, uid);	// 존재하지 않으므로 0 리턴

		uid = finder.Find(NPC_UIID_SEED+10000);
		CHECK_EQUAL(0, uid);	// 존재하지 않으므로 0 리턴
	}


	TEST(GIndexedNPCUIDMap_AddDel)
	{
		GIndexedNPCUIDMap finder;

		MUID uid1 = MUID(1, 1);
		MUID uid2 = MUID(1, 2);
		MUID uid3 = MUID(1, 3);

		UIID idxID = finder.Add(uid1);
		CHECK_EQUAL(NPC_UIID_SEED+1, idxID);
		CHECK_EQUAL(uid1, finder.Find(NPC_UIID_SEED+1));

		idxID = finder.Add(uid2);
		CHECK_EQUAL(NPC_UIID_SEED+2, idxID);
		CHECK_EQUAL(uid2, finder.Find(NPC_UIID_SEED+2));

		// 첫번째 넣은 UID를 제거
		finder.Del(NPC_UIID_SEED+1);	

		idxID = finder.Add(uid3);
		CHECK_EQUAL(NPC_UIID_SEED+1, idxID);
		CHECK_EQUAL(uid3, finder.Find(NPC_UIID_SEED+1));
	}

	TEST(GIndexedNPCUIDMap_Del)
	{
		GIndexedNPCUIDMap finder;

		finder.Add(MUID(1,1));
		CHECK_EQUAL(MUID(1,1), finder.Find(NPC_UIID_SEED+1));

		finder.Del(NPC_UIID_SEED+1);
		CHECK_EQUAL(MUID(0,0), finder.Find(NPC_UIID_SEED+1));


		// 2차
		finder.Add(MUID(55,55));
		CHECK_EQUAL(MUID(55,55), finder.Find(NPC_UIID_SEED+1));
		finder.Add(MUID(66,66));
		CHECK_EQUAL(MUID(66,66), finder.Find(NPC_UIID_SEED+2));

		finder.Del(NPC_UIID_SEED+2);
		CHECK_EQUAL(MUID(55,55), finder.Find(NPC_UIID_SEED+1));
		CHECK_EQUAL(MUID(0,0), finder.Find(NPC_UIID_SEED+2));

		finder.Del(NPC_UIID_SEED+3);
		CHECK_EQUAL(MUID(55,55), finder.Find(NPC_UIID_SEED+1));
		CHECK_EQUAL(MUID(0,0), finder.Find(NPC_UIID_SEED+2));

		finder.Del(NPC_UIID_SEED+1);
		CHECK_EQUAL(MUID(0,0), finder.Find(NPC_UIID_SEED+1));
		CHECK_EQUAL(MUID(0,0), finder.Find(NPC_UIID_SEED+2));
	}

	TEST(GIndexedNPCUIDMap_Del_Negative)
	{
		GIndexedNPCUIDMap finder;

		finder.Del(10000);		// 엄한 값 입력해서 뻗으면 안된다.
	}

	TEST(GIndexedNPCUIDMap_Del_Negative2)
	{
		GIndexedNPCUIDMap finder;

		// 이미 삭제되어 있으면 NotUsedQueue에 넣지 않는다.

		finder.Add(MUID(0, 1));

		finder.Del(NPC_UIID_SEED+1);
		finder.Del(NPC_UIID_SEED+1);	// 중복 삭제

		UIID nIdxId = finder.Add(MUID(0, 2));
		CHECK_EQUAL(NPC_UIID_SEED+1, nIdxId);

		nIdxId = finder.Add(MUID(0, 3));
		CHECK_EQUAL(NPC_UIID_SEED+2, nIdxId);		// 새로 할당
	}

	TEST(IsPlayerUIID)
	{
		CHECK_EQUAL(false, IsPlayerUIID(PLAYER_UIID_SEED));
		CHECK_EQUAL(true, IsPlayerUIID(PLAYER_UIID_SEED+1));

		CHECK_EQUAL(true, IsPlayerUIID(PLAYER_UIID_SEED + MAX_PLAYER_INDEXED_ID));
		CHECK_EQUAL(false, IsPlayerUIID(PLAYER_UIID_SEED + MAX_PLAYER_INDEXED_ID+1));
	}

	TEST(IsNPCUIID)
	{
		CHECK_EQUAL(false, IsNPCUIID(NPC_UIID_SEED));
		CHECK_EQUAL(true, IsNPCUIID(NPC_UIID_SEED+1));

		CHECK_EQUAL(true, IsNPCUIID(NPC_UIID_SEED + MAX_NPC_INDEXED_ID));
		CHECK_EQUAL(false, IsNPCUIID(NPC_UIID_SEED + MAX_NPC_INDEXED_ID+1));
	}
}