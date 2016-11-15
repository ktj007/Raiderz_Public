#include "StdAfx.h"
#include "GPlayerSpawnedNPC.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "AStlUtil.h"

GPlayerSpawnedNPC::GPlayerSpawnedNPC(GEntityPlayer* pPlayer)
: m_pOwner(pPlayer)
{
}

GPlayerSpawnedNPC::~GPlayerSpawnedNPC(void)
{
	DespawnAll();
}

void GPlayerSpawnedNPC::Spawn( int nNPCID, float fDespawnTime )
{
	GEntityNPC* pAlreadySpawnedNPC = Get(nNPCID);
	if (NULL != pAlreadySpawnedNPC)
	{
		pAlreadySpawnedNPC->Warp(m_pOwner->GetPos(), m_pOwner->GetDir(), true);
	}
	else
	{
		GEntityNPC* pNewNPC = m_pOwner->GetField()->Spawn(nNPCID, m_pOwner->GetPos(), 0.0f, fDespawnTime);
		if (NULL != pNewNPC)
		{
			m_mapSpawnedNPC.insert(MAP_SPAWNEDNPC::value_type(pNewNPC->GetID(), pNewNPC->GetUID()));
		}
	}
}

GEntityNPC* GPlayerSpawnedNPC::Get( int nNPCID )
{
	MAP_SPAWNEDNPC::iterator itor = m_mapSpawnedNPC.find(nNPCID);
	if (m_mapSpawnedNPC.end() == itor) return NULL;

	MUID nNPCUID = itor->second;

	GEntityNPC* pNPC = m_pOwner->FindNPC(nNPCUID);
	if (NULL == pNPC)
	{
		m_mapSpawnedNPC.erase(itor);
		return NULL;
	}

	if (pNPC->IsDead())
	{
		m_mapSpawnedNPC.erase(itor);
		return NULL;
	}

	return pNPC;	
}

void GPlayerSpawnedNPC::DespawnAll()
{
	for each (const MAP_SPAWNEDNPC::value_type& val in m_mapSpawnedNPC)
	{
		GEntityNPC* pNPC = m_pOwner->FindNPC(val.second);
		if (NULL == pNPC) continue;

		pNPC->DespawnNow(false);
	}

	m_mapSpawnedNPC.clear();
}

bool GPlayerSpawnedNPC::IsSpawend( int nNPCID )
{
	if (NULL == Get(nNPCID)) return false;

	return true;
}