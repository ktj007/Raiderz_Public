#include "StdAfx.h"
#include "GFieldGrid.h"

//////////////////////////////////////////////////////////////////////////
//
//	BoundChecker
//
//////////////////////////////////////////////////////////////////////////

GFieldGrid::BoundChecker::BoundChecker(GFieldGrid::SectorSelector& selector) 
: m_pLisener(NULL)
, m_Selector(selector)
{

}

void GFieldGrid::BoundChecker::SetLisener( Lisener* pLisener )
{
	m_pLisener = pLisener;
}

void GFieldGrid::BoundChecker::SetBoundary( MRect rtBoundary )
{
	m_rtBoundary = rtBoundary;
}

void GFieldGrid::BoundChecker::OnEnterSector( const MPoint& sectorPt, MUID uidPlayer )
{
	//if (!IsBoundary(sx, sy))
	//	return;

	//if (m_pLisener)
	//{
	//	m_pLisener->OnEnterBoundSector(sx, sy, uidPlayer, GetPlayerQtyInBoundary());
	//}

	if (IsBoundary(sectorPt))
		m_setOutters.insert(uidPlayer);
	else
		m_setOutters.erase(uidPlayer);

	if (m_pLisener)
	{
		m_pLisener->OnChangeOutters(GetPlayerQtyInBoundary());
	}
}

void GFieldGrid::BoundChecker::OnLeaveSector( const MPoint& sectorPt, MUID uidPlayer )
{
	m_setOutters.erase(uidPlayer);

	if (m_pLisener)
	{
		m_pLisener->OnChangeOutters(GetPlayerQtyInBoundary());
	}
	//if (!IsBoundary(sx, sy))
	//	return;

	//if (m_pLisener)
	//{
	//	m_pLisener->OnLeaveBoundSector(sx, sy, uidPlayer, GetPlayerQtyInBoundary());
	//}
}

int GFieldGrid::BoundChecker::GetPlayerQtyInBoundary()
{
	//int nPlayerQty = 0;
	//for (int x=m_rtBoundary.left; x<=m_rtBoundary.right; ++x)
	//{
	//	GetPlayerQtyInBoundaryImpl(x, m_rtBoundary.top, nPlayerQty);
	//	GetPlayerQtyInBoundaryImpl(x, m_rtBoundary.bottom, nPlayerQty);
	//}

	//for (int y=m_rtBoundary.top+1; y<=m_rtBoundary.bottom-1; ++y)
	//{
	//	GetPlayerQtyInBoundaryImpl(m_rtBoundary.left, y, nPlayerQty);
	//	GetPlayerQtyInBoundaryImpl(m_rtBoundary.right, y, nPlayerQty);
	//}

	//return nPlayerQty;
	return (int)m_setOutters.size();
}

void GFieldGrid::BoundChecker::GetPlayerQtyInBoundaryImpl(int sx, int sy, int& outnPlayerQty)
{
	//GFieldGrid::Sector* pSector = m_Selector.Lookup(sx, sy);
	//VALID(pSector);

	//outnPlayerQty += pSector->GetEntityQty(ETID_PLAYER);
}

bool GFieldGrid::BoundChecker::IsBoundary( const MPoint& sectorPt ) const
{
	if (m_rtBoundary.left >= sectorPt.x)
		return true;

	if (m_rtBoundary.right <= sectorPt.x)
		return true;

	if (m_rtBoundary.top >= sectorPt.y)
		return true;

	if (m_rtBoundary.bottom <= sectorPt.y)
		return true;

	return false;
}