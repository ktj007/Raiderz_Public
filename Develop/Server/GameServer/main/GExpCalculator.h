#pragma once

#include "GDef.h"
#include "GModGradeTable.h"
#include "GLevelFactorTable.h"
#include "GNpcXPTable.h"
#include "SLevelTable.h"

// 2차 클베 기준 최고레벨
#define CBT2_LIMIT_MAX_LEVEL 30

class GExpCalculator
{
protected:
	static SLevelTable					m_LevelTable;
	static GNpcXPTable					m_NpcXPTable;

	static float CalcXPFactorLevel(int nLevelFactor);
public:
	static int CalcXPFromNPC(int nPCLevel, int nNPCLevel, int nNPCGrade);
	static float CalcBeneficiaryCountFactor(int BeneficiaryCount);
	static float CalcLevelNormalizeFactor(int nLevel, int nSumLevel, int nSameServerBeneficiaryCount, int nAllBeneficiaryCount);
	static float CalcFatigueFactor(short nFatigueQuantity);
	
	static float CalcExpPercent(int nExp, int nCurrentLevel);
	static int CalcNewLevel(int nCurrentLevel, int nCurrentExp, const int nMaxLevel = CBT2_LIMIT_MAX_LEVEL);
	static int CalcNewExp(int nCurrentLevel, int nCurrentExp, const int nMaxLevel = CBT2_LIMIT_MAX_LEVEL);

	static int GetLevelUpRequiredXP(int nCurrentLevel);
};