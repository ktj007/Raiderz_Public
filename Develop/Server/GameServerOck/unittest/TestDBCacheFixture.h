#ifndef _TEST_DBCACHE_MOCK_DB_MGR_H
#define _TEST_DBCACHE_MOCK_DB_MGR_H

#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GDBCacheSystem.h"
#include "GTestWrapper.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GDBCacheSystem.h"
#include "SAsyncDB.h"
#include "SDBTask.h"
#include "GDBTaskCharData.h"
#include "GUTHelper_Item.h"
#include "MockField.h"
#include "GTestForward.h"

namespace test_dbcache_fixture
{
	class MockAsycnDB : public SAsyncDB
	{
	public :
		MockAsycnDB() : SAsyncDB() {}

		virtual bool ExecuteAsync(SDBAsyncTask* pTask) override
		{
			pTask->SetTaskSuccess();
			pTask->OnCompleted();
			SAFE_DELETE(pTask);
			return true;
		}
	};

	class MockDBManager : public GDBManager
	{
	public :
		MockDBManager()
		{
			m_pGameDB = new MockAsycnDB;
		}

		~MockDBManager() 
		{
			SAFE_DELETE(m_pGameDB);
		}
	};

	class FIXTURE
	{
	public :
		virtual ~FIXTURE()
		{
			gsys.pDBCacheSystem->ReleasePlayerDBCacheMgr();
			gsys.pDBCacheSystem->Release();
			gmgr.pPlayerObjectManager->DeleteAllPlayer();
		}

		virtual GEntityPlayer* CreateTestPlayer(const CID nCID = 1, const int nXP = 1, const int nMoney = 1000)
		{
			static MUID uidPlayer = 1;

			GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer++);
			pPlayerObj->GetEntity()->GetPlayerInfo()->nCID = nCID;
			pPlayerObj->GetEntity()->GetPlayerInfo()->nXP = nXP;
			pPlayerObj->GetEntity()->GetPlayerInfo()->nLevel = 1;
			pPlayerObj->GetEntity()->GetPlayerInfo()->SetMoney(nMoney);
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
			gsys.pDBCacheSystem->CreatePlayerCache(pPlayerObj->GetUID(), nCID);
			return pPlayerObj->GetEntity();
		}

		virtual GItem* GiveTestItem(GEntityPlayer* pPlayer, const int nItemID = 1, const int16 nAmt = 10, const uint8 nDura = 10, const uint8 nMaxDura = 100)
		{
			GItemData*	pItemData	= GUTHelper_Item::NewItemData(nItemID);
			GItem*		pItem		= GUTHelper_Item::GiveNewItem(pPlayer, nItemID, nAmt);

			pItemData->m_nMaxDurability = nMaxDura;
			pItem->m_nDurability		= nDura;

			gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			
			return pItem;
		}

		GItemDBCache* SetItemCache(const MUID& uidPlayer, GItemDBCache_Leaf* pNew)
		{
			GPlayerDBCache* pc = gsys.pDBCacheSystem->FindPlayerCache(uidPlayer);
			GItemDBCache* pi = pc->GetItemCcMgr().Find(pNew->GetIUID());
			if (NULL != pi)
			{
				pc->GetItemCcMgr().Erase(pNew->GetIUID());
				SAFE_DELETE(pi);
			}
			pc->GetItemCcMgr().Insert(pNew);

			return pNew;
		}

		virtual void SetTestCharCc(GCharacterDBCache* pch)
		{
			pch->IncXP(1, false);
			pch->DecXP(1, false);
			pch->IncMoney(1, false);
			pch->DecMoney(1, false);
		}

		virtual void SetTestItemCc(GItemDBCache* pi)
		{
			pi->IncStackAmt(1, false);
			pi->DecStackAmt(1, false);			
		}

		virtual void CHECK_CHAR_CACHE(GCharacterDBCache* pch, const int nEachRange, const size_t nEachModCnt, const bool bSyncing)
		{
			CHECK_EQUAL(nEachRange, (int)pch->GetXP().GetIncrease());
			CHECK_EQUAL(nEachRange, (int)pch->GetXP().GetDecrease());
			CHECK_EQUAL(nEachRange, (int)pch->GetMoney().GetIncrease());
			CHECK_EQUAL(nEachRange, (int)pch->GetMoney().GetDecrease());

			CHECK_EQUAL(nEachModCnt, pch->GetXP().GetModifiedCnt());
			CHECK_EQUAL(nEachModCnt, pch->GetMoney().GetModifiedCnt());

			CHECK_EQUAL(bSyncing, pch->IsSyncing());
		}

		virtual void CHECK_CHAR_BEGIN_CACHE(GCharacterDBCache* pch, const bool bSyncing)
		{
			CHECK_CHAR_CACHE(pch, 1, 2, bSyncing);
		}

		virtual void CHECK_CHAR_END_CACHE(GCharacterDBCache* pch)
		{
			CHECK_CHAR_CACHE(pch, 0, 0, false);
		}

		virtual void CHECK_ITEM_CACHE(GItemDBCache* pi, const int nEacnRange, const size_t nEachModCnat, const bool bSyncing)
		{
			CHECK_EQUAL(nEacnRange, pi->GetStackAmt().GetIncrease());
			CHECK_EQUAL(nEacnRange, pi->GetStackAmt().GetDecrease());
			CHECK_EQUAL(0, pi->GetDura().GetIncrease());
			CHECK_EQUAL(0, pi->GetDura().GetDecrease());

			CHECK_EQUAL(nEachModCnat, pi->GetStackAmt().GetModifiedCnt());
			CHECK_EQUAL(0, pi->GetDura().GetModifiedCnt());

			CHECK_EQUAL(bSyncing, pi->IsSyncing());
		}

		virtual void CHECK_ITEM_BEGIN_CACHE(GItemDBCache* pi, const bool bSyncing)
		{
			CHECK_ITEM_CACHE(pi, 1, 2, bSyncing);
		}

		virtual void CHECK_ITEM_END_CACHE(GItemDBCache* pi)
		{
			CHECK_ITEM_CACHE(pi, 0, 0, false);
		}		
		
	public :
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;
		GTestMgrWrapper<GPlayerDBCacheMgr>		m_PlayerDBCacheMgrWrapper;
		GTestSysWrapper<GDBCacheSystem>			m_DBCacheSystemWrapper;
	};

}


#endif