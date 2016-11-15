#include "StdAfx.h"
#include "GPacketRouteLimiterMgr.h"
#include "GPacketRouteLimiter.h"
#include "GFieldGrid.h"
#include "GConst.h"

GPacketRouteLimiterMgr::GPacketRouteLimiterMgr(GFieldGrid* pGrid)
: m_pGrid(pGrid)
{
}

GPacketRouteLimiterMgr::~GPacketRouteLimiterMgr(void)
{
}


void GPacketRouteLimiterMgr::ProcessCreate( GFieldGrid::Sector& sector, ENTITY_TYPEID nEntityTypeID )
{
	if (GConst::SECTOR_CROWDED_LEVEL_CAPACITY == 0)
		return;

	if (GConst::SECTOR_PACKET_IGNORE_EACH_COUNT == 0)
		return;

	const MPoint& sectorPt(sector.Position());
	uint32 nThisFlatPoint = m_pGrid->CalcFlatPoint(sectorPt);
	if (m_mapPacketLimit.find(nThisFlatPoint) != m_mapPacketLimit.end())
		return;

	if ((int)sector.GetEntityQty(ETID_PLAYER) < GConst::SECTOR_CROWDED_LEVEL_CAPACITY)
		return;

	for (int y=sectorPt.y-1; y<=sectorPt.y+1; ++y)
	{
		if (0 > y)
		{
			continue;;
		}
		for (int x=sectorPt.x-1; x<=sectorPt.x+1; ++x)
		{
			if (0 > x)
			{
				continue;;
			}

			uint32 nFlatPoint = m_pGrid->CalcFlatPoint(MPoint(x,y));
			if (m_mapPacketLimit.find(nFlatPoint) != m_mapPacketLimit.end())
				continue;

			m_mapPacketLimit.insert(make_pair(nFlatPoint, GPacketRouteLimiter()));
			m_mapPacketLimit[nFlatPoint].AddOwner(nThisFlatPoint);
		}
	}
}

void GPacketRouteLimiterMgr::ProcessDestroy( GFieldGrid::Sector& sector, ENTITY_TYPEID nEntityTypeID )
{
	if (GConst::SECTOR_CROWDED_LEVEL_CAPACITY == 0)
		return;

	if (GConst::SECTOR_PACKET_IGNORE_EACH_COUNT == 0)
		return;

	const MPoint& sectorPt(sector.Position());
	uint32 nThisFlatPoint = m_pGrid->CalcFlatPoint(sectorPt);
	if (m_mapPacketLimit.find(nThisFlatPoint) == m_mapPacketLimit.end())
		return;

	if (!m_mapPacketLimit[nThisFlatPoint].IsOwnerSector(nThisFlatPoint))
		return; // 직접 생성된 섹터가 아님

	if ((int)sector.GetEntityQty(ETID_PLAYER) >= GConst::SECTOR_CROWDED_LEVEL_CAPACITY)
		return;

	for (int y=sectorPt.y-1; y<=sectorPt.y+1; ++y)
	{
		if (0 > y)
		{
			continue;;
		}

		for (int x=sectorPt.x-1; x<=sectorPt.x+1; ++x)
		{
			if (0 > x)
			{
				continue;;
			}

			uint32 nFlatPoint = m_pGrid->CalcFlatPoint(MPoint(x,y));
			PacketLimiterMap::iterator it = m_mapPacketLimit.find(nFlatPoint);
			if (it == m_mapPacketLimit.end())
				continue;

			GPacketRouteLimiter& limiter = it->second;
			limiter.RemoveOwner(nThisFlatPoint);
			if (!limiter.IsEmptyOwner())
				continue;

			m_mapPacketLimit.erase(nFlatPoint);
		}
	}
}

GPacketRouteLimiter* GPacketRouteLimiterMgr::Get( MPoint ptSector )
{
	VALID_RET(NULL!=m_pGrid, NULL);
	if (false == m_pGrid->IsValidIndex(ptSector))
		return NULL;

	uint32 nFlatPoint = m_pGrid->CalcFlatPoint(ptSector);
	PacketLimiterMap::iterator it = m_mapPacketLimit.find(nFlatPoint);
	if (it == m_mapPacketLimit.end())
		return NULL;

	return &it->second;
}

bool GPacketRouteLimiterMgr::TryRoute(const GFieldGrid::Sector& sector,int nCmdID,MUID uidSender)
{
	if (m_pGrid->GetFieldInfo()->m_bDynamic)
		return true; // 동적필드일 때는 무조건 유효

	GPacketRouteLimiter* const pPacketLimiter = Get(sector.Position());
	if (!pPacketLimiter)
		return true; // 패킷 전송 제한 없음

	return pPacketLimiter->TryRoute(sector.GetEntityQty(ETID_PLAYER), nCmdID, uidSender);
}

int GPacketRouteLimiterMgr::CalcCrowdedLevel( MPoint ptSector, int nPlayerQty ) 
{
	GPacketRouteLimiter* pPacketLimiter = Get(ptSector);
	if (!pPacketLimiter)
		return 0;

	return pPacketLimiter->CalcCrowdedLevel(nPlayerQty);
}
