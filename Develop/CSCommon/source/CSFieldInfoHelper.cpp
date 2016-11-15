#include "stdafx.h"
#include "CSFieldInfoHelper.h"
#include "CSDef.h"

MPoint CSFieldInfoHelper::ConvertToGridPos( const float fPosX, const float fPosY, const float fFieldMinX, const float fFieldMinY )
{
	int x = (int)((fPosX - fFieldMinX) / FIELD_ATTR_CELL_SIZE);
	int y = (int)((fPosY - fFieldMinY) / FIELD_ATTR_CELL_SIZE);

	return MPoint(x, y);
}