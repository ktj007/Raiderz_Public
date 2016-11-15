#pragma once

#include "MPathFinder.h"

class RHeightField;
class MPathTerrainCallback : public iTerrainCallBack
{
private:
	RHeightField *m_pBindHeightField;
public:
	MPathTerrainCallback(RHeightField *pHeightField);
	virtual ~MPathTerrainCallback();

	virtual float getHeightAt(tSigned32 x, tSigned32 y);
	virtual float getHeightAtWithPrecision(tSigned32 x, tSigned32 y, float precisionX, float precisionY);

};