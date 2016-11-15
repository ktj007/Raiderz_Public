#include "StdAfx.h"
#include "GPlayerEmblem.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GEmblemInfoMgr.h"
#include "GDBManager.h"
#include "CSEmblemInfo.h"
#include "GDBTaskDataEmblem.h"

GPlayerEmblem::GPlayerEmblem(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GPlayerEmblem::~GPlayerEmblem(void)
{
	m_pOwner->DetachObserver(this);
}

bool GPlayerEmblem::CheckEmblem( int nEmblemID ) const
{
	return (m_setEmblems.find(nEmblemID) != m_setEmblems.end());
}

bool GPlayerEmblem::HasKilled( int nNPCID ) const
{
	return (m_setKilleds.find(nNPCID) != m_setKilleds.end());
}

void GPlayerEmblem::OnRewarded(int nNPCID)
{
	if (HasKilled(nNPCID))
		return;

	vector<const CSEmblemInfo*> vecEmblemInfos = gmgr.pEmblemInfoMgr->GetKillEmblem(nNPCID);
	for each (const CSEmblemInfo* each in vecEmblemInfos)
	{
		CompleteEmblem(each->nID);
	}
	m_setKilleds.insert(nNPCID);
}

void GPlayerEmblem::CompleteEmblem(int nEmblemID)
{
	if (m_setEmblems.end() != m_setEmblems.find(nEmblemID))
		return;

	m_setEmblems.insert(nEmblemID);

	GDBT_EMBLEM data(m_pOwner->GetAID(), m_pOwner->GetUID(), m_pOwner->GetCID(), nEmblemID);

	gsys.pDBManager->EmblemInsert(data);
}

void GPlayerEmblem::LoadFromDB( const deque<int>& qEmblems )
{
	for each (int each in qEmblems)
	{
		m_setEmblems.insert(each);
		
		int nKillNPCID = gmgr.pEmblemInfoMgr->GetKillNPCIDByEmblem(each);
		if (nKillNPCID == INVALID_NPC_ID)
			continue;

		m_setKilleds.insert(nKillNPCID);
	}
}
