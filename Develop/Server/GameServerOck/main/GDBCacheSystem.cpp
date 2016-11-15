#include "stdafx.h"
#include "GDBCacheSystem.h"
#include "GGlobal.h"
#include "GPlayerDBCache.h"
#include "GPlayerDBCacheMgr.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GItemHolder.h"
#include "GDBCacheFlush.h"
#include "GDBManager.h"

GDBCacheSystem::~GDBCacheSystem()
{
	Release();
}

GPlayerDBCache* GDBCacheSystem::CreatePlayerCache( const MUID& uidPlayer, const CID nCID )
{
	if (0 >= uidPlayer || 0 >= nCID)
		return NULL;

	GCharacterDBCache* pch = new GCharacterDBCache_Leaf(uidPlayer, nCID);
	if (NULL == pch)
		return NULL;

	GPlayerDBCache* pc = new GPlayerDBCache(uidPlayer, nCID, pch);
	if (NULL == pc)
	{
		SAFE_DELETE(pch);
		return NULL;
	}

	if (!gmgr.pPlayerDBCacheMgr->Insert(pc))
	{
		SAFE_DELETE(pc);
		return NULL;
	}

	return pc;
}

void GDBCacheSystem::DeletePlayerCache( const MUID& uidPlayer )
{
	GPlayerDBCache* pcc = gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
	if (NULL == pcc)
		return;

	if (pcc->GetCharCc()->IsModified() || pcc->GetCharCc()->IsSyncing())
		dlog("cache data must sync before delete character(%d)\n", pcc->GetCharCc()->GetCID());
	
	for (GITEM_DBCACHE_MAP::iterator it = pcc->GetItemCcMgr().begin(); it != pcc->GetItemCcMgr().end(); ++it)
	{
		if (it->second->IsModified() || it->second->IsSyncing())
			dlog("cache data must sync before delete item(CID:%d, IUID:%I64d)\n", pcc->GetCharCc()->GetCID(), it->second->GetIUID());

		SAFE_DELETE(it->second);
	}

	pcc->GetItemCcMgr().clear();

	gmgr.pPlayerDBCacheMgr->Erase(uidPlayer);
	SAFE_DELETE(pcc);
}

void GDBCacheSystem::Release()
{
	// 반드시 GPlayerObjectManager보다 먼저 릴리즈 되어야 한다.
	// GPlayerObjectManager가 먼저 초기화 되면 캐릭터와 아이템의 정보를 가저올 수 없기 때문에
	//   DB에 데이터를 반영할 수 없다.
	if (NULL == gmgr.pPlayerDBCacheMgr)
	{
		MLog1("ALREADY DB CACHE RELEASED.\n");
		return;
	}

	if (gmgr.pPlayerDBCacheMgr->empty())
	{
		MLog1("NO HAVE DB CACHE.\n");
		return;
	}

	MLog1("START DB CACHE RELEASE.\n");

	for (;;)
	{
		const bool bRemainDBCache = FlushAllCache();
		if (!bRemainDBCache)
		{
			ReleasePlayerDBCacheMgr();
			break;
		}

		_ASSERT(NULL != gsys.pDBManager);
		if (NULL != gsys.pDBManager)
			gsys.pDBManager->Update();			
	}
	MLog1("COMPLETED DB CACHE RELEASE.\n");
}

void GDBCacheSystem::ReleasePlayerDBCacheMgr()
{
	while(!gmgr.pPlayerDBCacheMgr->empty())
	{
		GPLAYER_DBCACHE_HMAP::iterator it = gmgr.pPlayerDBCacheMgr->begin();

		DeletePlayerCache(it->second->GetUID());
	}

	gmgr.pPlayerDBCacheMgr->clear();
}

bool GDBCacheSystem::FlushAllCache()
{
	bool bRemainCacheData = false;

	GPLAYER_DBCACHE_HMAP::iterator end  = gmgr.pPlayerDBCacheMgr->end();
	for (GPLAYER_DBCACHE_HMAP::iterator it = gmgr.pPlayerDBCacheMgr->begin(); it != end; ++it)
	{
		if (it->second->IsFlushing())
		{
			bRemainCacheData = true;
			continue;
		}

		GDBCacheFlush cf(it->second);
		if (cf.Post())
		{
			if (cf.HasTask())
				bRemainCacheData = true;
		}
		else
		{
			bRemainCacheData = true;
		}

		if (0 < it->second->GetSyncingCnt())
			bRemainCacheData = true;
	}

	return bRemainCacheData;
}

GItemDBCache* GDBCacheSystem::CreateItem( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	if (0 >= uidPlayer || 0 >= nIUID)
		return NULL;

	GPlayerDBCache* pc = FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return NULL;

	GItemDBCache* pi = new GItemDBCache_Leaf(uidPlayer, nIUID);
	if (!pc->GetItemCcMgr().Insert(pi))
	{
		SAFE_DELETE(pi);
		return NULL;
	}

	pi->SetSlot(nSlotType, nSlotID);

	return pi;
}

void GDBCacheSystem::DeleteItem( const MUID& uidPlayer, const IUID nIUID )
{
	GPlayerDBCache* pc = FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return;

	GItemDBCache* pi = pc->GetItemCcMgr().Find(nIUID);
	if (NULL == pi)
		return;

	pc->GetItemCcMgr().Erase(nIUID);

	SAFE_DELETE(pi);
}

GPlayerDBCache* GDBCacheSystem::FindPlayerCache( const MUID& uidPlayer )
{
	if (0 >= uidPlayer)
		return NULL;

	GPlayerDBCache* pc = gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
	if (NULL == pc)
	{
		GEntityPlayer* pPlayer = GetEntityPlayer(uidPlayer);
		if (NULL == pPlayer)
			return NULL;

		pc = CreatePlayerCache(uidPlayer, pPlayer->GetCID());		
	}

	return pc;
}

GCharacterDBCache* GDBCacheSystem::FindCharCache( const MUID& uidPlayer )
{
	if (0 >= uidPlayer)
		return NULL;

	GPlayerDBCache* pc = FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return NULL;

	return pc->GetCharCc();
}

GItemDBCache* GDBCacheSystem::FindItemCache( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	if (0 >= nIUID)
		return NULL;

	GPlayerDBCache* pc = FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return NULL;

	GItemDBCache* pi = pc->GetItemCcMgr().Find(nIUID);
	if (NULL == pi)
		pi = CreateItem(uidPlayer, nSlotType, nSlotID,  nIUID);
	
	return pi;
}

GEntityPlayer* GDBCacheSystem::GetEntityPlayer( const MUID& uidPlayer )
{
	return gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
}

size_t GDBCacheSystem::GetItemCount( const MUID& uidPlayer )
{
	GPlayerDBCache* pc = FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return 0;

	return pc->GetItemCcMgr().size();
}

void GDBCacheSystem::SetItemSlot( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = FindItemCache(uidPlayer, nSlotType, nSlotID, nIUID);
	if (NULL == ic)
		return;

	ic->SetSlot(nSlotType, nSlotID);
}

int16 GDBCacheSystem::GetItemCacheModAmt( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = FindItemCache(uidPlayer, nSlotType, nSlotID, nIUID);
	if (NULL == ic)
		return 0;

	return ic->GetModAmt();
}

int GDBCacheSystem::GetCharCacheModMoney( const MUID& uidPlayer )
{
	if (0 >= uidPlayer)
		return 0;

	GPlayerDBCache* pc = FindPlayerCache(uidPlayer);
	if (NULL == pc)
		return NULL;

	GCharacterDBCache* cc = pc->GetCharCc();
	if (NULL == cc)
		return NULL;

	return cc->GetModMoney();
}
