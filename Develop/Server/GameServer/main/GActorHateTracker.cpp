#include "StdAfx.h"
#include "GActorHateTracker.h"
#include "GEntityNPC.h"
#include "GHateTable.h"
#include "GField.h"
#include "GGlobal.h"
#include "GNPCMgr.h"

GActorHateTracker::GActorHateTracker(GEntityActor* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GActorHateTracker::~GActorHateTracker(void)
{
	m_pOwner->DetachObserver(this);
}

void GActorHateTracker::OnAddHateTarget( GEntityActor* pEnemy )
{
	VALID(pEnemy);
	m_setHateNPCs.insert(pEnemy->GetUID());
}

void GActorHateTracker::OnLostHateTarget( MUID uidEnemy )
{
	m_setHateNPCs.erase(uidEnemy);
}

void GActorHateTracker::Clear()
{
	if (m_setHateNPCs.empty())
		return;

	MUID uidOwner = m_pOwner->GetUID();


	GVectorMUID vecEnemys;
	GetHateNPCs(vecEnemys.Vector());

	for each (const MUID& uidEnemy in vecEnemys.Vector())
	{
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidEnemy);
		if (!pNPC)  
			continue;

		pNPC->GetHateTable().DropPoint(uidOwner);
	}

	m_setHateNPCs.clear();
}

bool GActorHateTracker::IsEmpty() const
{
	return m_setHateNPCs.empty();
}

size_t GActorHateTracker::GetQty() const
{
	return m_setHateNPCs.size();
}

void GActorHateTracker::GetHateNPCs(vector<MUID>& vecNPCs) const
{
	vecNPCs.resize(m_setHateNPCs.size());
	copy(m_setHateNPCs.begin(), m_setHateNPCs.end(), vecNPCs.begin());
}

void GActorHateTracker::AddHate_RecoverHP( int nRecoveredHP, float fHateFactor )
{
	if (nRecoveredHP <= 0) 
		return;

	for each (const MUID& uidEnemy in m_setHateNPCs)
	{
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidEnemy);
		if (!pNPC)  
			continue;

		pNPC->GetHateTable().AddPoint_EnemyRecoverHP(m_pOwner, nRecoveredHP, fHateFactor);
	}
}

void GActorHateTracker::AddHate_UseTalent( int nHateAdj )
{
	for each (const MUID& uidEnemy in m_setHateNPCs)
	{
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidEnemy);
		if (!pNPC)  
			continue;

		pNPC->GetHateTable().AddPoint_EnemyApplyEffect(m_pOwner, nHateAdj);
	}
}

void GActorHateTracker::OnDie()
{
	Clear();
}

void GActorHateTracker::OnChangeField()
{
	Clear();
}

void GActorHateTracker::OnDestroy()
{
	Clear();
}

void GActorHateTracker::OnWarpSameField()
{
	Clear();
}