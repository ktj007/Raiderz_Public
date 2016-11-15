#include "stdafx.h"
#include "GItemDeleteLogCache.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GGlobal.h"
#include "GDBTaskSQL.h"
#include "GConfig.h"


void GItemDeleteLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_ITEM_DELETE_LOG);
	if (NULL == pTask)
		return;

	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GItemDeleteLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_ITEM_DELETE_LOG (%d\
					 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
					 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
					 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
					 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
					 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nDeltaStackAmt, m_Log[0].m_nMaxDura, m_Log[0].m_strUsagePeriod, m_Log[0].m_strEffEndDate.c_str()
					 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nDeltaStackAmt, m_Log[1].m_nMaxDura, m_Log[1].m_strUsagePeriod, m_Log[1].m_strEffEndDate.c_str()
					 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nDeltaStackAmt, m_Log[2].m_nMaxDura, m_Log[2].m_strUsagePeriod, m_Log[2].m_strEffEndDate.c_str()
					 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nDeltaStackAmt, m_Log[3].m_nMaxDura, m_Log[3].m_strUsagePeriod, m_Log[3].m_strEffEndDate.c_str()
					 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nDeltaStackAmt, m_Log[4].m_nMaxDura, m_Log[4].m_strUsagePeriod, m_Log[4].m_strEffEndDate.c_str());
}