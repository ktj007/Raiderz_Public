#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskCharLevelUp.h"
#include "GPlayerTalent.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_LevelUp)
	{
		TEST_FIXTURE(FIXTURE, TestDBCacheLevelUpSuccess)
		{
			MockDBManager db;

			const CID			nCID			= 2;
			GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const uint8			nNewLevel		= 2;
			const int			nNewXP			= 10;
			const int16			nRemainTP		= pPlayer->GetTalent().GetTP();
			const int16			nTotalTP		= pPlayer->GetTalent().GetTotalTP();			
			const MUID			uidPlayer		= pPlayer->GetUID();
			GCharacterDBCache*	pch				= gsys.pDBCacheSystem->FindCharCache(uidPlayer);

			pch->IncXP(1, false);

			GDBT_CHAR_LEVEL_UP_DATA data(0
				, uidPlayer
				, nCID
				, 0
				, L"NULL"
				, nNewXP
				, nNewXP
				, nNewLevel
				, 0
				, nRemainTP
				, 1
				, 0
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetFieldID()
				, pPlayer->GetPos());

			GDBTaskCharLevelUp* pTask = new GDBTaskCharLevelUp(uidPlayer);

			CHECK(pTask->Input(data));
			CHECK(pch->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK_EQUAL(0, pch->GetXP().GetModifiedCnt());
			CHECK_EQUAL(0, pch->GetXP().GetIncrease());
			CHECK_EQUAL(data.m_nXP, pPlayer->GetPlayerInfo()->nXP);
			CHECK_EQUAL(data.m_nLevel, pPlayer->GetPlayerInfo()->nLevel);
			CHECK_EQUAL(data.m_nRemainTP+1, pPlayer->GetTalent().GetTP());
		}

		TEST_FIXTURE(FIXTURE, TestDBCacheLevelUpFail)
		{
			const CID			nCID			= 2;
			const uint8			nNewLevel		= 2;
			const int			nNewXP			= 10;
			const int16			nRemainTP		= 10;
			const int16			nTotalTP		= 10;
			GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const MUID			uidPlayer		= pPlayer->GetUID();
			GCharacterDBCache*	pch				= gsys.pDBCacheSystem->FindCharCache(uidPlayer);
			const int			nXP				= pPlayer->GetPlayerInfo()->nXP;
			const uint8			nLevel			= pPlayer->GetPlayerInfo()->nLevel;
			const int16			nBefRemainTP	= pPlayer->GetTalent().GetTP();
			const int16			nBefTotalTP		= pPlayer->GetTalent().GetTotalTP();

			pch->BeginCacheSync();

			GDBT_CHAR_LEVEL_UP_DATA data(0
				, uidPlayer
				, nCID
				, 0
				, L"NULL"
				, nNewXP
				, nNewXP
				, nNewLevel
				, 0
				, nRemainTP
				, 1
				, 0
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetFieldID()
				, pPlayer->GetPos());

			GDBTaskCharLevelUp* pTask = new GDBTaskCharLevelUp(uidPlayer);

			CHECK(!pTask->Input(data));
			
			CHECK(pch->IsSyncing());
			CHECK_EQUAL(0, pch->GetXP().GetModifiedCnt());
			CHECK_EQUAL(0, pch->GetXP().GetIncrease());
			CHECK_EQUAL(nXP, pPlayer->GetPlayerInfo()->nXP);
			CHECK_EQUAL(nLevel, pPlayer->GetPlayerInfo()->nLevel);
			CHECK_EQUAL(nBefRemainTP, pPlayer->GetTalent().GetTP());
			CHECK_EQUAL(nBefTotalTP, pPlayer->GetTalent().GetTotalTP());
		}
	}
}