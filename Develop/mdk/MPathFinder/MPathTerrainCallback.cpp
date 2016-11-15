#include "stdafx.h"
#include "MPathTerrainCallback.h"
#include "RHeightField.h"

MPathTerrainCallback::MPathTerrainCallback(RHeightField *pHeightField)
: m_pBindHeightField(pHeightField)
{
	_ASSERT(pHeightField);
}

MPathTerrainCallback::~MPathTerrainCallback()
{
	m_pBindHeightField = NULL;
}

float MPathTerrainCallback::getHeightAt(tSigned32 x, tSigned32 y)
{
	float fHeight = m_pBindHeightField->GetMinZ();
	m_pBindHeightField->GetHeightAt(&fHeight, NULL, (float)x, (float)y);

	return fHeight;
}

float MPathTerrainCallback::getHeightAtWithPrecision(tSigned32 x, tSigned32 y, float precisionX, float precisionY)
{
	float fHeight = m_pBindHeightField->GetMinZ();
	m_pBindHeightField->GetHeightAt(&fHeight, NULL, x + precisionX, y + precisionY);

	return fHeight;
}