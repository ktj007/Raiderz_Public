#include "stdafx.h"
#include "GDBTaskCacheFlush.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GPlayerDBCache.h"
#include "GItemDBCache.h"
#include "GDBCacheSystem.h"
#include "GCharacterDBCache.h"


GDBTaskCacheFlush::GDBTaskCacheFlush( const MUID& uidPlayer )
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_CACHE_SYNC)
{

}

GDBTaskCacheFlush::~GDBTaskCacheFlush()
{

}

void GDBTaskCacheFlush::SyncCharCache( const CID nCID, CString& strSQL )
{
	m_CharSync.SetData(nCID, strSQL.GetBuffer());
}

void GDBTaskCacheFlush::SyncItemCache( const IUID nIUID, CString& strSQL )
{
	m_vItemSync.push_back(GItemSyncQuery(nIUID, strSQL.GetBuffer()));
}

void GDBTaskCacheFlush::OnExecute( mdb::MDatabase& rfDB )
{
	if (!m_CharSync.GetQuery().empty())
	{
		mdb::MDatabaseQuery q(&rfDB);
		if (ExecuteW(q, m_CharSync.GetQuery()))
			m_CharSync.SetSuccess();
		q.Close();
	}

	for (size_t i = 0; i < m_vItemSync.size(); ++i)
	{
		mdb::MDatabaseQuery q(&rfDB);
		if (ExecuteW(q, m_vItemSync[i].GetQuery()))
			m_vItemSync[i].SetSuccess();
		q.Close();
	}
}

void GDBTaskCacheFlush::_OnEndCompleted()
{
	GPlayerDBCache* pc = GetPlayerDBCache(GetReqPlayer());
	if (NULL == pc)
		return;

	pc->EndFlush();

	if (!m_CharSync.GetQuery().empty())
		EndCacheSync(pc->GetCharCc(), m_CharSync.IsSuccess());		
	
	for (size_t i = 0; i < m_vItemSync.size(); ++i)
	{
		GItemDBCache* pi = pc->GetItemCcMgr().Find(m_vItemSync[i].GetID());
		if (NULL == pi)
			continue;

		EndCacheSync(pi, m_vItemSync[i].IsSuccess());		

		if (m_vItemSync[i].IsSuccess())
			gsys.pDBCacheSystem->DeleteItem(pc->GetUID(), pi->GetIUID());		
	}	
}

void GDBTaskCacheFlush::EndCacheSync( GObjectDBCache* pCacheObj, const bool bSuccess)
{
	pCacheObj->EndCacheSync(bSuccess);
}

GPlayerDBCache* GDBTaskCacheFlush::GetPlayerDBCache(const MUID& uidPlayer)
{
	return gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
}