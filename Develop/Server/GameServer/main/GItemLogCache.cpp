#include "stdafx.h"
#include "GItemLogCache.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GGlobal.h"
#include "GDBTaskSQL.h"
#include "GConfig.h"



GItemLogCache::GItemLogCache() : GLogCache<GLOG_DATA_ITEM>(MAX_LOG_COUNT)
{

}


void GItemLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_ITEM_LOOT_LOG);
	if (NULL == pTask)
		return;
	
	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GItemLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_ITEM_LOG (%d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nCurStackAmt, m_Log[0].m_nDeltaStackAmt, m_Log[0].m_strNpcID
					 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nCurStackAmt, m_Log[1].m_nDeltaStackAmt, m_Log[1].m_strNpcID
					 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nCurStackAmt, m_Log[2].m_nDeltaStackAmt, m_Log[2].m_strNpcID
					 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nCurStackAmt, m_Log[3].m_nDeltaStackAmt, m_Log[3].m_strNpcID
					 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nCurStackAmt, m_Log[4].m_nDeltaStackAmt, m_Log[4].m_strNpcID);
}