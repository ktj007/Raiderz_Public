#include "stdafx.h"
#include "GPartyLogCache.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GGlobal.h"
#include "GDBTaskSQL.h"
#include "GConfig.h"

void GPartyLogCache::OnPostLog()
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_PARTY_LOG);
	if (NULL == pTask)
		return;

	PostString();

	pTask->PushSQL(m_strSQL);

	gsys.pDBManager->Post(pTask);
}

void GPartyLogCache::PostString()
{
	m_strSQL.Format(L"{CALL dbo.USP_RZ_PARTY_LOG (%d\
					 , %I64d, %d, %s, %d, %I64d, %I64d\
					 , %I64d, %d, %s, %d, %I64d, %I64d\
					 , %I64d, %d, %s, %d, %I64d, %I64d\
					 , %I64d, %d, %s, %d, %I64d, %I64d\
					 , %I64d, %d, %s, %d, %I64d, %I64d)}"
					 , GConfig::m_nMyWorldID
					 , m_Log[0].m_uidParty, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nType, m_Log[0].m_nAID, m_Log[0].m_nCID
					 , m_Log[1].m_uidParty, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nType, m_Log[1].m_nAID, m_Log[1].m_nCID
					 , m_Log[2].m_uidParty, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nType, m_Log[2].m_nAID, m_Log[2].m_nCID
					 , m_Log[3].m_uidParty, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nType, m_Log[3].m_nAID, m_Log[3].m_nCID
					 , m_Log[4].m_uidParty, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nType, m_Log[4].m_nAID, m_Log[4].m_nCID);
}

