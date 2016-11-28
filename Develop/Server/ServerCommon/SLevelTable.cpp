#include "stdafx.h"
#include "SLevelTable.h"
#include "MMath.h"

SLevelTable::SLevelTable()
{
	for (int level = 1; level <= MAX_LEVEL; level++)
	{
		int XP_required = CalcLevelUpRequiredXP(level);

		int index = GetIndex(level);
		m_RequiredXP[index] = XP_required;
	}
}

int SLevelTable::GetIndex( int nLevel )
{
	if (nLevel <= 0) return 0;
	if (nLevel > MAX_LEVEL) return MAX_LEVEL-1;
	return nLevel-1;
}

int SLevelTable::GetLevelUpRequiredXP( int nLevel )
{
	int index = GetIndex(nLevel);
	return m_RequiredXP[index];
}


/// 경험치 공식 함수
int SLevelTable::CalcLevelUpRequiredXP( int nCurrentLevel )
{
	// http://iworks2.maiet.net/wiki/sh:formulas:xp_to_level

	int nRetXP = 200 * (nCurrentLevel * nCurrentLevel) + (800 * nCurrentLevel);

	return nRetXP;
}
