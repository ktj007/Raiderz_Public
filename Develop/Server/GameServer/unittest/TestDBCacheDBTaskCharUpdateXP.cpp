#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskCharUpdateXP.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_UpdateXP)
	{
		TEST_FIXTURE(FIXTURE, TestDBCacheUpdateXPNotSync)
		{
			if (DBCACHE_SKEEP != DBCACHE_MAX_MOD_DURA)
			{
				MockDBManager db;

				const CID			nCID			= 2;
				const int			nIncXP			= 10;
				GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);
				const MUID			uidPlayer		= pPlayer->GetUID();
				GCharacterDBCache*	pch				= gsys.pDBCacheSystem->FindCharCache(uidPlayer);

				GDBT_CHAR_XP_DATA data(0, uidPlayer
					, nCID
					, 0
					, L"NULL"
					, pPlayer->GetPlayerInfo()->nXP + nIncXP
					, nIncXP
					, pPlayer->GetLevel()
					, pPlayer->GetMoney()
					, pPlayer->GetPlayerInfo()->nPlayTimeSec
					, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
					, 0);

				GDBTaskCharUpdateXP* t = new GDBTaskCharUpdateXP(uidPlayer);

				CHECK(t->Input(data));
				CHECK(!pch->IsSyncing());

				CHECK(db.Post(t));

				CHECK(!pch->IsSyncing());
				CHECK_EQUAL(1, pch->GetXP().GetModifiedCnt());
				CHECK_EQUAL(nIncXP, pch->GetXP().GetIncrease());
				CHECK_EQUAL(data.m_nXP, pPlayer->GetPlayerInfo()->nXP);
			}			
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheUpdateXPSyncSuccess)
		{
			if (DBCACHE_SKEEP != DBCACHE_MAX_MOD_DURA)
			{
				MockDBManager db;

				const CID			nCID			= 2;
				const int			nIncXP			= 10;
				GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);
				const MUID			uidPlayer		= pPlayer->GetUID();
				GCharacterDBCache*	pch				= gsys.pDBCacheSystem->FindCharCache(uidPlayer);

				pch->SetMaxModCount(0);

				GDBT_CHAR_XP_DATA data(0, uidPlayer
					, nCID
					, 0
					, L"NULL"
					, pPlayer->GetPlayerInfo()->nXP + nIncXP
					, nIncXP
					, pPlayer->GetLevel()
					, pPlayer->GetMoney()
					, pPlayer->GetPlayerInfo()->nPlayTimeSec
					, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
					, 0);

				GDBTaskCharUpdateXP* t = new GDBTaskCharUpdateXP(uidPlayer);

				CHECK(t->Input(data));
				CHECK(pch->IsSyncing());

				CHECK(db.Post(t));

				CHECK(!pch->IsSyncing());
				CHECK_EQUAL(0, pch->GetXP().GetModifiedCnt());
				CHECK_EQUAL(0, pch->GetXP().GetIncrease());
				CHECK_EQUAL(data.m_nXP, pPlayer->GetPlayerInfo()->nXP);
			}			
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheUpdateXPSyncFail)
		{
			if (DBCACHE_SKEEP != DBCACHE_MAX_MOD_DURA)
			{
				MockDBManager db;

				const CID			nCID			= 2;
				const int			nIncXP			= 10;
				GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);
				const MUID			uidPlayer		= pPlayer->GetUID();
				GCharacterDBCache*	pch				= gsys.pDBCacheSystem->FindCharCache(uidPlayer);

				pch->BeginCacheSync();

				GDBT_CHAR_XP_DATA data(0, uidPlayer
					, nCID
					, 0
					, L"NULL"
					, pPlayer->GetPlayerInfo()->nXP + nIncXP
					, nIncXP
					, pPlayer->GetLevel()
					, pPlayer->GetMoney()
					, pPlayer->GetPlayerInfo()->nPlayTimeSec
					, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
					, 0);

				GDBTaskCharUpdateXP* t = new GDBTaskCharUpdateXP(uidPlayer);

				CHECK(!t->Input(data));

				CHECK_EQUAL(0, pch->GetXP().GetModifiedCnt());
				CHECK_EQUAL(0, pch->GetXP().GetIncrease());
				CHECK_EQUAL(data.m_nXP - nIncXP, pPlayer->GetPlayerInfo()->nXP);
			}			
		}
	}
}
	