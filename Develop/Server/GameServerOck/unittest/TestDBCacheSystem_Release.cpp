#include "stdafx.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GDBCacheSystem.h"
#include "GTestWrapper.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GUTHelper_Item.h"
#include "MockDBManager.h"
#include "GDBTaskCacheFlush.h"


SUITE(TestDBCache_DBCacheSystem_Release)
{
	class MockDB : public MockDBManager
	{
	public :
		MockDB() : m_pTask(NULL) {}
		~MockDB()
		{
			if (NULL != m_pTask)
				SAFE_DELETE(m_pTask);
		}

		virtual bool Post(GDBAsyncTask* pTask) override 
		{
			m_pTask = (GDBTaskCacheFlush*)pTask;

			m_pTask->OnPrePost();
			m_pTask->GetCharQuery().SetSuccess();

			for (size_t i = 0; i < m_pTask->GetItemQueryVec().size(); ++i)
				m_pTask->GetItemQueryVec()[i].SetSuccess();

			m_pTask->SetTaskSuccess();
			m_pTask->OnCompleted();
			
			return true;
		}

		GDBTaskCacheFlush* m_pTask;
	};

	class FIXTURE_RELEASE
	{
	public :
		FIXTURE_RELEASE()
		{
			m_pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(1);
			m_pPlayerObj->GetAccountInfo().nAID = 1;
			m_pPlayerObj->GetEntity()->GetPlayerInfo()->nCID = 1;
			gmgr.pPlayerObjectManager->AddPlayer(m_pPlayerObj);
			m_pItem1 = GUTHelper_Item::GiveNewItem(m_pPlayerObj->GetEntity());
			m_pItem2 = GUTHelper_Item::GiveNewItem(m_pPlayerObj->GetEntity());

			FindItem(m_pPlayerObj, m_pItem1);
			FindItem(m_pPlayerObj, m_pItem2);
		}

		GItemDBCache* FindItem(GPlayerObject* pPlayerObj, GItem* pItem)
		{
			return gsys.pDBCacheSystem->FindItemCache(pPlayerObj->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
		}

		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;
		GTestSysWrapper<GDBCacheSystem>			m_pDBCacheSystem;
		GTestSysWrapper2<GDBManager, MockDB>	m_pDB;
		
		GPlayerObject*	m_pPlayerObj;
		GItem*			m_pItem1;
		GItem*			m_pItem2;
	};

	class MockDBCacheSystem : public GDBCacheSystem
	{
	public :
	};

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_Empty)
	{
		CHECK(!gsys.pDBCacheSystem->FlushAllCache());
	}

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_NoSyncCache)
	{
		gsys.pDBCacheSystem->FindCharCache(m_pPlayerObj->GetUID());
		
		CHECK(!gsys.pDBCacheSystem->FlushAllCache());
	}

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_CharHasDBCache)
	{
		GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(m_pPlayerObj->GetUID());
		cc->IncMoney(1, false);

		CHECK(gsys.pDBCacheSystem->FlushAllCache());
		
		CHECK(!cc->IsSyncing());
		CHECK(!cc->IsModified());
	}

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_FirstItemHasDBCache)
	{
		GItemDBCache* ic = FindItem(m_pPlayerObj, m_pItem1);
		ic->IncStackAmt(1, false);

		CHECK(gsys.pDBCacheSystem->FlushAllCache());
		CHECK(1 == gsys.pDBCacheSystem->GetItemCount(m_pPlayerObj->GetUID()));
		CHECK(NULL != gmgr.pPlayerDBCacheMgr->Find(m_pPlayerObj->GetUID())->GetItemCcMgr().Find(m_pItem2->m_nIUID));
	}

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_SecondItemHasDBCache)
	{
		GItemDBCache* ic = FindItem(m_pPlayerObj, m_pItem2);
		ic->IncStackAmt(1, false);

		CHECK(gsys.pDBCacheSystem->FlushAllCache());
		CHECK(1 == gsys.pDBCacheSystem->GetItemCount(m_pPlayerObj->GetUID()));
		CHECK(NULL != gmgr.pPlayerDBCacheMgr->Find(m_pPlayerObj->GetUID())->GetItemCcMgr().Find(m_pItem1->m_nIUID));
	}

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_AllItemHasDBCache)
	{
		GItemDBCache* ic1 = FindItem(m_pPlayerObj, m_pItem1);
		ic1->IncStackAmt(1, false);

		GItemDBCache* ic2 = FindItem(m_pPlayerObj, m_pItem2);
		ic2->IncStackAmt(1, false);

		CHECK(gsys.pDBCacheSystem->FlushAllCache());
		CHECK(0 == gsys.pDBCacheSystem->GetItemCount(m_pPlayerObj->GetUID()));		
	}

	TEST_FIXTURE(FIXTURE_RELEASE, DBCacheSystem_FlushAllCache_CharAndAllItemHasDBCache)
	{
		GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(m_pPlayerObj->GetUID());
		cc->IncMoney(1, false);

		GItemDBCache* ic1 = FindItem(m_pPlayerObj, m_pItem1);
		ic1->IncStackAmt(1, false);

		GItemDBCache* ic2 = FindItem(m_pPlayerObj, m_pItem2);
		ic2->IncStackAmt(1, false);

		CHECK(gsys.pDBCacheSystem->FlushAllCache());

		CHECK(!cc->IsSyncing());
		CHECK(!cc->IsModified());

		CHECK(0 == gsys.pDBCacheSystem->GetItemCount(m_pPlayerObj->GetUID()));		
	}
}