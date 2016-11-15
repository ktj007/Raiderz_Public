#include "stdafx.h"
#include "GXPLogCache.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GGlobal.h"
#include "GDBTaskSQL.h"
#include "GConfig.h"


GXPLogCache::GXPLogCache() : GLogCache<GLOG_DATA_XP>(MAX_LOG_COUNT)
{
	
}

void GXPLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_CHAR_XP_LOG);
	if (NULL == pTask)
		return;

	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GXPLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_XP_LOG (%d\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
					 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurXP, m_Log[0].m_nDeltaXP, m_Log[0].m_strNpcID
					 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurXP, m_Log[1].m_nDeltaXP, m_Log[1].m_strNpcID
					 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurXP, m_Log[2].m_nDeltaXP, m_Log[2].m_strNpcID
					 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurXP, m_Log[3].m_nDeltaXP, m_Log[3].m_strNpcID
					 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurXP, m_Log[4].m_nDeltaXP, m_Log[4].m_strNpcID);
}

