#include "stdafx.h"
#include "GNpcShopLogCache.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GGlobal.h"
#include "GDBTaskSQL.h"
#include "GConfig.h"

void GNpcShopLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_ITEM_SELL_LOG);
	if (NULL == pTask)
		return;

	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GNpcShopLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_NPC_SHOP_LOG (%d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_strCurMoney, m_Log[0].m_strDeltaMoney, m_Log[0].m_strIUID, m_Log[0].m_strItemID, m_Log[0].m_strCurStackAmt, m_Log[0].m_strDeltaStackAmt
					 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_strCurMoney, m_Log[1].m_strDeltaMoney, m_Log[1].m_strIUID, m_Log[1].m_strItemID, m_Log[1].m_strCurStackAmt, m_Log[1].m_strDeltaStackAmt
					 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_strCurMoney, m_Log[2].m_strDeltaMoney, m_Log[2].m_strIUID, m_Log[2].m_strItemID, m_Log[2].m_strCurStackAmt, m_Log[2].m_strDeltaStackAmt
					 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_strCurMoney, m_Log[3].m_strDeltaMoney, m_Log[3].m_strIUID, m_Log[3].m_strItemID, m_Log[3].m_strCurStackAmt, m_Log[3].m_strDeltaStackAmt
					 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_strCurMoney, m_Log[4].m_strDeltaMoney, m_Log[4].m_strIUID, m_Log[4].m_strItemID, m_Log[4].m_strCurStackAmt, m_Log[4].m_strDeltaStackAmt);
}

