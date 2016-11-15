#include "stdafx.h"
#include "GItemMergeLogCache.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GGlobal.h"
#include "GDBTaskSQL.h"
#include "GConfig.h"


void GItemMergeAndSplitLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_ITEM_MERGE_AND_SPLIT_LOG);
	if (NULL == pTask)
		return;

	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GItemMergeAndSplitLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_ITEM_MERGE_AND_SPLIT_LOG (%d\
					 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
					 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
					 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
					 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
					 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_nAID, m_Log[0].m_nOwnerID, m_Log[0].m_nCode, m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nCurStackAmt, m_Log[0].m_nDeltaStackAmt, m_Log[0].m_nRefAID, m_Log[0].m_nRefOwnerID, m_Log[0].m_nRefIUID, m_Log[0].m_nRefCurStackAmt
					 , m_Log[1].m_nAID, m_Log[1].m_nOwnerID, m_Log[0].m_nCode, m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nCurStackAmt, m_Log[1].m_nDeltaStackAmt, m_Log[1].m_nRefAID, m_Log[1].m_nRefOwnerID, m_Log[1].m_nRefIUID, m_Log[1].m_nRefCurStackAmt
					 , m_Log[2].m_nAID, m_Log[2].m_nOwnerID, m_Log[0].m_nCode, m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nCurStackAmt, m_Log[2].m_nDeltaStackAmt, m_Log[2].m_nRefAID, m_Log[2].m_nRefOwnerID, m_Log[2].m_nRefIUID, m_Log[2].m_nRefCurStackAmt
					 , m_Log[3].m_nAID, m_Log[3].m_nOwnerID, m_Log[0].m_nCode, m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nCurStackAmt, m_Log[3].m_nDeltaStackAmt, m_Log[3].m_nRefAID, m_Log[3].m_nRefOwnerID, m_Log[3].m_nRefIUID, m_Log[3].m_nRefCurStackAmt
					 , m_Log[4].m_nAID, m_Log[4].m_nOwnerID, m_Log[0].m_nCode, m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nCurStackAmt, m_Log[4].m_nDeltaStackAmt, m_Log[4].m_nRefAID, m_Log[4].m_nRefOwnerID, m_Log[4].m_nRefIUID, m_Log[4].m_nRefCurStackAmt);
}