#include "stdafx.h"
#include "GDamageMODTable.h"

GPlayerDMODTable::GPlayerDMODTable()
{
	GLevelFactorTable level_factor_table;
	for (int level_diff = LEVELFACTOR_DIFF_MIN; level_diff <= LEVELFACTOR_DIFF_MAX; level_diff++)
	{
		int index = GetIndex(level_diff);
		int lf = level_factor_table.GetFactor(level_diff);

		if (lf >= 5)
		{
			m_fValues[index] = float(2 - 0.2 * lf);
		}
		else
		{
			m_fValues[index] = float(1.2 - 0.05 * lf);
		}
	}
}

float GPlayerDMODTable::GetValue( int nPCLevel, int nNPCLevel )
{
	int nLevelDiff = nPCLevel - nNPCLevel;
	return GetValue(nLevelDiff);
}

float GPlayerDMODTable::GetValue( int nLevelDiff )
{
	int index = GetIndex(nLevelDiff);
	return m_fValues[index];
}

GNPC_DMODTable::GNPC_DMODTable()
{
	GLevelFactorTable level_factor_table;
	for (int level_diff = LEVELFACTOR_DIFF_MIN; level_diff <= LEVELFACTOR_DIFF_MAX; level_diff++)
	{
		int index = GetIndex(level_diff);
		int lf = level_factor_table.GetFactor(level_diff);

		if (lf >= 5)
		{
			m_fValues[index] = float(1 + 0.1 * (lf - 5));
		}
		else
		{
			m_fValues[index] = float(1 + 0.05 * (lf - 5));
		}
	}
}