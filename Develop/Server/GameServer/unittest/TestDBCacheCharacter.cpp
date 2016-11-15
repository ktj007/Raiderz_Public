#include "stdafx.h"
#include "GCharacterDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_Character)
{
	class MockDBCacheCharacter : public GCharacterDBCache
	{
	public :
		MockDBCacheCharacter(const MUID& uidPlayer, const int nCID) : GCharacterDBCache(uidPlayer, nCID) {}

		cache_Money GetMoney() { return m_cacheMoney; }

		using GCharacterDBCache::IsOverflowModifiedRange;
		using GCharacterDBCache::IsOverflowModifiedCnt;
		using GCharacterDBCache::SetSyncTm;
		using GCharacterDBCache::GetLastSyncTm;
		using GCharacterDBCache::CalcModifiedCnt;
		using GCharacterDBCache::StartCaching;

		void SetTestPlayerDBCache(GPlayerDBCache* pc)
		{
			m_pc = pc;
		}

		virtual GPlayerDBCache*	GetPlayerDBCache() override 
		{
			return m_pc;
		}

		GPlayerDBCache* m_pc;
	};

	TEST(DBCacheCharacterCreate)
	{
		const MUID	uidPlayer	= 1;
		const int	nCID		= 2;

		MockDBCacheCharacter cc(uidPlayer, nCID);

		CHECK_EQUAL(nCID, cc.GetCID());
		CHECK_EQUAL(0, cc.GetMoney().GetIncrease());
		CHECK_EQUAL(0, cc.GetMoney().GetDecrease());
		CHECK(!cc.IsSyncing());
		CHECK(!cc.IsModified());
		CHECK(!cc.IsOverflowModifiedCnt());
		CHECK(!cc.IsOverflowModifiedRange());
	}	

	TEST(TestCharacterDBCache_Sync)
	{
		MockDBCacheCharacter* pCh = new MockDBCacheCharacter(1, 2);
		GPlayerDBCache pc(pCh->GetPlayerUID(), pCh->GetCID(), pCh);
		pCh->SetTestPlayerDBCache(&pc);

		pCh->BeginCacheSync();
		CHECK_EQUAL(1, pc.GetSyncingCnt());
	}

	TEST(TestCharacterDBCache_EndSync_SetZero_True)
	{
		MockDBCacheCharacter* cc = new MockDBCacheCharacter(1, 2);
		GPlayerDBCache pc(cc->GetPlayerUID(), cc->GetCID(), cc);
		cc->SetTestPlayerDBCache(&pc);
		
		cc->IncMoney(1, false);
		cc->DecMoney(1, false);
		cc->IncXP(1, false);
		cc->DecXP(1, false);
		cc->SetSyncTm(0);

		cc->GetPlayerDBCache()->IncreaseSyncCnt();

		cc->EndCacheSync(true);

		CHECK_EQUAL(0, cc->GetMoney().GetIncrease());
		CHECK_EQUAL(0, cc->GetMoney().GetDecrease());
		CHECK_EQUAL(0, cc->GetXP().GetIncrease());
		CHECK_EQUAL(0, cc->GetXP().GetDecrease());
		CHECK(0 < cc->GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_EndSync_SetZero_False)
	{
		MockDBCacheCharacter* cc = new MockDBCacheCharacter(1, 2);
		GPlayerDBCache pc(cc->GetPlayerUID(), cc->GetCID(), cc);
		cc->SetTestPlayerDBCache(&pc);
		
		cc->IncMoney(1, false);
		cc->DecMoney(1, false);
		cc->IncXP(1, false);
		cc->DecXP(1, false);
		cc->SetSyncTm(0);

		cc->GetPlayerDBCache()->IncreaseSyncCnt();

		cc->EndCacheSync(false);

		CHECK_EQUAL(1, cc->GetMoney().GetIncrease());
		CHECK_EQUAL(1, cc->GetMoney().GetDecrease());
		CHECK_EQUAL(1, cc->GetXP().GetIncrease());
		CHECK_EQUAL(1, cc->GetXP().GetDecrease());
		CHECK_EQUAL(0, cc->GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_StartCaching_Success)
	{
		MockDBCacheCharacter cc(0, 0);
		cc.SetSyncTm(0);
		cc.StartCaching();

		CHECK(0 < cc.GetLastSyncTm());
	}

	TEST(TestCharacterDBCache_StartCaching_Fail)
	{
		MockDBCacheCharacter* cc = new MockDBCacheCharacter(1, 2);
		GPlayerDBCache pc(cc->GetPlayerUID(), cc->GetCID(), cc);
		cc->SetTestPlayerDBCache(&pc);

		cc->IncMoney(1);
		cc->SetSyncTm(0);
		cc->StartCaching();

		CHECK(0 == cc->GetLastSyncTm());
	}	
}