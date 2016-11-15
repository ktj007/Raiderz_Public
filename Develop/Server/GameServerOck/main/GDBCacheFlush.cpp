#include "stdafx.h"
#include "GDBCacheFlush.h"
#include "GPlayerDBCache.h"
#include "GDBTaskCacheFlush.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GItem.h"
#include "GItemHolder.h"
#include "GConfig.h"


GDBCacheFlush::GDBCacheFlush( GPlayerDBCache* pPlayerCc ) 
: m_pPlayerCc(pPlayerCc), m_nCID(0), m_pTask(NULL), m_bPost(false)
{

}

GDBCacheFlush::~GDBCacheFlush()
{
	if (NULL != m_pTask)
		SAFE_DELETE(m_pTask);
}


bool GDBCacheFlush::Post()
{
	if (NULL == m_pPlayerCc)
		return true;

	if (m_pPlayerCc->IsFlushing())
		return true;

	if (!BuidDBCacheChar(m_pPlayerCc->GetCharCc()))
	{
		Rollback(m_pPlayerCc);
		return false;
	}

	if (!BuldDBCacheItem(m_pPlayerCc))
	{
		Rollback(m_pPlayerCc);
		return false;
	}

	if (!PostTask(m_pPlayerCc, m_pTask))
	{
		Rollback(m_pPlayerCc);
		return false;
	}

	return true;
}

bool GDBCacheFlush::PostTask( GPlayerDBCache* pc, GDBTaskCacheFlush* pTask )
{
	SetState(FLUSH_POST_TASK);

	if (NULL == pTask)
		return true;

	m_pTask = NULL;

	pc->BeginFlush();
	if (!GetDBManager()->Post(pTask))
	{
		pc->EndFlush();
		return false;
	}

	m_bPost = true;

	return true;
}

bool GDBCacheFlush::BuidDBCacheChar( GCharacterDBCache* cc )
{
	SetState(FLUSH_BUILD_DBCACHE_CHAR);

	if (NULL == cc)
		return true;

	if (!cc->IsModified() || cc->IsSyncing())
		return true;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(cc->GetPlayerUID());
	if (NULL == pPlayer)
	{
		cc->SetZero();
		return true;
	}

	const int64 nAID = pPlayer->GetAID();
	if (0 == nAID)
	{
		cc->SetZero();
		return true;
	}

	m_pTask = NewTask(cc->GetPlayerUID());
	if (NULL == m_pTask)
		return false;

	cc->BeginCacheSync();

	CString strCharSyncSQL;
	strCharSyncSQL.Format(L"{CALL dbo.USP_RZ_CHAR_SYNC (%d, %I64d, %I64d, %d, %d)}"
		, GConfig::m_nMyWorldID
		, pPlayer->GetAID()
		, (int64)cc->GetCID()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetPlayerInfo()->GetMoney());	

	m_pTask->SyncCharCache(cc->GetCID(), strCharSyncSQL);

	m_nCID = cc->GetCID();

	return true;
}

bool GDBCacheFlush::BuldDBCacheItem( GPlayerDBCache* pc )
{
	SetState(FLUSH_BUILD_DBCACHE_ITEM);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pc->GetUID());
	if (NULL == pPlayer)
		return true;

	CString strItemSyncSQL;
	GITEM_DBCACHE_MAP::iterator It  = pc->GetItemCcMgr().begin();
	GITEM_DBCACHE_MAP::iterator End = pc->GetItemCcMgr().end();
	for (; It != End; ++It)
	{
		GItemDBCache* ic = It->second;

		if (!ic->IsModified() || ic->IsSyncing())
			continue;

		GItem* pItem = GetItem(pPlayer, ic);
		if (NULL == pItem)
		{
			ic->SetZero();
			continue;
		}

		m_pTask = NewTask(pc->GetUID());
		if (NULL == m_pTask)
			return false;

		strItemSyncSQL.ReleaseBuffer();
		strItemSyncSQL.Format(L"{CALL dbo.USP_RZ_ITEM_SYNC (%d, %I64d, %d, %d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, (int64)pPlayer->GetCID()
			, pItem->m_nSlotType
			, pItem->m_nSlotID
			, ic->GetIUID()
			, pItem->GetAmount()
			, pItem->m_nDurability);

		m_pTask->SyncItemCache(It->second->GetIUID(), strItemSyncSQL);			
		
		It->second->BeginCacheSync();

		m_vIUID.push_back(It->second->GetIUID());
	}

	return true;
}

GItem* GDBCacheFlush::GetItem( GEntityPlayer* pPlayer, GItemDBCache* ic )
{
	GItem* pItem = pPlayer->GetItemHolder()->GetItem((SH_ITEM_SLOT_TYPE)ic->GetSlotType(), ic->GetSlotID());
	if (!IsSameItem(pItem, ic))
	{
		dlog("db cache item has incorrect slot. so try again by IUID\n");
		pItem = pPlayer->GetItemHolder()->GetItem((SH_ITEM_SLOT_TYPE)ic->GetSlotType(), ic->GetIUID());
	}				

	return pItem;
}

bool GDBCacheFlush::IsSameItem( GItem* pItem, GItemDBCache* ic )
{
	if (NULL == pItem || NULL == ic)
		return false;

	return pItem->m_nIUID == ic->GetIUID();
}

void GDBCacheFlush::Rollback( GPlayerDBCache* pc )
{
	SetState(FLUSH_ROLLBACK);

	if (0 != m_nCID)
		pc->GetCharCc()->EndCacheSync(false);
	
	for (size_t i = 0; i < m_vIUID.size(); ++i)
	{
		GItemDBCache* pi = pc->GetItemCcMgr().Find(m_vIUID[i]);
		if (NULL != pi)
			pi->EndCacheSync(false);			
	}

	m_vIUID.clear();
}

void GDBCacheFlush::SetState( const FLUST_STATE state )
{
	m_vState.push_back(state);
}

GDBCacheFlush::FLUST_STATE GDBCacheFlush::GetLastState()
{
	if (m_vState.empty()) 
		return FLUSH_NONE;

	return	m_vState.back();
}

GDBTaskCacheFlush* GDBCacheFlush::NewTask( const MUID& uidPlayer )
{
	if (NULL == m_pTask)
		m_pTask = new GDBTaskCacheFlush(uidPlayer);

	return m_pTask;
}

GDBManager* GDBCacheFlush::GetDBManager()
{
	return gsys.pDBManager;
}

bool GDBCacheFlush::HasTask()
{
	return m_bPost;
}

