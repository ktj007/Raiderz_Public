#include "stdafx.h"
#include "GDBTaskNPCKillLogBuilder.h"
#include "GTimeType.h"
#include "GTimeCalculator.h"
#include "GDBTaskSQL.h"
#include "GGlobal.h"
#include "GDBManager.h"

GDBTaskNPCKillLogBuilder::GDBTaskNPCKillLogBuilder()
{
	Reset();	
}

GDBTaskNPCKillLogBuilder::~GDBTaskNPCKillLogBuilder()
{

}


void GDBTaskNPCKillLogBuilder::SetPlayer( const size_t nIdx, const GNPC_KILLER& killer )
{
	if (GNPC_KILLER::MAX_COUNT <= nIdx)
		return;

	m_Player[nIdx] = killer;
}

void GDBTaskNPCKillLogBuilder::Reset()
{
	for (size_t i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
		m_Player[i].Reset();	
}

void GDBTaskNPCKillLogBuilder::Post( int nWorldID, const wstring& strRegDate, int nNpcID )
{
	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_KILL_NPC_LOG (%d, %s, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d)}"
				   , nWorldID, strRegDate.c_str(), nNpcID
				   , m_Player[0].m_nAID, m_Player[0].m_nCID, m_Player[0].m_nCharPtm, m_Player[0].m_nDeltaCharPtm, m_Player[0].m_nLevel
				   , m_Player[1].m_nAID, m_Player[1].m_nCID, m_Player[1].m_nCharPtm, m_Player[1].m_nDeltaCharPtm, m_Player[1].m_nLevel
				   , m_Player[2].m_nAID, m_Player[2].m_nCID, m_Player[2].m_nCharPtm, m_Player[2].m_nDeltaCharPtm, m_Player[2].m_nLevel
				   , m_Player[3].m_nAID, m_Player[3].m_nCID, m_Player[3].m_nCharPtm, m_Player[3].m_nDeltaCharPtm, m_Player[3].m_nLevel
				   , m_Player[4].m_nAID, m_Player[4].m_nCID, m_Player[4].m_nCharPtm, m_Player[4].m_nDeltaCharPtm, m_Player[4].m_nLevel);				   

	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_NPC_KILLED_LOG);
	if (NULL == pTask)
		return;

	pTask->PushSQL(strSQL);

	PostTask(pTask);
}

void GDBTaskNPCKillLogBuilder::PostTask( GLogDBTaskQuery* pTask )
{
	gsys.pDBManager->Post(pTask);
}

void GDBTaskNPCKillLogBuilder::PostNpcKillLog( const int nWorldID, const wstring& strRegDate, const int nNpcID, const vector<GNPC_KILLER>& vKiller )
{
	size_t nIdx = 0;
	for (size_t i = 0; i < vKiller.size(); ++i)
	{
		SetPlayer(nIdx, vKiller[i]);

		++nIdx;

		if (nIdx >= GNPC_KILLER::MAX_COUNT)
		{
			nIdx = 0;
			Post(nWorldID, strRegDate, nNpcID);
			Reset();
		}		
	}

	if (0 < nIdx)
		Post(nWorldID, strRegDate, nNpcID);	
}

