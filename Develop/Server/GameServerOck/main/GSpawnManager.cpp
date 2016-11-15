
#include "stdafx.h"
#include "GSpawnManager.h"
#include "GFieldInfo.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GConst.h"
#include "GPathFinder.h"
#include "GNPCInfoMgr.h"
#include "GSystem.h"
#include "GGlobal.h"
#include "GSpawn.h"
#include "GSpawnDelay.h"
#include "GSTLUtil.h"
#include "GNPCMgr.h"
#include "GDespawnAuto.h"


GSpawnManager::GSpawnManager(GField* pField)
:m_pField(pField)
{
}

GSpawnManager::~GSpawnManager()
{
	Fini();
}

GSpawn* GSpawnManager::NewSpawn()
{
	return new GSpawn();
}

void GSpawnManager::Init()
{
	VALID(m_mapSpawns.empty());
	VALID(m_listSpawnDelays.empty());
	VALID(m_listDespawnAuto.empty());
	VALID(m_pField);
	VALID(m_pField->GetInfo());

	if (GConst::DEBUG_DISABLE_DEFAULT_FIELD_SPAWN)
		return; // 설정에 의해 무시

	const SPAWN_INFO_MAP& mapSpawnInfo = m_pField->GetInfo()->GetSpawnInfo();
	for (SPAWN_INFO_MAP::const_iterator it = mapSpawnInfo.begin();
		 it != mapSpawnInfo.end();
		 ++it)
	{
		const SPAWN_ID		nID = it->first;
		const SPAWN_INFO&	infoSpawn = it->second;

		AddSpawn(nID, &infoSpawn);
	}
}

void GSpawnManager::Fini()
{
	SAFE_DELETE_MAP(m_mapSpawns);
	VALID(m_mapSpawns.empty());

	SAFE_DELETE_VECTOR(m_listSpawnDelays);
	VALID(m_listSpawnDelays.empty());

	SAFE_DELETE_VECTOR(m_listDespawnAuto);
	VALID(m_listDespawnAuto.empty());

	m_vecUnspawneds.clear();
}

void GSpawnManager::AddSpawn( const SPAWN_ID nID, const SPAWN_INFO* pSpawnInfo )
{
	VALID(!Find(nID) && L"중복 등록되었습니다.");

	GSpawn* pNewSpawn = NewSpawn();
	pNewSpawn->Init(m_pField, pSpawnInfo);
	m_mapSpawns.insert(SPAWN_MAP::value_type(nID, pNewSpawn));
	m_vecUnspawneds.push_back(pNewSpawn);
}

void GSpawnManager::AddSpawnDelay(int nNPCID, const vec3& vPos, const vec3& vDir, float fDelayTime)
{
	m_listSpawnDelays.push_back(new GSpawnDelay(m_pField, nNPCID, vPos, vDir, fDelayTime));	
}

void GSpawnManager::AddSpawnDelay( SPAWN_ID nSpawnID, float fDelayTime )
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return;

	m_listSpawnDelays.push_back(new GSpawnDelay(m_pField, nSpawnID, fDelayTime));	
}

void GSpawnManager::AddDespawnAuto(MUID uidNPC, float fDespawnTime)
{
	m_listDespawnAuto.push_back(new GDespawnAuto(m_pField, uidNPC, fDespawnTime));
}

void GSpawnManager::AddDespawnAuto( SPAWN_ID nSpawnID, float fDespawnTime )
{
	m_listDespawnAuto.push_back(new GDespawnAuto(m_pField, nSpawnID, fDespawnTime));
}

GSpawn* GSpawnManager::Find( SPAWN_ID nSpawnID )
{
	SPAWN_MAP::iterator it = m_mapSpawns.find(nSpawnID);
	if (it == m_mapSpawns.end())		return NULL;
	return it->second;
}

GEntityNPC* GSpawnManager::Spawn(SPAWN_ID nSpawnID, float fSpawnDelayTime, float fDespawnTime)
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return NULL;
	GEntityNPC* pNPC = NULL;

	if (0.0f < fSpawnDelayTime)
	{
		AddSpawnDelay(nSpawnID, fSpawnDelayTime);
	}
	else
	{
		pNPC = pSpawn->Spawn();
	}

	if (0.0f < fDespawnTime)
	{
		AddDespawnAuto(nSpawnID, fSpawnDelayTime + fDespawnTime);
	}

	return pNPC;
}

GEntityNPC* GSpawnManager::Spawn(int nNPCID, const vec3& vPos, const vec3& vDir, float fSpawnDelayTime, float fDespawnTime, SpawnDetailInfo* pDetailInfo)
{
	GEntityNPC* pNPC = NULL;

	if (0.0f < fSpawnDelayTime)
	{
		AddSpawnDelay(nNPCID, vPos, vDir, fSpawnDelayTime);
	}
	else
	{
		pNPC = m_pField->SpawnNPC(nNPCID, NULL, vPos, vDir, 0, pDetailInfo);
		if (NULL == pNPC) return NULL;

		if (0.0f < fDespawnTime)
		{
			AddDespawnAuto(pNPC->GetUID(), fDespawnTime);
		}
	}	

	return pNPC;
}

GEntityNPC* GSpawnManager::Despawn(SPAWN_ID nSpawnID, bool bSpawnEnable)
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return NULL;

	if (bSpawnEnable)
	{
		m_vecUnspawneds.push_back(pSpawn);
	}

	pSpawn->Enable(bSpawnEnable);
	return pSpawn->Despawn();
}

void GSpawnManager::Reset( SPAWN_ID nSpawnID )
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return;

	return pSpawn->Reset();
}

GEntityNPC* GSpawnManager::GetNPCInstance(SPAWN_ID nSpawnID)
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return NULL;

	return pSpawn->GetNPCInstance();
}

void GSpawnManager::Enable(SPAWN_ID nSpawnID, bool bSwitch)
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return;

	vector<GSpawn*>::iterator it = std::find(m_vecUnspawneds.begin(),m_vecUnspawneds.end(), pSpawn);

	if (bSwitch)
	{
		if (it == m_vecUnspawneds.end())
		{
			m_vecUnspawneds.push_back(pSpawn);
		}
	}
	else
	{
		if (it != m_vecUnspawneds.end())
		{
			m_vecUnspawneds.erase(it);
		}
	}

	pSpawn->Enable(bSwitch);
}

bool GSpawnManager::IsEnable( SPAWN_ID nSpawnID )
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return false;

	return pSpawn->IsEnable();
}

bool GSpawnManager::IsSpawnedNPC( SPAWN_ID nSpawnID, MUID uidNPC )
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (!pSpawn)	return false;

	return pSpawn->IsSpawnedNPC(uidNPC);
}

void GSpawnManager::Update(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(6500);

	Update_Spawn(fDelta);
	Update_SpawnDelay(fDelta);
	Update_DespawnAuto(fDelta);
}

void GSpawnManager::Update_Spawn( float fDelta )
{
	//for (SPAWN_MAP::iterator it = m_mapSpawns.begin();
	//	it != m_mapSpawns.end();
	//	++it)
	//{
	//	GSpawn* pSpawn = it->second;
	//	pSpawn->Update(fDelta);
	//}

	for (vector<GSpawn*>::iterator it = m_vecUnspawneds.begin();
		it != m_vecUnspawneds.end();)
	{
		GSpawn* pSpawn = *it;
		bool bSpawned = 
			pSpawn->Update(fDelta);

		if (bSpawned)
		{
			it = m_vecUnspawneds.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void GSpawnManager::Update_SpawnDelay( float fDelta )
{
	for (SPAWNDELAY_LIST::iterator it = m_listSpawnDelays.begin();
		it != m_listSpawnDelays.end();)
	{
		GSpawnDelay* pSpawnDelay = (*it);
		if (NULL == pSpawnDelay) continue;
		pSpawnDelay->Update(fDelta);

		if (true == pSpawnDelay->IsSpawned())
		{
			it = m_listSpawnDelays.erase(it);
			SAFE_DELETE(pSpawnDelay);
		}
		else
		{
			++it;
		}
	}
}

void GSpawnManager::Update_DespawnAuto(float fDelta)
{
	uint32 nNowTime = gsys.pSystem->GetNowTime();

	for (DESPAWNAUTO_LIST::iterator it = m_listDespawnAuto.begin();
		it != m_listDespawnAuto.end();)
	{
		GDespawnAuto* pDespawnAuto = (*it);
		if (NULL == pDespawnAuto) continue;
		pDespawnAuto->Update(fDelta);

		if (true == pDespawnAuto->IsDespawned())
		{
			it = m_listDespawnAuto.erase(it);
			SAFE_DELETE(pDespawnAuto);
		}
		else
		{
			++it;
		}
	}
}

void GSpawnManager::OnTimeChangedDetail( int nTime )
{
	for (SPAWN_MAP::iterator it = m_mapSpawns.begin();
		it != m_mapSpawns.end();
		++it)
	{
		GSpawn* pSpawn = it->second;
		pSpawn->OnTimeChangedDetail( nTime );
	}
}

void GSpawnManager::OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime )
{
	for (SPAWN_MAP::iterator it = m_mapSpawns.begin();
		it != m_mapSpawns.end();
		++it)
	{
		GSpawn* pSpawn = it->second;
		pSpawn->OnTimeChanged( nOldTime, nNewTime );
	}
}

void GSpawnManager::OnWeatherChanged( GAME_WEATHER_TYPE nOldWeather, GAME_WEATHER_TYPE nNewWeather )
{
	for (SPAWN_MAP::iterator it = m_mapSpawns.begin();
		it != m_mapSpawns.end();
		++it)
	{
		GSpawn* pSpawn = it->second;
		pSpawn->OnWeatherChanged( nOldWeather, nNewWeather );
	}
}

bool GSpawnManager::IsSpawnDelayEmpty()
{
	return m_listSpawnDelays.empty();
}

void GSpawnManager::ActivateSpawnGroup(int nGroupID)
{
	for (SPAWN_MAP::iterator it = m_mapSpawns.begin(); it != m_mapSpawns.end(); it++)
	{
		GSpawn* pSpawn = it->second;
		
		if (nGroupID == pSpawn->GetGroupID())
		{
			pSpawn->ActivateUpdate();
		}
	}
}

void GSpawnManager::OnNPCDie( SPAWN_ID nID )
{
	if (nID == INVALID_ID)
		return;

	GSpawn* pSpawn = Find(nID);
	VALID(pSpawn);
	m_vecUnspawneds.push_back(pSpawn);
}

const SPAWN_INFO* GSpawnManager::GetSpawnInfo( unsigned long nSpawnID )
{
	GSpawn* pSpawn = Find(nSpawnID);
	if (NULL == pSpawn) return NULL;

	return pSpawn->GetInfo();
}

