#include "stdafx.h"
#include "GDBCacheRegularFlush.h"
#include "GPlayerDBCache.h"
#include "GSystem.h"
#include "SUnitTestUtil.h"
#include "GDBCacheFlush.h"
#include "GTestForward.h"
#include "GCharacterDBCache.h"

SUITE(TestDBCache_RegularFlush)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);

			m_pCache = new GPlayerDBCache(m_pPlayer->GetUID(), m_pPlayer->GetCID(), new GCharacterDBCache(m_pPlayer->GetUID(), m_pPlayer->GetCID()));
			gmgr.pPlayerDBCacheMgr->Insert(m_pCache);
		}

		bool IsFlushed(GPlayerDBCache* pCache)
		{
			return 0 < pCache->GetLastFlushTime();
		}

		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GPlayerDBCache*		m_pCache;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;		
	};

	TEST_FIXTURE(Fixture, NoFlushNeedCache_CachingIsEmpty)
	{
		GDBCacheRegularFlush regularFlush;
		regularFlush.Update(_FLUSHCHECK_TIME);

		CHECK_EQUAL(false, IsFlushed(m_pCache));
	}

	TEST_FIXTURE(Fixture, NoFlushNeedCache_NotLastFlushTimeYet)
	{
		// LastFlushTime 세팅
		m_pCache->BeginFlush();
		m_pCache->EndFlush();

		gsys.pSystem->Sleep(_FLUSH_TIME/1000-1);

		m_pCache->GetCharCc()->IncMoney(1);
		m_pCache->GetCharCc()->EndCacheSync(false);

		GDBCacheRegularFlush regularFlush;
		regularFlush.Update(_FLUSHCHECK_TIME);

		CHECK_EQUAL(false, IsFlushed(m_pCache));
	}


	TEST_FIXTURE(Fixture, FlushNeedCache)
	{
		// LastFlushTime 세팅
		m_pCache->BeginFlush();
		m_pCache->EndFlush();

		gsys.pSystem->Sleep(_FLUSH_TIME);

		m_pCache->GetCharCc()->IncMoney(1);
		m_pCache->GetCharCc()->EndCacheSync(false);

		GDBCacheRegularFlush regularFlush;
		regularFlush.Update(_FLUSHCHECK_TIME);

		CHECK_EQUAL(true, IsFlushed(m_pCache));
	}

}