#ifndef _GCALCULATOR_H
#define _GCALCULATOR_H

#include "GDef.h"
#include "GMath.h"
#include "GModGradeTable.h"
#include "GLevelFactorTable.h"
#include "GDamageCalculator.h"


class GEntityPlayer;
class GNPCInfo;

/// 공식 모음
class GCalculator
{
private:
	static GLevelFactorTable			m_LevelFactorTable;
	static GModGradeTable				m_ModGradeTable;
	static GDamageCalculator			m_DamageCalculator;

public:
	static int CalcMaxTP(int nLevel);

	// 공통
	static int CalcLevelFactor(int nActorLevel, int nTargetLevel);
	static float CalcModGrade(int nNPCGrade);
	static float CalcModGroup(int nGroupMembers);

	// 낙하 관련
	static int CalcFallingDamage(bool bIsFallenWater, float fFallingStartHeight, int nMaxHP);
	static int CalcFallingDebuffID(float fFallingHeight);

	// 공유필드 채널 관련
	static CHANNEL_STATUS CalcChannelStatus(int nCurPlayerCount, int nMaxPlayerCount);

private:
	static float CalcFallingDamageFactor(bool bIsFallenWater, float fFallingHeight);	
};


#endif