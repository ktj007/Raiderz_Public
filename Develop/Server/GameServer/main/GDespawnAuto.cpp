#include "StdAfx.h"
#include "GDespawnAuto.h"
#include "GField.h"
#include "GSpawnManager.h"
#include "GGlobal.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"

GDespawnAuto::GDespawnAuto(GField* pField, MUID uidNPC, float fDespawnTime)
: m_pField(pField), m_uidNPC(uidNPC), m_nSpawnID(0), m_bDespawned(false)
{
	m_pRgrDespawn = new MRegulator();
	m_pRgrDespawn->SetTickTime(fDespawnTime);
}


GDespawnAuto::GDespawnAuto(GField* pField, SPAWN_ID nSpawnID, float fDespawnTime) 
: m_pField(pField), m_uidNPC(MUID::ZERO), m_nSpawnID(nSpawnID), m_bDespawned(false)
{
	m_pRgrDespawn = new MRegulator();
	m_pRgrDespawn->SetTickTime(fDespawnTime);
}


GDespawnAuto::~GDespawnAuto(void)
{
	SAFE_DELETE(m_pRgrDespawn);
}

void GDespawnAuto::Update(float fDelta)
{
	if (false == m_pRgrDespawn->IsReady(fDelta))	return;

	Despawn();
}

void GDespawnAuto::Despawn()
{
	VALID(m_pField);
	VALID(m_pField->GetInfo());

	if (true == IsDespawnBySpawnID())
	{
		m_pField->GetSpawn()->Despawn(m_nSpawnID, false);
	}
	else
	{
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(m_uidNPC);
		if (NULL == pNPC) return;

		pNPC->DespawnNow(false);
	}	

	m_bDespawned = true;
}

bool GDespawnAuto::IsDespawnBySpawnID()
{
	return 0 != m_nSpawnID;
}
