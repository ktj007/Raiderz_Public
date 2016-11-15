#include "StdAfx.h"
#include "GFieldGrid.h"
#include "GFieldInfo.h"
#include "GSectorAIEventInvoker.h"
#include "GSectorAIEventInvokerMgr.h"
#include "GAIStatistics.h" // AI_STAT_NPC_MAX
#include "GConst.h"
#include "GPacketRouteLimiterMgr.h"

//////////////////////////////////////////////////////////////////////////
//
//	GFieldGrid
//
//////////////////////////////////////////////////////////////////////////

GFieldGrid::GFieldGrid()
: m_pFieldInfo(NULL)
, m_Sectors(NULL)
, m_nSizeX(-1)
, m_nSizeY(-1)
, m_pAIInvokerMgr(NULL)
, m_pBoundChecker(NULL)
{
	m_pRouteLimiterMgr = new GPacketRouteLimiterMgr(this);
	m_pAIInvokerMgr = new GSectorAIEventInvokerMgr();
	m_pSelector = new SectorSelector(*this);
	m_pEntitySelector = new EntitySelector(*m_pSelector);
	m_pDeltaSelector = new DeltaSelector(*m_pSelector);
}

GFieldGrid::~GFieldGrid(void)
{
	Clear();

	SAFE_DELETE(m_pBoundChecker);
	SAFE_DELETE(m_pRouteLimiterMgr);
	SAFE_DELETE(m_pAIInvokerMgr);
	SAFE_DELETE(m_pSelector);
	SAFE_DELETE(m_pEntitySelector);
	SAFE_DELETE(m_pDeltaSelector);
}

void GFieldGrid::Init(const MUID uidField, const GFieldInfo* pFieldInfo)
{
	VALID(pFieldInfo);
	m_pFieldInfo = pFieldInfo;
	float fWidth = GetFieldMaxX() - GetFieldMinX();
	float fHeight = GetFieldMaxY() - GetFieldMinY();
	VALID(fWidth > 0.0f);
	VALID(fHeight > 0.0f);
	VALID(m_pAIInvokerMgr);

	m_nSizeX = (int)(fWidth / SECTOR_SIZE) + 1;
	m_nSizeY = (int)(fHeight / SECTOR_SIZE) + 1;

	m_Sectors = new Sector[m_nSizeX * m_nSizeY];
	for (int y = 0; y < m_nSizeY; ++y)
	{
		for (int x = 0; x < m_nSizeX; ++x)
		{
			m_Sectors[CalcFlatPoint(x, y)].SetPosition(x, y);
		}
	}

	m_pAIInvokerMgr->Init(uidField, m_nSizeX, m_nSizeY);
}

void GFieldGrid::Clear()
{
	SAFE_DELETE_ARRAY(m_Sectors);
	m_nSizeX = -1;
	m_nSizeY = -1;

	if (m_pAIInvokerMgr)
		m_pAIInvokerMgr->Clear();

	m_mapHugeSectors.clear();
}

void GFieldGrid::AddEntity( Sector& sector, ENTITY_TYPEID nEntityTypeID, const MUID& uid, bool bIsHuge )
{
	PFC_THISFUNC;
	VALID(m_pAIInvokerMgr);

	sector.AddEntity(nEntityTypeID, uid);

	const MPoint& sectorPt(sector.Position());
	if (nEntityTypeID == ETID_PLAYER &&
		m_pBoundChecker)
	{
		m_pBoundChecker->OnEnterSector(sectorPt, uid);
	}
	else if (nEntityTypeID == ETID_NPC)
	{
		AI_STAT_NPC_MAX(m_pFieldInfo->m_nFieldID, sectorPt.x, sectorPt.y, sector.GetEntityQty(ETID_NPC));
	}

	m_pAIInvokerMgr->AddEntity(sectorPt, nEntityTypeID, uid, GetEntitySelector());

	if (bIsHuge)
	{
		AddEntitiyToHugeSector(sectorPt, nEntityTypeID, uid);
	}

	GetPacketRouteLimiterMgr()->ProcessCreate(sector, nEntityTypeID);
}

bool GFieldGrid::RemoveEntity( const MPoint& ptSector, ENTITY_TYPEID nEntityTypeID, const MUID& uid, bool bIsHuge )
{
	PFC_THISFUNC;
	VALID_RET(m_pAIInvokerMgr, false);

	Sector* pSector = Lookup(ptSector);
	if (!pSector)
		return false;	// 유효하지 않은 셀

	if (!pSector->IsExist(uid))
		return false;	// 존재하지 않는 엔티티

	pSector->RemoveEntity(nEntityTypeID, uid);

	if (nEntityTypeID == ETID_PLAYER &&
		m_pBoundChecker)
	{
		m_pBoundChecker->OnLeaveSector(ptSector, uid);
	}

	m_pAIInvokerMgr->RemoveEntity(ptSector, nEntityTypeID, pSector);

	if (bIsHuge)
	{
		RemoveEntityFromHugeSector(ptSector, nEntityTypeID, uid);
	}

	GetPacketRouteLimiterMgr()->ProcessDestroy(*pSector, nEntityTypeID);

	return true;
}

void GFieldGrid::MoveEntity( const MPoint& ptOld, const MPoint& ptNew, ENTITY_TYPEID nEntityTypeID, const MUID& uid, bool bIsHuge )
{
	PFC_THISFUNC;
	VALID(m_pAIInvokerMgr);
	
	if (!IsValidIndex(ptOld))
	{
		//todo 이런 일이 일어났을 경우 처리가 필요 할 듯
		return;
	}

	Sector* const sector = Lookup(ptNew);
	if (NULL == sector)
	{
		// 유효하지 않은 셀
		return;
	}

	// AI LOD Event 가드 시작
	AIEVENT_INVOKER_GUARD(m_pAIInvokerMgr, ptOld);
	AIEVENT_INVOKER_GUARD(m_pAIInvokerMgr, ptNew);
	
	if (!RemoveEntity(ptOld, nEntityTypeID, uid, bIsHuge))
		return;

	AddEntity(*sector, nEntityTypeID, uid, bIsHuge);
}

GFieldGrid::SectorSelector* GFieldGrid::GetSectorSelector()
{
	return m_pSelector;
}


GFieldGrid::EntitySelector* GFieldGrid::GetEntitySelector()
{
	return m_pEntitySelector;
}

GFieldGrid::DeltaSelector* GFieldGrid::GetDeltaSelector()
{
	return m_pDeltaSelector;
}

void GFieldGrid::AddEntitiyToHugeSector( const MPoint& ptSector, ENTITY_TYPEID nEntityTypeID, MUID uidEntity )
{
	int nFlatPoint = CalcFlatPoint(ptSector);
	HugeSectorMap::iterator it = m_mapHugeSectors.find(nFlatPoint);
	Sector* cell=NULL;
	if (it == m_mapHugeSectors.end())
	{
		m_mapHugeSectors.insert(std::make_pair(nFlatPoint, Sector()));
		cell = &m_mapHugeSectors[nFlatPoint];
		cell->SetPosition(ptSector.x, ptSector.y);
	}
	else
	{
		cell = &it->second;
	}

	DCHECK(cell);
	cell->AddEntity(nEntityTypeID, uidEntity);
}

void GFieldGrid::RemoveEntityFromHugeSector( const MPoint& ptSector, ENTITY_TYPEID nEntityTypeID, MUID uidEntity )
{
	HugeSectorMap::iterator it = m_mapHugeSectors.find(CalcFlatPoint(ptSector));
	if (it == m_mapHugeSectors.end())
		return;

	Sector& cell = it->second;
	cell.RemoveEntity(nEntityTypeID, uidEntity);

	if (cell.GetEntityQty() <= 0)
	{
		m_mapHugeSectors.erase(it);
	}
}

GFieldGrid::Sector* const GFieldGrid::LookupHugeSector( MPoint ptSector ) const
{
	if (0 > ptSector.x || 0 > ptSector.y || ptSector.x >= m_nSizeX || ptSector.y >= m_nSizeY)
	{
		return NULL;
	}

	HugeSectorMap::const_iterator it = m_mapHugeSectors.find(CalcFlatPoint(ptSector));
	if (it == m_mapHugeSectors.end())
		return NULL;

	return const_cast<GFieldGrid::Sector*>(&it->second);
}

GPacketRouteLimiterMgr* GFieldGrid::GetPacketRouteLimiterMgr()
{
	return m_pRouteLimiterMgr;
}

void GFieldGrid::CreateBoundChecker( BoundChecker::Lisener* const pLisener )
{
	if (!m_pBoundChecker)
	{
		m_pBoundChecker = new BoundChecker(*GetSectorSelector());
		MRect rtBoundarySector;
		rtBoundarySector.left	= ((int)GetFieldInfo()->m_rtBoundary.left/SECTOR_SIZE)+1;
		rtBoundarySector.right	= ((int)GetFieldInfo()->m_rtBoundary.right/SECTOR_SIZE)-1;
		rtBoundarySector.top	= ((int)GetFieldInfo()->m_rtBoundary.top/SECTOR_SIZE)+1;
		rtBoundarySector.bottom	= ((int)GetFieldInfo()->m_rtBoundary.bottom/SECTOR_SIZE)-1;
		m_pBoundChecker->SetBoundary(rtBoundarySector);
	}
	
	m_pBoundChecker->SetLisener(pLisener);
}

const GFieldInfo* const GFieldGrid::GetFieldInfo() const
{
	return m_pFieldInfo;
}
