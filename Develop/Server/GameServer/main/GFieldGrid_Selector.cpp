#include "stdafx.h"
#include "GFieldGrid.h"

//////////////////////////////////////////////////////////////////////////
//
//	Selector
//
//////////////////////////////////////////////////////////////////////////
namespace
{
	void CorrectIfOverHugeSectorHalfSideCount(long& count)
	{
		if (count > HUGE_SECTOR_SCALE)
		{
			count = HUGE_SECTOR_SCALE;
		}
	}
}

GFieldGrid::SectorSelector::SectorSelector( const GFieldGrid& grid )
: m_Grid(grid)
{
}

GFieldGrid::Sector* GFieldGrid::SectorSelector::Lookup( int x, int y ) const
{
	return m_Grid.Lookup(x, y);
}

GFieldGrid::Sector* GFieldGrid::SectorSelector::LookupHugeSector( int x, int y ) const
{
	return m_Grid.LookupHugeSector(MPoint(x, y));
}

void GFieldGrid::SectorSelector::VisitNeighbors(GFieldGrid::SectorSelector::IVisitor& visitor,const MPoint& sectorNo, long halfSideCnt) const
{
	CorrectIfOverHugeSectorHalfSideCount(halfSideCnt);

	for (int y = sectorNo.y - halfSideCnt; y <= sectorNo.y + halfSideCnt; ++y)
	{
		for (int x = sectorNo.x - halfSideCnt; x <= sectorNo.x + halfSideCnt; ++x)
		{
			Sector* const sector = m_Grid.Lookup(x, y);
			if (NULL != sector)
			{
				visitor.OnVisit(*sector);
			}
		}
	}
}

void GFieldGrid::SectorSelector::VisitNeighborsByPos(IVisitor& visitor,const vec3& position, long halfSideCnt) const
{
	CorrectIfOverHugeSectorHalfSideCount(halfSideCnt);
	VisitNeighbors(visitor, m_Grid.ConvertToIdx(position), halfSideCnt);
}

void GFieldGrid::SectorSelector::VisitNeighborsByRadius(IVisitor& visitor, const vec3& position, const float& radius) const
{
	VALID(0.0f < radius);
	long halfSideCnt = 1 + scast<int>(radius / SECTOR_SIZE);
	CorrectIfOverHugeSectorHalfSideCount(halfSideCnt);
	MPoint ptPoint = m_Grid.ConvertToIdx(position);

	for (int y = ptPoint.y - halfSideCnt; y <= ptPoint.y + halfSideCnt; ++y)
	{
		for (int x = ptPoint.x - halfSideCnt; x <= ptPoint.x + halfSideCnt; ++x)
		{
			Sector* const sector = m_Grid.Lookup(x, y);
			if (NULL == sector)
				continue;

			// 실제 섹터의 범위보다 radius만큼 넓은 범위를 만들어 해당 지점이 범위 안에 있다면 ok
			MBox cell_box(
				m_Grid.GetFieldMinX() + (x * SECTOR_SIZE) - radius,		// min_x
				m_Grid.GetFieldMinY() + (y * SECTOR_SIZE) - radius,		// min_y
				position.z,												// min_z
				m_Grid.GetFieldMinX() + ((x+1) * SECTOR_SIZE) + radius,	// max_x
				m_Grid.GetFieldMinY() + ((y+1) * SECTOR_SIZE) + radius,	// max_y
				position.z);											// max_z

			if (cell_box.IsOut(position))
				continue;

			visitor.OnVisit(*sector);
		}
	}
}

void GFieldGrid::SectorSelector::VisitNeighborsIntersectWithAABB(IVisitor& visitor, const vec3& position, const MBox& aabb) const
{
	VALID(aabb.minx < aabb.maxx);
	VALID(aabb.miny < aabb.maxy);
	MBox aabbAtPos(
		aabb.minx + position.x,
		aabb.miny + position.y,
		-1.0f,	// z는 고려하지 않음
		aabb.maxx + position.x,
		aabb.maxy + position.y,
		1.0f	// z는 고려하지 않음
		);

	MPoint ptPoint = m_Grid.ConvertToIdx(position);

	long halfSideCnt = 1 + scast<int>(aabbAtPos.GetRadius() / SECTOR_SIZE);
	CorrectIfOverHugeSectorHalfSideCount(halfSideCnt);
	for (int y = ptPoint.y - halfSideCnt; y <= ptPoint.y + halfSideCnt; ++y)
	{
		for (int x = ptPoint.x - halfSideCnt; x <= ptPoint.x + halfSideCnt; ++x)
		{
			Sector* const sector = m_Grid.Lookup(x, y);
			if (NULL == sector)
				continue;

			MBox sectorBox(
				m_Grid.GetFieldMinX() + (x * SECTOR_SIZE),		// min_x
				m_Grid.GetFieldMinY() + (y * SECTOR_SIZE),		// min_y
				0.0f,											// min_z
				m_Grid.GetFieldMinX() + ((x+1) * SECTOR_SIZE),	// max_x
				m_Grid.GetFieldMinY() + ((y+1) * SECTOR_SIZE),	// max_y
				0.0f);											// max_z

			if (sectorBox.IsOut(vec3(aabbAtPos.minx, aabbAtPos.miny, 0.0f)) &&
				sectorBox.IsOut(vec3(aabbAtPos.minx, aabbAtPos.maxy, 0.0f)) &&
				sectorBox.IsOut(vec3(aabbAtPos.maxx, aabbAtPos.miny, 0.0f)) &&
				sectorBox.IsOut(vec3(aabbAtPos.maxx, aabbAtPos.maxy, 0.0f)) &&
				!aabbAtPos.ContainsAABB(sectorBox))
			{
				continue;
			}

			visitor.OnVisit(*sector);
		}
	}
}

void GFieldGrid::SectorSelector::VisitNeighborsIntersectWithRect(IVisitor& visitor,const MRect& rect) const
{
	VALID(rect.left < rect.right);
	MPoint ptStart = m_Grid.ConvertToIdx(vec3(static_cast<float>(rect.left), static_cast<float>(rect.top), 0));
	MPoint ptEnd = m_Grid.ConvertToIdx(vec3(static_cast<float>(rect.right), static_cast<float>(rect.bottom), 0));

	long halfSideCnt = 1 + scast<int>((rect.right - rect.left) / SECTOR_SIZE);
	CorrectIfOverHugeSectorHalfSideCount(halfSideCnt);

	for (int x = ptStart.x - halfSideCnt; x <= ptEnd.x + halfSideCnt; ++x)
	{
		for (int y = ptStart.y - halfSideCnt; y <= ptEnd.y + halfSideCnt; ++y)
		{
			Sector* const sector = m_Grid.Lookup(x, y);
			if (NULL == sector)
				continue;

			visitor.OnVisit(*sector);
		}
	}
}

void GFieldGrid::SectorSelector::VisitHugeNeighbors(IVisitor& visitor,const MPoint& sectorNo) const
{
	for (int y = sectorNo.y - HUGE_SECTOR_SCALE; y < sectorNo.y + HUGE_SECTOR_SCALE * 2; ++y)
	{
		for (int x = sectorNo.x - HUGE_SECTOR_SCALE; x < sectorNo.x + HUGE_SECTOR_SCALE * 2; ++x)
		{
			Sector* const sector = m_Grid.LookupHugeSector(MPoint(x, y));
			if (NULL == sector)
				continue;

			visitor.OnVisit(*sector);
		}
	}
}

// void GFieldGrid::SectorSelector::GetAllNeighborSectorsExceptThis( vector<Sector*>& vecSectors, MPoint ptPoint, int nSize ) const
// {
// 	class GetNeighbotExceptCenterSector : public IVisitor
// 	{
// 	public:
// 		GetNeighbotExceptCenterSector(vector<Sector*>& vecSectors,const Sector* const exceptSector)
// 			: m_vecSectors(vecSectors)
// 			, m_exceptSector(exceptSector)
// 		{
// 		}
// 
// 	private:
// 		void OnVisit(Sector& sector)
// 		{
// 			if (m_exceptSector == &sector)
// 			{
// 				return;
// 			}
// 			m_vecSectors.push_back(&sector);
// 		}
// 
// 	private:
// 		vector<Sector*>&	m_vecSectors;
// 		const Sector* const	m_exceptSector;
// 	};
// 
// 	Sector* const exceptSector = m_Grid.Lookup(ptPoint.x, ptPoint.y);
// 	SectorSelector::VisitNeighbors( GetNeighbotExceptCenterSector(vecSectors, exceptSector), ptPoint, nSize );
// }

//////////////////////////////////////////////////////////////////////////
//
//	EntitySelector
//
//////////////////////////////////////////////////////////////////////////

GFieldGrid::EntitySelector::EntitySelector( const SectorSelector& sectorSelector )
: m_sectorSelector(sectorSelector)
{
}

namespace
{
	class SectorEntitiesPusher : public GFieldGrid::Sector::IVisitor
	{
	public:
		SectorEntitiesPusher(vector<MUID>& vecEntities) : m_vecEntities(vecEntities) {}

	private:
		void OnVisit(const MUID& uid)
		{
			m_vecEntities.push_back(uid);
		}
	private:
		vector<MUID>& m_vecEntities;
	};

// 	class SectorEntitiesPusherWithExceptableID : public GFieldGrid::Sector::IVisitor
// 	{
// 	public:
// 		SectorEntitiesPusherWithExceptableID(vector<MUID>& vecEntities,const MUID& uid)
// 			: m_vecEntities(vecEntities)
// 			, m_uid(uid)
// 		{
// 
// 		}
// 
// 	private:
// 		void OnVisit(const MUID& uid)
// 		{
// 			if (m_uid == uid)
// 			{
// 				return;
// 			}
// 			m_vecEntities.push_back(uid);
// 		}
// 
// 	private:
// 		vector<MUID>&	m_vecEntities;
// 		const MUID&		m_uid;
// 	};

// 	class SectorEntitiesFilteringPusher : public GFieldGrid::Sector::IVisitor
// 	{
// 	public:
// 		SectorEntitiesFilteringPusher(vector<MUID>& vecEntities,GFieldGrid::GSelectorFilter& filter)
// 			: m_vecEntities(vecEntities)
// 			, m_filter(filter)
// 		{
// 
// 		}
// 
// 	private:
// 		void OnVisit(const MUID& uid)
// 		{
// 			if (true == m_filter.Check(uid))
// 			{
// 				m_vecEntities.push_back(uid);
// 			}
// 		}
// 	private:
// 		vector<MUID>&					m_vecEntities;
// 		GFieldGrid::GSelectorFilter&	m_filter;
// 	};
// 
// 	class SectorEntitiesFilteringPusherWithExceptableID : public GFieldGrid::Sector::IVisitor
// 	{
// 	public:
// 		SectorEntitiesFilteringPusherWithExceptableID(vector<MUID>& vecEntities,const MUID& uid,GFieldGrid::GSelectorFilter& filter)
// 			: m_vecEntities(vecEntities)
// 			, m_uid(uid)
// 			, m_filter(filter)
// 		{
// 
// 		}
// 
// 	private:
// 		void OnVisit(const MUID& uid)
// 		{
// 			if (m_uid == uid)
// 			{
// 				return;
// 			}
// 
// 			if (true == m_filter.Check(uid))
// 			{
// 				m_vecEntities.push_back(uid);
// 			}
// 		}
// 
// 	private:
// 		vector<MUID>&					m_vecEntities;
// 		const MUID&						m_uid;
// 		GFieldGrid::GSelectorFilter&	m_filter;
// 	};

	class VisitingSector : public GFieldGrid::SectorSelector::IVisitor
	{
	public:
		VisitingSector(GFieldGrid::Sector::IVisitor& visitor,const ENTITY_TYPEID& type)
			: m_visitor(visitor), m_type(type) { }

	private:
		void OnVisit(GFieldGrid::Sector& sector) { sector.Visit(m_visitor, m_type); }

	private:
		GFieldGrid::Sector::IVisitor&	m_visitor;
		const ENTITY_TYPEID&			m_type;
	};
}

void GFieldGrid::EntitySelector::Visit(Sector::IVisitor& visitor,ENTITY_TYPEID type,const MPoint& sectorNo,const long& halfSideCnt) const
{
	m_sectorSelector.VisitNeighbors( VisitingSector(visitor, type), sectorNo, halfSideCnt );
}

void GFieldGrid::EntitySelector::VisitByPos(Sector::IVisitor& visitor,ENTITY_TYPEID type,const vec3& position,const long& halfSideCnt) const
{
	m_sectorSelector.VisitNeighborsByPos( VisitingSector(visitor, type), position, halfSideCnt );
}

void GFieldGrid::EntitySelector::VisitByRadius(Sector::IVisitor& visitor,ENTITY_TYPEID type,const vec3& position,const float& radius) const
{
	m_sectorSelector.VisitNeighborsByRadius( VisitingSector(visitor, type), position, radius );
}

void GFieldGrid::EntitySelector::VisitIntersectWithAABB(Sector::IVisitor& visitor,ENTITY_TYPEID type,const vec3& position,const MBox& aabb) const
{
	m_sectorSelector.VisitNeighborsIntersectWithAABB( VisitingSector(visitor, type), position, aabb );
}

void GFieldGrid::EntitySelector::VisitIntersectWithRect(Sector::IVisitor& visitor,ENTITY_TYPEID type,const MRect& rect) const
{
	m_sectorSelector.VisitNeighborsIntersectWithRect( VisitingSector(visitor, type), rect );
}

void GFieldGrid::EntitySelector::VisitInHugeSectors(Sector::IVisitor& visitor,ENTITY_TYPEID type,const MPoint& sectorNo) const
{
	m_sectorSelector.VisitHugeNeighbors( VisitingSector(visitor, type), sectorNo);
}

// void GFieldGrid::EntitySelector::GetEntitiesEx( vector<MUID>& vecEntities, ENTITY_TYPEID type, const vector<Sector*>& vecSectors, GSelectorFilter* pFilter, MUID uidExcept ) const
// {
// 	GVectorMUID vecCellEntities;
// 	for each (Sector* sector in vecSectors)
// 	{
// 		vecCellEntities.Vector().clear();
// 		if (uidExcept.IsValid())
// 		{
// 			sector->Visit(SectorEntitiesFilteringPusherWithExceptableID(vecEntities, uidExcept, *pFilter), type);
// 
// 			return;
// 		}
// 		else
// 		{
// 			sector->Visit(SectorEntitiesFilteringPusher(vecEntities, *pFilter), type);
// 		}
// 
// 		if (vecCellEntities.Vector().empty())
// 			continue;
// 
// 		std::copy(vecCellEntities.Vector().begin(), vecCellEntities.Vector().end(), std::back_inserter(vecEntities));
// 	}
// }

void GFieldGrid::EntitySelector::GetActors( vector<MUID>& vecEntities, MPoint ptSector, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_ACTOR, ptSector, nSize);
}

void GFieldGrid::EntitySelector::GetPlayers( vector<MUID>& vecEntities, MPoint ptSector, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_PLAYER, ptSector, nSize);
}

void GFieldGrid::EntitySelector::GetNPCs( vector<MUID>& vecEntities, MPoint ptSector, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_NPC, ptSector, nSize);
}

void GFieldGrid::EntitySelector::GetActors( vector<MUID>& vecEntities, vec3 vPoint, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_ACTOR, m_sectorSelector.Grid().ConvertToIdx(vPoint), nSize);
}

void GFieldGrid::EntitySelector::GetPlayers( vector<MUID>& vecEntities, vec3 vPoint, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_PLAYER, m_sectorSelector.Grid().ConvertToIdx(vPoint), nSize);
}

void GFieldGrid::EntitySelector::GetNPCs( vector<MUID>& vecEntities, vec3 vPoint, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_NPC, m_sectorSelector.Grid().ConvertToIdx(vPoint), nSize);
}

void GFieldGrid::EntitySelector::GetBuffEntities( vector<MUID>& vecEntities, vec3 vPoint, int nSize/*=1*/ ) const
{
	Visit(SectorEntitiesPusher(vecEntities), ETID_BUFF_ENTITY, m_sectorSelector.Grid().ConvertToIdx(vPoint), nSize);
}

void GFieldGrid::EntitySelector::GetActors( vector<MUID>& vecEntities, const MRect& rect, int nSize/*=1 */ ) const
{
	VisitIntersectWithRect(SectorEntitiesPusher(vecEntities), ETID_ACTOR, rect);
}

void GFieldGrid::EntitySelector::GetPlayers( vector<MUID>& vecEntities, const MRect& rect, int nSize/*=1 */ ) const
{
	VisitIntersectWithRect(SectorEntitiesPusher(vecEntities), ETID_PLAYER, rect);
}

void GFieldGrid::EntitySelector::GetNPCs( vector<MUID>& vecEntities, const MRect& rect, int nSize/*=1 */ ) const
{
	VisitIntersectWithRect(SectorEntitiesPusher(vecEntities), ETID_NPC, rect);
}

void GFieldGrid::EntitySelector::GetPlayers(vector<MUID>& vecEntities,MPoint ptSector,int nSize ,MUID uidExcept) const
{
	class VisitingNeighborSectors : public SectorSelector::IVisitor
	{
	public:
		VisitingNeighborSectors(vector<MUID>& vecEntities)
			: m_vecEntities(vecEntities)
		{
		}

	private:
		void OnVisit(Sector& sector)
		{
			sector.Visit(SectorEntitiesPusher(m_vecEntities), ETID_PLAYER);
		}

	private:
		vector<MUID>& m_vecEntities;
	};

	m_sectorSelector.VisitNeighbors( VisitingNeighborSectors(vecEntities), ptSector, nSize );
}

void GFieldGrid::EntitySelector::GetPlayers(vector<MUID>& vecEntities,vec3 vPoint,int nSize,MUID uidExcept) const
{
	GetPlayers(vecEntities, m_sectorSelector.Grid().ConvertToIdx(vPoint), nSize, uidExcept);
}

// void GFieldGrid::EntitySelector::GetNPCsEx( vector<MUID>& vecEntities, const vector<Sector*>& vecSectors, GSelectorFilter* pFilter, MUID uidExcept ) const
// {
// 	GetEntitiesEx(vecEntities, ETID_NPC, vecSectors, pFilter, uidExcept);	
// }

bool GFieldGrid::EntitySelector::IsEmptyPlayers( MPoint ptSector, int nSize/*=1*/ ) const
{
	int playersCount = 0;

	class SectorPlayersCounter : public GFieldGrid::Sector::IVisitor
	{
	public:
		SectorPlayersCounter(int& result) : m_result(result)	{}

	private:
		void OnVisit(const MUID& uid)
		{
			++m_result;
		}

	private:
		int& m_result;
	};

	class SectorVisitor : public SectorSelector::IVisitor
	{
	public:
		SectorVisitor(int& result) : m_result(result) {}

	private:
		void OnVisit(Sector& sector)
		{
			sector.Visit(SectorPlayersCounter(m_result), ETID_PLAYER);
		}

	private:
		int& m_result;
	};

	m_sectorSelector.VisitNeighbors( SectorVisitor(playersCount), ptSector, nSize );

	return 0 == playersCount;
}

bool GFieldGrid::EntitySelector::IsEmptyPlayers( vec3 vPoint, int nSize/*=1*/ ) const
{
	return IsEmptyPlayers(m_sectorSelector.Grid().ConvertToIdx(vPoint), nSize);
}

void GFieldGrid::EntitySelector::GetHugeEntities( vector<MUID>& vecEntities, vec3 vPoint, ENTITY_TYPEID type ) const
{
	vecEntities.clear();

	MPoint ptSector = m_sectorSelector.Grid().ConvertToIdx(vPoint);
	for (int y=ptSector.y-HUGE_SECTOR_SCALE * 3 / 2; y<=ptSector.y+HUGE_SECTOR_SCALE * 3 / 2; ++y)
	{
		if (0 > y)
		{
			continue;
		}

		for (int x=ptSector.x-HUGE_SECTOR_SCALE * 3 / 2; x<=ptSector.x+HUGE_SECTOR_SCALE * 3 / 2; ++x)
		{
			if (0 > x)
			{
				continue;
			}

			const GFieldGrid::Sector* const pSector = m_sectorSelector.Grid().LookupHugeSector(MPoint(x, y));
			if (!pSector)
				continue; // 거대몹이 없음

			pSector->Visit(SectorEntitiesPusher(vecEntities), type);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
//	DeltaSelector
//
//////////////////////////////////////////////////////////////////////////

GFieldGrid::DeltaSelector::DeltaSelector( const SectorSelector& sectorSelector)
: m_sectorSelector(sectorSelector)
{
}

GFieldGrid::MovingDirection 
GFieldGrid::DeltaSelector::CalcMovingDirection( MPoint ptOld, MPoint ptNew ) const
{
	int nIntervalX = ptNew.x - ptOld.x;
	int nIntervalY = ptNew.y - ptOld.y;
	MovingDirection nDirction = MD_NONE;

	if (ptNew.x == -1 && ptNew.y == -1)
		return MD_FULL;

	if (ptOld.x == -1 && ptOld.y == -1)
		return MD_FULL;

	if (nIntervalX > 1 || nIntervalY > 1 ||
		nIntervalX < -1 || nIntervalY < -1)				
	{
		return MD_FULL;
	}

	if (nIntervalX == -1 && nIntervalY == 0)		
		return MD_LEFT;
	if (nIntervalX == -1 && nIntervalY == -1)		
		return MD_TOPLEFT;
	if (nIntervalX == 0 && nIntervalY == -1)		
		return MD_TOP;
	if (nIntervalX == 1 && nIntervalY == -1)		
		return MD_TOPRIGHT;
	if (nIntervalX == 1 && nIntervalY == 0)		
		return MD_RIGHT;
	if (nIntervalX == 1 && nIntervalY == 1)		
		return MD_BOTTOMRIGHT;
	if (nIntervalX == 0 && nIntervalY == 1)		
		return MD_BOTTOM;
	if (nIntervalX == -1 && nIntervalY == 1)		
		return MD_BOTTOMLEFT;
	if (nIntervalX == 0 && nIntervalY == 0)		
		return MD_SAME;

	return MD_NONE;
}

void GFieldGrid::DeltaSelector::VisitNewSectors(Sector::IVisitor& visitor, ENTITY_TYPEID entityType, const MPoint& oldSectorPt, const MPoint& newSectorPt, int nSize ) const
{
	VisitImpl(visitor, entityType, &SectorSelector::Lookup, false, CalcMovingDirection(oldSectorPt, newSectorPt), newSectorPt, nSize);
}

void GFieldGrid::DeltaSelector::VisitOldSectors(Sector::IVisitor& visitor,ENTITY_TYPEID entityType,const MPoint& oldSectorPt,const MPoint& newSectorPt,int nSize) const
{
	VisitImpl(visitor, entityType, &SectorSelector::Lookup, true, CalcMovingDirection(oldSectorPt, newSectorPt), oldSectorPt, nSize);
}

void GFieldGrid::DeltaSelector::VisitNewHugeSectors(Sector::IVisitor& visitor,ENTITY_TYPEID entityType,const MPoint& oldSectorPt,const MPoint& newSectorPt) const
{
	VisitImpl(visitor, entityType, &SectorSelector::LookupHugeSector, false, CalcMovingDirection(oldSectorPt, newSectorPt), newSectorPt, HUGE_SECTOR_SCALE);
}

void GFieldGrid::DeltaSelector::VisitOldHugeSectors(Sector::IVisitor& visitor,ENTITY_TYPEID entityType,const MPoint& oldSectorPt,const MPoint& newSectorPt) const
{
	VisitImpl(visitor, entityType, &SectorSelector::LookupHugeSector, true, CalcMovingDirection(oldSectorPt, newSectorPt), oldSectorPt, HUGE_SECTOR_SCALE);
}

void GFieldGrid::DeltaSelector::GetDeltaNewSectors( vector<Sector*>& vecSectors, 
												   MPoint ptOld, 
												   MPoint ptNew,
												   int nSize/*=1*/ ) const
{
	MovingDirection nDirection = CalcMovingDirection(ptOld, ptNew);
	GetDeltaSectorsImpl(false,	vecSectors, nDirection, ptNew, nSize);
}

void GFieldGrid::DeltaSelector::GetDeltaOldSectors( vector<Sector*>& vecSectors, 
												   MPoint ptOld, 
												   MPoint ptNew,
												   int nSize/*=1*/ ) const
{
	MovingDirection nDirection = CalcMovingDirection(ptOld, ptNew);
	GetDeltaSectorsImpl(true,	vecSectors, nDirection, ptOld, nSize);
}

// void GFieldGrid::DeltaSelector::GetDeltaSectors( vector<Sector*>& vecOldSectors, 
// 												vector<Sector*>& vecNewSectors, 
// 												MPoint ptOld, 
// 												MPoint ptNew,
// 												int nSize/*=1*/ ) const
// {
// 	MovingDirection nDirection = CalcMovingDirection(ptOld, ptNew);
// 	GetDeltaSectorsImpl(true,		vecOldSectors, nDirection, ptOld, nSize);
// 	GetDeltaSectorsImpl(false,	vecNewSectors, nDirection, ptNew, nSize);
// }

void GFieldGrid::DeltaSelector::GetDeltaNewHugeSectors( vector<Sector*>& vecSectors, 
													   MPoint ptOld, 
													   MPoint ptNew) const
{
	MovingDirection nDirection = CalcMovingDirection(ptOld, ptNew);
	GetDeltaSectorsImpl(false,	vecSectors, nDirection, ptNew, HUGE_SECTOR_SCALE, MD_NONE, true);
}

void GFieldGrid::DeltaSelector::GetDeltaSectorsImpl(	bool bOld,
													vector<Sector*>& vecSectors, 
													MovingDirection nDirection, 
													MPoint ptSector, 
													int nRealSize,
													MovingDirection nInnerCallType,
													bool bIsHuge
													) const
{
	if (nInnerCallType==MD_NONE)
	{
		vecSectors.clear();
	}

	int nStartX = ptSector.x-nRealSize;
	int nEndX = ptSector.x+(nRealSize*2);
	int nStartY = ptSector.y-nRealSize;
	int nEndY = ptSector.y+(nRealSize*2);

	// 새로운 셀은 반대 위치로 구함
	int sign = (bOld)?1: -1;

	// 이동 셀은 old 기준으로 선출
	switch(nDirection)
	{
	case MD_SAME:
		return;
	case MD_FULL:
		break;
	case MD_TOP:
		nStartY = ptSector.y+(sign*nRealSize);
		nEndY = nStartY+nRealSize;
		break;
	case MD_BOTTOM:
		nStartY = ptSector.y-(sign*nRealSize);
		nEndY = nStartY+nRealSize;
		break;
	case MD_LEFT:
		nStartX = ptSector.x+(sign*nRealSize);
		nEndX = nStartX+nRealSize;
		break;
	case MD_RIGHT:
		nStartX = ptSector.x-(sign*nRealSize);
		nEndX = nStartX+nRealSize;
		break;
	case MD_TOPLEFT:
		GetDeltaSectorsImpl(bOld, vecSectors, MD_TOP, ptSector, nRealSize, MD_NONE, bIsHuge);
		GetDeltaSectorsImpl(bOld, vecSectors, MD_LEFT, ptSector, nRealSize, MD_TOP, bIsHuge);
		return; 
	case MD_TOPRIGHT:
		GetDeltaSectorsImpl(bOld, vecSectors, MD_TOP, ptSector, nRealSize, MD_NONE, bIsHuge);
		GetDeltaSectorsImpl(bOld, vecSectors, MD_RIGHT, ptSector, nRealSize, MD_TOP, bIsHuge);
		return; 
	case MD_BOTTOMLEFT:
		GetDeltaSectorsImpl(bOld, vecSectors, MD_BOTTOM, ptSector, nRealSize, MD_NONE, bIsHuge);
		GetDeltaSectorsImpl(bOld, vecSectors, MD_LEFT, ptSector, nRealSize, MD_BOTTOM, bIsHuge);
		return; 
	case MD_BOTTOMRIGHT:
		GetDeltaSectorsImpl(bOld, vecSectors, MD_BOTTOM, ptSector, nRealSize, MD_NONE, bIsHuge);
		GetDeltaSectorsImpl(bOld, vecSectors, MD_RIGHT, ptSector, nRealSize, MD_BOTTOM, bIsHuge);
		return; 
	default:
		VALID(false);
	}

	switch(nInnerCallType)
	{
	case MD_TOP:
		if (nDirection == MD_LEFT ||
			nDirection == MD_RIGHT)
		{
			if (bOld)	nEndY -= nRealSize;
			else		nStartY += nRealSize;
		}
		break;
	case MD_BOTTOM:
		if (nDirection == MD_LEFT ||
			nDirection == MD_RIGHT)
		{
			if (bOld)	nStartY += nRealSize;
			else		nEndY -= nRealSize;
		}
		break;
	}

	for (int y=nStartY; y<nEndY; ++y)
	{
		if (y < 0) 
			continue;

		for (int x=nStartX; x<nEndX; ++x)
		{
			if (x < 0)
				continue;

			Sector* pSector = NULL;
			if (bIsHuge)
				pSector = m_sectorSelector.LookupHugeSector(x, y);
			else
				pSector = m_sectorSelector.Lookup(x, y);

			if (!pSector)
				continue;

			vecSectors.push_back(pSector);
		}
	}
}

void GFieldGrid::DeltaSelector::VisitImpl(Sector::IVisitor& visitor, ENTITY_TYPEID entityType, SectorLookupFunc sectorLookUp, bool bOld,MovingDirection nDirection,const MPoint& ptSector,int nRealSize,MovingDirection nInnerCallType) const
{
	VALID(NULL!=sectorLookUp);

	int nStartX = ptSector.x-nRealSize;
	int nEndX = ptSector.x+(nRealSize*2);
	int nStartY = ptSector.y-nRealSize;
	int nEndY = ptSector.y+(nRealSize*2);

	// 새로운 셀은 반대 위치로 구함
	int sign = (bOld)?1: -1;

	// 이동 셀은 old 기준으로 선출
	switch(nDirection)
	{
	case MD_SAME:
		return;
	case MD_FULL:
		break;
	case MD_TOP:
		nStartY = ptSector.y+(sign*nRealSize);
		nEndY = nStartY+nRealSize;
		break;
	case MD_BOTTOM:
		nStartY = ptSector.y-(sign*nRealSize);
		nEndY = nStartY+nRealSize;
		break;
	case MD_LEFT:
		nStartX = ptSector.x+(sign*nRealSize);
		nEndX = nStartX+nRealSize;
		break;
	case MD_RIGHT:
		nStartX = ptSector.x-(sign*nRealSize);
		nEndX = nStartX+nRealSize;
		break;
	case MD_TOPLEFT:
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_TOP, ptSector, nRealSize, MD_NONE);
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_LEFT, ptSector, nRealSize, MD_TOP);
		return; 
	case MD_TOPRIGHT:
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_TOP, ptSector, nRealSize, MD_NONE);
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_RIGHT, ptSector, nRealSize, MD_TOP);
		return; 
	case MD_BOTTOMLEFT:
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_BOTTOM, ptSector, nRealSize, MD_NONE);
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_LEFT, ptSector, nRealSize, MD_BOTTOM);
		return; 
	case MD_BOTTOMRIGHT:
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_BOTTOM, ptSector, nRealSize, MD_NONE);
		VisitImpl(visitor, entityType, sectorLookUp, bOld, MD_RIGHT, ptSector, nRealSize, MD_BOTTOM);
		return; 
	default:
		VALID(false);
	}

	switch(nInnerCallType)
	{
	case MD_TOP:
		if (nDirection == MD_LEFT ||
			nDirection == MD_RIGHT)
		{
			if (bOld)	nEndY -= nRealSize;
			else		nStartY += nRealSize;
		}
		break;
	case MD_BOTTOM:
		if (nDirection == MD_LEFT ||
			nDirection == MD_RIGHT)
		{
			if (bOld)	nStartY += nRealSize;
			else		nEndY -= nRealSize;
		}
		break;
	}

	for (int y=nStartY; y<nEndY; ++y)
	{
		if (y < 0) 
			continue;

		for (int x=nStartX; x<nEndX; ++x)
		{
			if (x < 0)
				continue;

			Sector* const pSector = (m_sectorSelector.*sectorLookUp)(x, y);
			if (NULL == pSector)
				continue;

			pSector->Visit(visitor, entityType);
		}
	}
}



class SectorEntitiesPusherWithExceptableID : public GFieldGrid::Sector::IVisitor
{
public:
	SectorEntitiesPusherWithExceptableID(vector<MUID>& vecEntities,const MUID& uid)
		: m_vecEntities(vecEntities)
		, m_uid(uid)
	{

	}

private:
	void OnVisit(const MUID& uid)
	{
		if (m_uid == uid)
		{
			return;
		}
		m_vecEntities.push_back(uid);
	}

private:
	vector<MUID>&	m_vecEntities;
	const MUID&		m_uid;
};

void GFieldGrid::EntitySelector::GetEntities( vector<MUID>& vecEntities, ENTITY_TYPEID type, const vector<Sector*>& vecSectors, MUID uidExcept ) const
{
	for each (Sector* const sector in vecSectors)
	{
		if (uidExcept.IsValid())
		{
			sector->Visit(SectorEntitiesPusherWithExceptableID(vecEntities, uidExcept), type);
		}
		else
		{
			sector->Visit(SectorEntitiesPusher(vecEntities), type);
		}		
	}
}

void GFieldGrid::EntitySelector::GetNPCs( vector<MUID>& vecEntities, const vector<Sector*>& vecSectors, MUID uidExcept ) const
{
	GetEntities(vecEntities, ETID_NPC, vecSectors, uidExcept);
}

void GFieldGrid::EntitySelector::GetActors( vector<MUID>& vecEntities, const vector<Sector*>& vecSectors, MUID uidExcept ) const
{
	GetEntities(vecEntities, ETID_ACTOR, vecSectors, uidExcept);
}

void GFieldGrid::EntitySelector::GetPlayers( vector<MUID>& vecEntities, const vector<Sector*>& vecSectors, MUID uidExcept ) const
{
	GetEntities(vecEntities, ETID_PLAYER, vecSectors, uidExcept);
}

void GFieldGrid::EntitySelector::GetBuffEntities( vector<MUID>& vecEntities, const vector<Sector*>& vecSectors, MUID uidExcept ) const
{
	GetEntities(vecEntities, ETID_BUFF_ENTITY, vecSectors, uidExcept);
}