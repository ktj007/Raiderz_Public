#ifndef _GDAMAGE_MOD_TABLE_H
#define _GDAMAGE_MOD_TABLE_H

#include "GDef.h"
#include "GLevelFactorTable.h"

class GPlayerDMODTable
{
private:
	float m_fValues[LEVELFACTOR_COUNT];
	int GetIndex(int nLevelDiff)
	{
		int index = nLevelDiff - LEVELFACTOR_DIFF_MIN;
		if (index >= LEVELFACTOR_COUNT) index = LEVELFACTOR_COUNT - 1;
		else if (index < 0) index = 0;
		return index;
	}
public:
	GPlayerDMODTable();
	float GetValue(int nPCLevel, int nNPCLevel);
	float GetValue(int nLevelDiff);
};

class GNPC_DMODTable
{
private:
	float m_fValues[LEVELFACTOR_COUNT];
	int GetIndex(int nLevelDiff)
	{
		int index = nLevelDiff - LEVELFACTOR_DIFF_MIN;
		if (index >= LEVELFACTOR_COUNT) index = LEVELFACTOR_COUNT - 1;
		else if (index < 0) index = 0;
		return index;
	}
public:
	GNPC_DMODTable();
	float GetValue(int nPCLevel, int nNPCLevel)
	{
		int nLevelDiff = nPCLevel - nNPCLevel;
		return GetValue(nLevelDiff);
	}
	float GetValue(int nLevelDiff)
	{
		int index = GetIndex(nLevelDiff);
		return m_fValues[index];
	}
};


#endif