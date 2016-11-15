#include "stdafx.h"
#include "GLevelFactorTable.h"

GLevelFactorTable::GLevelFactorTable()
{
	for (int diff = LEVELFACTOR_DIFF_MIN; diff <= LEVELFACTOR_DIFF_MAX; diff++)
	{
		if (diff >= 0)
		{
			m_nValues[diff-LEVELFACTOR_DIFF_MIN] = min(5, max(6 - diff, 0));
		}
		else
		{
			m_nValues[diff-LEVELFACTOR_DIFF_MIN] = min(4 - diff, 10);
		}
	}
}

int GLevelFactorTable::GetFactor( int nActorLevel, int nTargetLevel )
{
	int diff = nActorLevel - nTargetLevel;
	return GetFactor(diff);
}

int GLevelFactorTable::GetFactor( int nLevelDiff )
{
	int index = nLevelDiff - LEVELFACTOR_DIFF_MIN;

	if (index >= LEVELFACTOR_COUNT) index = LEVELFACTOR_COUNT - 1;
	else if (index < 0) index = 0;

	return m_nValues[index];
}
