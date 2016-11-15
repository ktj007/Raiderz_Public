#include "StdAfx.h"
#include "GSpawnDelay.h"
#include "MTime.h"
#include "GDef.h"
#include "GField.h"
#include "GSpawn.h"
#include "GSpawnManager.h"

GSpawnDelay::GSpawnDelay(GField* pField, int nNPCID, const vec3& vPos, const vec3& vDir, float fDelayTime)
: m_pField(pField)
, m_nNPCID(nNPCID)
, m_vPos(vPos)
, m_vDir(vDir)
, m_nSpawnID(0)
, m_bSpawned(false)
{
	m_pRgrSpawn = new MRegulator();
	m_pRgrSpawn->SetTickTime(fDelayTime);
}

GSpawnDelay::GSpawnDelay(GField* pField, SPAWN_ID nSpawnID, float fDelayTime)
: m_pField(pField)
, m_nNPCID(0)
, m_vPos(vec3::ZERO)
, m_vDir(vec3::ZERO)
, m_nSpawnID(nSpawnID)
, m_bSpawned(false)
{
	m_pRgrSpawn = new MRegulator();
	m_pRgrSpawn->SetTickTime(fDelayTime);
}

GSpawnDelay::~GSpawnDelay()
{
	SAFE_DELETE(m_pRgrSpawn);
}

void GSpawnDelay::Update(float fDelta)
{
	if (false == m_pRgrSpawn->IsReady(fDelta))	return;

	Spawn();
}

void GSpawnDelay::Spawn()
{
	VALID(m_pField);
	VALID(m_pField->GetInfo());

	if (true == IsSpawnBySpawnID())
	{
		m_pField->GetSpawn()->Spawn(m_nSpawnID);
	}
	else
	{
		if (!m_pField->PickingZ(m_vPos, m_vPos))
		{
			dlog("invalid once_spawn position, field_id(%d), npc_id(%d), pos(%.1f %.1f %.1f)\n",
				m_pField->GetID(),
				m_nNPCID,
				m_vPos.x, m_vPos.y, m_vPos.z);

			return;
		}
		m_pField->SpawnNPC(m_nNPCID, NULL, m_vPos, m_vDir);
	}	

	m_bSpawned = true;
}

bool GSpawnDelay::IsSpawnBySpawnID()
{
	return 0 != m_nSpawnID;
}
