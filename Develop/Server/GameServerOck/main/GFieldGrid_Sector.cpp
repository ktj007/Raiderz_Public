#include "stdafx.h"
#include "GFieldGrid.h"

//////////////////////////////////////////////////////////////////////////
//
//	Sector
//
//////////////////////////////////////////////////////////////////////////

GFieldGrid::Sector::Sector()
{
}

GFieldGrid::Sector::~Sector()
{

}

void GFieldGrid::Sector::AddEntity( ENTITY_TYPEID nEntityTypeID, const MUID& uid )
{
	// 플레이어나 엔피씨는 액터 집합에도 추가
	if (nEntityTypeID == ETID_PLAYER ||
		nEntityTypeID == ETID_NPC)
	{
		m_setEntities[ETID_ACTOR].insert(uid);
	}

	m_setEntities[nEntityTypeID].insert(uid);
	m_setAllEntities.insert(uid);
}

void GFieldGrid::Sector::RemoveEntity( ENTITY_TYPEID nEntityTypeID, const MUID& uid )
{
	// 플레이어나 엔피씨는 액터 집합에서도 삭제
	if (nEntityTypeID == ETID_PLAYER ||
		nEntityTypeID == ETID_NPC)
	{
		m_setEntities[ETID_ACTOR].erase(uid);
	}

	m_setEntities[nEntityTypeID].erase(uid);
	m_setAllEntities.erase(uid);
}

void GFieldGrid::Sector::ClearEntity()
{
	for (int i = 0; i < ETID_MAX; i++)
		m_setEntities[i].clear();

	m_setAllEntities.clear();
}

bool GFieldGrid::Sector::IsExist( const MUID& uid ) const
{
	return m_setAllEntities.find(uid) != m_setAllEntities.end();
}

size_t GFieldGrid::Sector::GetEntityQty( ENTITY_TYPEID type ) const
{
	return m_setEntities[type].size();
}

size_t GFieldGrid::Sector::GetEntityQty() const
{
	return m_setAllEntities.size();
}

void GFieldGrid::Sector::Visit(IVisitor& visitor,const ENTITY_TYPEID& type) const
{
	VALID(ETID_ENTITY <= type && type <= ETID_MAX);

	if (type == ETID_MAX)
	{
		VisitAllTypes(visitor);
		return;
	}

	for each (const MUID& uid in m_setEntities[type])
	{
		visitor.OnVisit(uid);
	}
}

void GFieldGrid::Sector::VisitAllTypes(IVisitor& visitor) const
{
	for each (const MUID& uid in m_setAllEntities)
	{
		visitor.OnVisit(uid);
	}
}