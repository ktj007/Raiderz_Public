#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskCharIncMoney.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_DBTaskCharIncMoney)
	{
		TEST_FIXTURE(FIXTURE, TestIncMoneySyncSuccess)
		{
			MockDBManager	db;
			const CID		nCID			= 2;
			const int		nIncMoney		= 3;
			const int		nMoney			= 4;
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const MUID		uidPlayer		= pPlayer->GetUID();

			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->SetMaxModCount(0);

			GDBTaskCharIncMoney* pTask = new GDBTaskCharIncMoney(uidPlayer, SDBTID_CHAR_INC_MONEY);

			CHECK(pTask->Input(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), 0
				, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() + nIncMoney, nIncMoney, false, 0)));
			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(db.Post(pTask));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(0, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetIncrease());			
 			CHECK_EQUAL(nIncMoney, pPlayer->GetMoney());				
		}

		TEST_FIXTURE(FIXTURE, TestIncMoneySyncFail)
		{
			const CID		nCID			= 2;
			const int		nIncMoney		= 3;
			const int		nMoney			= 4;
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const MUID		uidPlayer		= pPlayer->GetUID();

			// 동기화 실패를 하기 위해 먼저 동기화 진행중으로 표시한다.
			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->BeginCacheSync();

			GDBTaskCharIncMoney t(uidPlayer, SDBTID_CHAR_INC_MONEY);

			CHECK(!t.Input(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), 0
				, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() + nIncMoney, nIncMoney, false, 0)));

			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());			
			CHECK_EQUAL(0, pPlayer->GetMoney());	
		}

		TEST_FIXTURE(FIXTURE, TestIncMoneyCacheNotSync)
		{
			MockDBManager	db;
			const CID		nCID			= 2;
			const int		nIncMoney		= 3;
			const int		nMoney			= 4;
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const MUID		uidPlayer		= pPlayer->GetUID();

			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->SetMaxModCount(DBCACHE_CHAR_MAX_MOD_COUNT);

			GDBTaskCharIncMoney* pTask = new GDBTaskCharIncMoney(uidPlayer, SDBTID_CHAR_INC_MONEY);

			CHECK(pTask->Input(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), 0
				, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() + nIncMoney, nIncMoney, false, 0)));
			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(db.Post(pTask));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(1, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetModifiedCnt());
			CHECK_EQUAL(nIncMoney, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetIncrease());
			CHECK_EQUAL(nIncMoney, pPlayer->GetMoney());
		}
	}
}