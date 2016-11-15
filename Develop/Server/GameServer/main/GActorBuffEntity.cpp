#include "StdAfx.h"
#include "GActorBuffEntity.h"
#include "GEntityActor.h"
#include "GBuffEntity.h"
#include "GField.h"

GActorBuffEntity::GActorBuffEntity(GEntityActor* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GActorBuffEntity::~GActorBuffEntity(void)
{
	m_pOwner->DetachObserver(this);
}

int GActorBuffEntity::GetQty(int nBuffID) const
{
	BuffEntityMap::const_iterator it = m_mapEntity.find(nBuffID);
	if (it == m_mapEntity.end())
	{
		return 0;
	}
	
	return it->second.size();
}

void GActorBuffEntity::DeleteFrontEntity(int nBuffID, int nLimitQty)
{
	BuffEntityMap::iterator it = m_mapEntity.find(nBuffID);
	if (it == m_mapEntity.end())
	{
		return;
	}

	deque<MUID>& vecEntities = it->second;
	if (vecEntities.empty())
		return;

	VALID(nLimitQty > 0);
	deque<MUID>::iterator itEntity = vecEntities.begin();
	while(true)
	{
		if (vecEntities.size() <= (size_t)nLimitQty)
			return;

		if (itEntity == vecEntities.end())
			return;

		GBuffEntity* pEntity = GetBuffEntity(*itEntity);
		if (pEntity)
		{
			pEntity->DeleteMe();		
		}
		itEntity = vecEntities.erase(itEntity);
	}
}

GBuffEntity* GActorBuffEntity::GetBuffEntity(MUID uidEntity)
{
	VALID_RET(m_pOwner->GetField(), NULL);
	GBuffEntity* pEntity = m_pOwner->GetField()->FindBuffEntity(uidEntity);
	return pEntity;
}

void GActorBuffEntity::OnCreateBuffEntity( int nBuffID, MUID uidEntity )
{
	if (m_mapEntity.find(nBuffID) == m_mapEntity.end())
	{
		m_mapEntity.insert(make_pair(nBuffID, deque<MUID>()));
	}

	m_mapEntity[nBuffID].push_back(uidEntity);
}

void GActorBuffEntity::OnDestroyBuffEntity( int nBuffID, MUID uidEntity )
{
	BuffEntityMap::iterator it = m_mapEntity.find(nBuffID);
	if (it == m_mapEntity.end())
		return;

	deque<MUID>& vecEntities = it->second;
	deque<MUID>::iterator itFind = 
		find(vecEntities.begin(), vecEntities.end(), uidEntity);
	if (itFind == vecEntities.end())
		return;
	
	vecEntities.erase(itFind);
	if (vecEntities.empty())
	{
		m_mapEntity.erase(nBuffID);
	}
}

