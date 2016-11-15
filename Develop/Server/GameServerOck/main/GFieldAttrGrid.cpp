#include "StdAfx.h"
#include "GFieldAttrGrid.h"
#include "GFieldInfo.h"
#include "GDef.h"
#include "CSFieldInfoHelper.h"

GFieldAttrGrid::GFieldAttrGrid(GFieldInfo* pFieldInfo)
: m_pFieldInfo(pFieldInfo)
, m_nGridXSize(0)
, m_nGridYSize(0)
, m_pCells(NULL)
{
}

GFieldAttrGrid::~GFieldAttrGrid(void)
{
	Fini();
}

void GFieldAttrGrid::Init()
{
	VALID(m_pFieldInfo);

	float fFieldWidth = m_pFieldInfo->MaxX() - m_pFieldInfo->MinX();
	float fFieldHeight = m_pFieldInfo->MaxY() - m_pFieldInfo->MinY();
	VALID(fFieldWidth > 0);
	VALID(fFieldHeight > 0);
	m_nGridXSize = (int)(fFieldWidth / FIELD_ATTR_CELL_SIZE) + 1;
	m_nGridYSize = (int)(fFieldHeight / FIELD_ATTR_CELL_SIZE) + 1;

	m_pCells = new Cell[m_nGridXSize * m_nGridYSize];
}

void GFieldAttrGrid::Fini()
{
	SAFE_DELETE_ARRAY(m_pCells);
	m_nGridXSize = 0;
	m_nGridYSize = 0;
}

MPoint GFieldAttrGrid::ConvertToGridPos(float fPosX, float fPosY)
{
	VALID_RET(m_pFieldInfo, MPoint());

	return CSFieldInfoHelper::ConvertToGridPos(fPosX, fPosY, m_pFieldInfo->MinX(), m_pFieldInfo->MinY());
}

GFieldAttrGrid::Cell* GFieldAttrGrid::FindCell( float fPosX, float fPosY )
{
	MPoint pt = ConvertToGridPos(fPosX, fPosY);

	return FindCellViaGridPos(pt.x, pt.y);
}

GFieldAttrGrid::Cell* GFieldAttrGrid::FindCellViaGridPos( int rx, int ry )
{
	if (!IsValidGridPos(rx, ry))
		return NULL;

	return &m_pCells[ry*m_nGridXSize+rx];
}

bool GFieldAttrGrid::IsValidGridPos( int rx, int ry )
{
	if (rx < 0)	
		return false;
	if (ry < 0)	
		return false;
	if (rx >= m_nGridXSize)	
		return false;
	if (ry >= m_nGridYSize)	
		return false;

	return true;
}

bool GFieldAttrGrid::GetCellsViaRect( MRect rect, vector<Cell*>& outCells )
{
	VALID_RET(rect.left <= rect.right, false);
	VALID_RET(rect.top <= rect.bottom, false);

	outCells.clear();

	 MPoint ptStart = ConvertToGridPos((float)rect.left, (float)rect.top);
	 MPoint ptEnd = ConvertToGridPos((float)rect.right, (float)rect.bottom);

	 for (int x = ptStart.x; x <= ptEnd.x; ++x)
	 {
		for (int y = ptStart.y; y <= ptEnd.y; ++y)
		{
			Cell* pCell = FindCellViaGridPos(x, y);
			if (!pCell)
				continue;

			outCells.push_back(pCell);
		}
	 }

	 return true;
}
