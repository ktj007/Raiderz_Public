#include "stdafx.h"
#include "CSBasicAttributeCalculator.h"
#include "CSChrInfo.h"

int CSBasicAttributeCalculator::CalcMaxHP(int nLevel, int nCON)
{
	if (nLevel <= 0) nLevel = 1;
	else if (nLevel > MAX_LEVEL) nLevel = MAX_LEVEL;

	int nHP = BASE_HP_RACE_HUMAN + (10 * (nLevel - 1)) + max(nCON - 25, 0) * 10;

	return nHP;
}


int CSBasicAttributeCalculator::CalcMaxEN(int nLevel, int nINT)
{
	if (nLevel <= 0) nLevel = 1;
	else if (nLevel > MAX_LEVEL) nLevel = MAX_LEVEL;

	int nEN = BASE_EN_RACE_HUMAN + (2 * (nLevel - 1)) + max(nINT - 20, 0) * 5;

	return nEN;
}

int CSBasicAttributeCalculator::CalcMaxSP(int nLevel, int nDEX)
{
	return 100; // 100¿∏∑Œ ∞Ì¡§µ 
	//if (nLevel <= 0) nLevel = 1;
	//else if (nLevel > MAX_LEVEL) nLevel = MAX_LEVEL;

	//int nSP = BASE_SP_RACE_HUMAN + (2 * (nLevel - 1)) + max(nDEX - 20, 0) * 5;

	//return nSP;
}
