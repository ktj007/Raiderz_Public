#include "stdafx.h"
#include "GEntityMgr.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GBuffEntity.h"
#include "GPlayerField.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GNPCMgr.h"

void GGatherEntityCounter::_Inc( GATHER_TYPE nGatherType )
{
	m_nCount[nGatherType]++;
}

void GGatherEntityCounter::_Dec( GATHER_TYPE nGatherType )
{
	m_nCount[nGatherType]--;
	if (m_nCount[nGatherType] < 0) 
	{
		m_nCount[nGatherType] = 0;
	}
}

GGatherEntityCounter::GGatherEntityCounter()
{
	for (int i = 0; i < MAX_GATHER_TYPE; i++)
	{
		m_nCount[i] = 0;
	}
}

void GGatherEntityCounter::Inc( GNPCInfo* pNPCInfo )
{
	if (pNPCInfo->nNpcType == NPC_TYPE_GATHER)
	{
		_Inc(pNPCInfo->nGatherType);
	}
}

void GGatherEntityCounter::Dec( GNPCInfo* pNPCInfo )
{
	if (pNPCInfo->nNpcType == NPC_TYPE_GATHER)
	{
		_Dec(pNPCInfo->nGatherType);
	}
}

//////////////////////////////////////////////////////////////////////////
GEntityMgr::GEntityMgr()
{
	for (int i = 0; i < ETID_MAX; i++)
	{
		m_nEntityQty[i] = 0;
	}
}

GEntityMgr::~GEntityMgr()
{
	Destroy();
}

void GEntityMgr::Create()
{

}

void GEntityMgr::Destroy()
{
	Clear();
}

void GEntityMgr::AddEntity(GEntity* pEntity)
{
	VALID(pEntity);

	int n = pEntity->GetTypeID();
	m_nEntityQty[n]++;

	AddEntity_UID(pEntity);

	if (pEntity->IsNPC())
	{
		GEntityNPC* pNPC = static_cast<GEntityNPC*>(pEntity);
		AddNPC_ID(pNPC);
		AddNPC_StaticID(pNPC);
		AddNPC_UIID(pNPC);
		AddNPC_GatherEntityCount(pNPC);
	}


	if (pEntity->IsPlayer())
	{
		m_vecPlayers.push_back(pEntity);
	}
	else
	{
		m_vecEntitiesExceptPlayer.push_back(pEntity);
	}
}

void GEntityMgr::AddEntity_UID(GEntity* pEntity)
{
	VALID(pEntity);
	VALID(m_EntityUIDMap.find(pEntity->GetUID().Value) == m_EntityUIDMap.end());

	m_EntityUIDMap.insert(ENTITY_UID_MAP::value_type(pEntity->GetUID().Value, pEntity));
}

void GEntityMgr::AddNPC_ID(GEntityNPC* pNPC)
{
	VALID(pNPC);

	NPC_ID_MAP::iterator itor = m_NPCIDMap.find(pNPC->GetID());
	if (m_NPCIDMap.end() == itor)
	{
		NPC_UID_MAP NPCUIDMap;
		NPCUIDMap.insert(NPC_UID_MAP::value_type(pNPC->GetUID().Value, pNPC));

		m_NPCIDMap.insert(NPC_ID_MAP::value_type(pNPC->GetID(), NPCUIDMap));
	}
	else
	{
		NPC_UID_MAP& NPCUIDMap = (*itor).second;

		NPCUIDMap.insert(NPC_UID_MAP::value_type(pNPC->GetUID().Value, pNPC));
	}
}

void GEntityMgr::AddNPC_StaticID(GEntityNPC* pNPC)
{		
	VALID(pNPC);

	if (pNPC->GetStaticID() > 0)
	{
		m_StaticEntityMap.insert(map<int, MUID>::value_type(pNPC->GetStaticID(), pNPC->GetUID()));
	}
}


void GEntityMgr::RemoveEntity(GEntity* pEntity, bool bDelete)
{
	VALID(pEntity);

	ENTITY_UID_MAP::iterator itor = m_EntityUIDMap.find(pEntity->GetUID().Value);
	if (itor == m_EntityUIDMap.end()) 
		return;

	RemoveEntity_UID(pEntity);

	if (pEntity->IsNPC())
	{
		GEntityNPC* pNPC = static_cast<GEntityNPC*>(pEntity);
		RemoveNPC_ID(pNPC);
		RemoveNPC_StaticID(pNPC);			
		RemoveNPC_UIID(pNPC);
		RemoveNPC_GatherEntityCount(pNPC);
	}	

	if (bDelete)
	{
		pEntity->Drop();
	}

	if (pEntity->IsPlayer())
	{
		vector<GEntity*>::iterator it = std::find(m_vecPlayers.begin(), m_vecPlayers.end(), pEntity);
		if (it != m_vecPlayers.end())
		{
			m_vecPlayers.erase(it);
		}
	}
	else
	{
		vector<GEntity*>::iterator it = std::find(m_vecEntitiesExceptPlayer.begin(), m_vecEntitiesExceptPlayer.end(), pEntity);
		if (it != m_vecEntitiesExceptPlayer.end())
		{
			m_vecEntitiesExceptPlayer.erase(it);
		}
	}
	
}

void GEntityMgr::RemoveEntity_UID(GEntity* pEntity)
{
	m_EntityUIDMap.erase(pEntity->GetUID().Value);
	int n = pEntity->GetTypeID();	
	if (0 < m_nEntityQty[n])
	{
		m_nEntityQty[n]--;
	}
}

void GEntityMgr::RemoveNPC_ID(GEntityNPC* pNPC)
{
	NPC_ID_MAP::iterator itorID = m_NPCIDMap.find(pNPC->GetID());
	if (m_NPCIDMap.end() == itorID) return;

	NPC_UID_MAP& NPCUIDMap = (*itorID).second;

	NPCUIDMap.erase(pNPC->GetUID().Value);
}

void GEntityMgr::RemoveNPC_StaticID(GEntityNPC* pNPC)
{
	if (pNPC->GetStaticID() != 0)
	{
		m_StaticEntityMap.erase(pNPC->GetStaticID());
	}
}


GEntity* GEntityMgr::FindEntity(const MUID& uidEntity)
{
	ENTITY_UID_MAP::iterator itor = m_EntityUIDMap.find(uidEntity.Value);
	if (itor != m_EntityUIDMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

GEntity* GEntityMgr::FindEntityFromStaticID(int nStaticID)
{
	map<int, MUID>::iterator itorStaticEntity = m_StaticEntityMap.find(nStaticID);
	if (itorStaticEntity != m_StaticEntityMap.end())
	{
		MUID uidEntity = (*itorStaticEntity).second;
		return FindEntity(uidEntity);
	}

	return NULL;
}

GEntityPlayer* GEntityMgr::FindPlayer(const MUID& uidPlayer)
{
	// 별로 안좋은 방식. 최적화할때 바꾸도록 함. - birdkr(2007-02-20)
	GEntity* pRet = FindEntity(uidPlayer);
	if (pRet && pRet->GetTypeID() == ETID_PLAYER) return static_cast<GEntityPlayer*>(pRet);
	return NULL;
}

GEntityNPC* GEntityMgr::FindNPC(const MUID& uidNPC)
{
	// 별로 안좋은 방식. 최적화할때 바꾸도록 함. - birdkr(2007-02-20)
	GEntity* pRet = FindEntity(uidNPC);
	if (pRet && pRet->GetTypeID() == ETID_NPC) return static_cast<GEntityNPC*>(pRet);
	return NULL;
}

GEntityNPC* GEntityMgr::FindNPC( const UIID nNPCUIID )
{
	MUID uidNPC = m_IndexedNPCUIDFinder.Find(nNPCUIID);
	if (uidNPC != MUID::ZERO)
	{
		return FindNPC(uidNPC);
	}
	return NULL;
}

vector<GEntityNPC*> GEntityMgr::FindNPCByID(int nNPCID)
{	
	NPC_ID_MAP::iterator itor = m_NPCIDMap.find(nNPCID);
	if (m_NPCIDMap.end() == itor) return vector<GEntityNPC*>();	

	vector<GEntityNPC*> vecNPC;

	NPC_UID_MAP NPCUIDMap = (*itor).second;
	for each (const NPC_UID_MAP::value_type& val in NPCUIDMap)
	{
		GEntityNPC* pNPC = val.second;
		vecNPC.push_back(pNPC);
	}

	return vecNPC;
}

GEntityNPC* GEntityMgr::FindOneNPCByID( int nNPCID )
{
	NPC_ID_MAP::iterator itor = m_NPCIDMap.find(nNPCID);
	if (m_NPCIDMap.end() == itor) return NULL;	

	NPC_UID_MAP NPCUIDMap = (*itor).second;
	for each (const NPC_UID_MAP::value_type& val in NPCUIDMap)
	{
		GEntityNPC* pNPC = val.second;
		return pNPC;
	}

	return NULL;
}

GEntityActor* GEntityMgr::FindActor(const MUID& uidActor)
{
	// 별로 안좋은 방식. 최적화할때 바꾸도록 함. - birdkr(2007-02-20)
	GEntity* pRet = FindEntity(uidActor);
	if (pRet && pRet->IsType(ET_ACTOR)) return static_cast<GEntityActor*>(pRet);
	return NULL;
}

GBuffEntity* GEntityMgr::FindBuffEntity( const MUID& uidMagicArea )
{
	// 별로 안좋은 방식. 최적화할때 바꾸도록 함. - birdkr(2007-02-20)
	GEntity* pRet = FindEntity(uidMagicArea);
	if (pRet && pRet->IsType(ET_BUFF_ENTITY)) return static_cast<GBuffEntity*>(pRet);
	return NULL;
}

void GEntityMgr::GetNeighborNPC(const vec3& vPos, const float fRange, vector<GEntityNPC*>& vecOutNeighborNPC )
{
	for each (const pair<uint64, GEntity*>& pairEntity in m_EntityUIDMap)
	{
		if (ETID_NPC == pairEntity.second->GetTypeID())
		{
			GEntityNPC* pEntityNPC = static_cast<GEntityNPC*>(pairEntity.second);

			if (fRange >= vPos.DistanceTo(pEntityNPC->GetPos()))
			{
				vecOutNeighborNPC.push_back(pEntityNPC);
			}			
		}
	}
}

void GEntityMgr::Update(float fDelta)
{
	PFC_THISFUNC;

	m_vecDeleteEntity.clear();

	UpdateEntity_Players(fDelta);
	UpdateEntity_ExceptPlayers(fDelta);

	for each (GEntity* pDeleteEntity in m_vecDeleteEntity)
	{
		RemoveEntity(pDeleteEntity, false);
		DestoryEntity(pDeleteEntity);
	}
}

bool GEntityMgr::UpdateEntityImpl(GEntity* pEntity, float fDelta) 
{
	pEntity->UpdateMovable(fDelta);
	pEntity->UpdateCombat(fDelta);
	pEntity->UpdateBuff(fDelta);
	pEntity->Update(fDelta);

	if (pEntity->IsDeleteMe())
	{
		m_vecDeleteEntity.push_back(pEntity);
		return false;
	}

	return true;
}

void GEntityMgr::UpdateEntity_ExceptPlayers(float fDelta) 
{
	for (size_t i=0; i<m_vecEntitiesExceptPlayer.size(); ++i)
	{
		GEntity* pEntity = m_vecEntitiesExceptPlayer[i];

		if (!pEntity ||
			pEntity->IsDeleteMe())
		{
			m_vecDeleteEntity.push_back(pEntity);
			continue;
		}

		if (!pEntity->IsActivateUpdate())
			continue;

		if (!UpdateEntityImpl(pEntity, fDelta))
			continue;
	}
}

void GEntityMgr::UpdateEntity_Players(float fDelta) 
{
	for (size_t i=0; i<m_vecPlayers.size(); ++i)
	{
		GEntityPlayer* pPlayer = ToEntityPlayer(m_vecPlayers[i]);

		if (!pPlayer ||
			pPlayer->IsDeleteMe())
		{
			m_vecDeleteEntity.push_back(pPlayer);
			continue;
		}

		if (!UpdateEntityImpl(pPlayer, fDelta))
			continue;

		pPlayer->GetPlayerField().Update(fDelta);
	}
}

int GEntityMgr::GetEntityQty(ENTITY_TYPEID nType) 
{
	return m_nEntityQty[nType];
}

void GEntityMgr::Clear(bool bExceptPlayers)
{
	ENTITY_UID_MAP::iterator iter = m_EntityUIDMap.begin();
	for (; iter!=m_EntityUIDMap.end();)
	{
		GEntity* pEntity = iter->second;

		if (bExceptPlayers &&
			pEntity->IsPlayer())
		{
			++iter;
			continue;
		}

		iter = m_EntityUIDMap.erase(iter);
		DestoryEntity(pEntity);
	}

	m_StaticEntityMap.clear();
	m_NPCIDMap.clear();
		
	for(int i = 0; i < ETID_MAX; i++)
	{
		if (bExceptPlayers && i == ETID_PLAYER)	continue;
		m_nEntityQty[i] = 0;
	}

	m_IndexedNPCUIDFinder.Clear();

	m_vecEntitiesExceptPlayer.clear();

	if (!bExceptPlayers)
	{
		m_vecPlayers.clear();
	}
}

vector<GEntityPlayer*> GEntityMgr::GetPlayers()
{
	vector<GEntityPlayer*> vecRet;
	for each (const ENTITY_UID_MAP::value_type& data in m_EntityUIDMap)
	{
		if (data.second->IsPlayer())
		{
			GEntityPlayer* pPlayer = static_cast<GEntityPlayer*>(data.second);
			vecRet.push_back(pPlayer);
		}
	}
	return vecRet;
}

vector<GEntityNPC*> GEntityMgr::GetNPCs()
{
	vector<GEntityNPC*> vecRet;
	for each (const ENTITY_UID_MAP::value_type& data in m_EntityUIDMap)
	{
		if (data.second->IsNPC())
		{
			GEntityNPC* pNPC = static_cast<GEntityNPC*>(data.second);
			vecRet.push_back(pNPC);
		}
	}
	return vecRet;
}

vector<GEntityActor*> GEntityMgr::GetActors()
{
	vector<GEntityActor*> vecRet;
	for each (const ENTITY_UID_MAP::value_type& data in m_EntityUIDMap)
	{
		if (data.second->IsActor())
		{
			GEntityActor* pActor = static_cast<GEntityActor*>(data.second);
			vecRet.push_back(pActor);
		}
	}
	return vecRet;
}

void GEntityMgr::GetPlayerUIDs(vector<MUID>& vecPlayerUIDs)
{
	for each (const ENTITY_UID_MAP::value_type& data in m_EntityUIDMap)
	{
		if (data.second->IsPlayer())
		{
			vecPlayerUIDs.push_back(data.first);
		}
	}
}

void GEntityMgr::AddNPC_UIID( GEntityNPC* pNPC )
{
	MUID uidNPC = pNPC->GetUID();
	UIID nNewUIID = m_IndexedNPCUIDFinder.Add(uidNPC);
	pNPC->RegUIID(nNewUIID);
}

void GEntityMgr::RemoveNPC_UIID( GEntityNPC* pNPC )
{
	UIID nUIID = pNPC->GetUIID();
	m_IndexedNPCUIDFinder.Del(nUIID);
}

void GEntityMgr::DestoryEntity(GEntity* pEntity)
{
	// 플레이어는 PlayerObjectManager가 책임진다.
	if (pEntity->IsPlayer())
	{
		MUID uidPlayer = pEntity->GetUID();
		gmgr.pPlayerObjectManager->DeletePlayer(uidPlayer);
	}
	else
	{
		if (pEntity->IsNPC())
		{
			gmgr.pNPCMgr->RemoveNPC(ToEntityNPC(pEntity));
		}
		
		pEntity->Destroy();
		pEntity->Drop();
	}
}

int GEntityMgr::GetGatherEntityCount( GATHER_TYPE nGatherType )
{
	return m_GatherEntityCounter.GetCount(nGatherType);
}

void GEntityMgr::AddNPC_GatherEntityCount( GEntityNPC* pNPC )
{
	if (pNPC->GetNPCInfo())
	{
		m_GatherEntityCounter.Inc(pNPC->GetNPCInfo());
	}
}

void GEntityMgr::RemoveNPC_GatherEntityCount( GEntityNPC* pNPC )
{
	if (pNPC->GetNPCInfo())
	{
		m_GatherEntityCounter.Dec(pNPC->GetNPCInfo());
	}
}

void GEntityMgr::ActiveSpawnGroup(int nGroupID)
{
	// 해당 그룹에 속한 NPC를 찾아 업데이트를 활성화한다.
	for(ENTITY_UID_MAP::iterator it = m_EntityUIDMap.begin(); it != m_EntityUIDMap.end(); it++)
	{
		GEntityNPC* pNPC = ToEntityNPC(it->second);

		if (pNPC == NULL)	
		{
			continue;
		}
		
		if (nGroupID == pNPC->GetGroupID())
		{
			pNPC->SetActivateUpdate(true);
		}
	}
}

int GEntityMgr::GetLiveNPCCount( int nNPCID )
{
	vector<GEntityNPC*> vecNPC = FindNPCByID(nNPCID);

	int nCount = 0;
	for each (GEntityNPC* pNPC in vecNPC)
	{
		if (true == pNPC->IsDead()) continue;
		if (true == pNPC->IsDeleteMe()) continue;

		nCount++;
	}
	
	return nCount;
}
