#include "stdafx.h"
#include "GQuestItemLogCache.h"
#include "GConfig.h"
#include "GDBTaskSQL.h"
#include "GDBManager.h"
#include "GGlobal.h"


void GQuestItemLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QPVP_REWARD_ITEM_LOG);
	if (NULL == pTask)
		return;

	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GQuestItemLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_QUEST_ITEM_LOG (%d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %d, %d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %d, %d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %d, %d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %d, %d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %d, %d)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_strCurMoney, m_Log[0].m_strDeltaMoney, m_Log[0].m_strIUID, m_Log[0].m_strItemID, m_Log[0].m_strCurStackAmt, m_Log[0].m_strDeltaStackAmt, m_Log[0].m_strRefMailUID, m_Log[0].m_nQuestID, m_Log[0].m_nEventID
					 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_strCurMoney, m_Log[1].m_strDeltaMoney, m_Log[1].m_strIUID, m_Log[1].m_strItemID, m_Log[1].m_strCurStackAmt, m_Log[1].m_strDeltaStackAmt, m_Log[1].m_strRefMailUID, m_Log[1].m_nQuestID, m_Log[1].m_nEventID
					 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_strCurMoney, m_Log[2].m_strDeltaMoney, m_Log[2].m_strIUID, m_Log[2].m_strItemID, m_Log[2].m_strCurStackAmt, m_Log[2].m_strDeltaStackAmt, m_Log[2].m_strRefMailUID, m_Log[2].m_nQuestID, m_Log[2].m_nEventID
					 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_strCurMoney, m_Log[3].m_strDeltaMoney, m_Log[3].m_strIUID, m_Log[3].m_strItemID, m_Log[3].m_strCurStackAmt, m_Log[3].m_strDeltaStackAmt, m_Log[3].m_strRefMailUID, m_Log[3].m_nQuestID, m_Log[3].m_nEventID
					 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_strCurMoney, m_Log[4].m_strDeltaMoney, m_Log[4].m_strIUID, m_Log[4].m_strItemID, m_Log[4].m_strCurStackAmt, m_Log[4].m_strDeltaStackAmt, m_Log[4].m_strRefMailUID, m_Log[4].m_nQuestID, m_Log[4].m_nEventID);
}

