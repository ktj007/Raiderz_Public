#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskCharDecMoney.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_DBTaskCharDecMoney)
	{
		TEST_FIXTURE(FIXTURE, TestDecMoneySyncSuccess)
		{
			MockDBManager	db;
			const CID		nCID			= 2;
			const int		nMoney			= 10;
			const int		nDecMoney		= 1;
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, nMoney);
			const MUID		uidPlayer		= pPlayer->GetUID();

			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->SetMaxModCount(0);
	
			GDBTaskCharDecMoney* pTask = new GDBTaskCharDecMoney(uidPlayer, SDBTID_CHAR_DEC_MONEY);

			CHECK(pTask->Input(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), 0
				, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() - nDecMoney, nDecMoney, false, 0)));
			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(db.Post(pTask));
			
			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(0, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetDecrease());
			CHECK_EQUAL(nMoney - nDecMoney, pPlayer->GetMoney());		
		}

		TEST_FIXTURE(FIXTURE, TestDecMoneySyncFail)
		{
			const CID		nCID			= 2;
			const int		nMoney			= 10;
			const int		nDecMoney		= 1;
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const MUID		uidPlayer		= pPlayer->GetUID();

			// 동기화 실패를 하기 위해 먼저 동기화 진행중으로 표시한다.
			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->BeginCacheSync();

			GDBTaskCharDecMoney t(uidPlayer, SDBTID_CHAR_INC_MONEY);

			CHECK(!t.Input(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), 0
				, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() - nDecMoney, nDecMoney, false, 0)));

			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());			
			CHECK_EQUAL(0, pPlayer->GetMoney());	
		}

		TEST_FIXTURE(FIXTURE, TestDecMoneyCacheNotSync)
		{
			MockDBManager	db;
			const CID		nCID			= 2;
			const int		nMoney			= 10;
			const int		nDecMoney		= 1;
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, nMoney);
			const MUID		uidPlayer		= pPlayer->GetUID();

			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->SetMaxModCount(DBCACHE_CHAR_MAX_MOD_COUNT);

			GDBTaskCharDecMoney* pTask = new GDBTaskCharDecMoney(uidPlayer, SDBTID_CHAR_DEC_MONEY);

			CHECK(pTask->Input(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), 0
				, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() - nDecMoney, nDecMoney, false, 0)));
			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(db.Post(pTask));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(1, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetModifiedCnt());
			CHECK_EQUAL(nDecMoney, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetDecrease());			
			CHECK_EQUAL(nMoney - nDecMoney, pPlayer->GetMoney());
		}
	}
}