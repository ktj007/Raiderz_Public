#include "stdafx.h"
#include "GCalculator.h"
#include "GEntityPlayer.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GConst.h"
#include "GMath.h"
#include "SDef.h"


GLevelFactorTable GCalculator::m_LevelFactorTable;
GModGradeTable GCalculator::m_ModGradeTable;
GDamageCalculator GCalculator::m_DamageCalculator;



////////////////////////////////////////////////////////



int GCalculator::CalcMaxTP(int nLevel)
{
	return nLevel*1;
}






int GCalculator::CalcLevelFactor( int nActorLevel, int nTargetLevel )
{
	return m_LevelFactorTable.GetFactor(nActorLevel, nTargetLevel);
}

float GCalculator::CalcModGrade( int nNPCGrade )
{
	return m_ModGradeTable.GetMODGrade(nNPCGrade);
}


float GCalculator::CalcModGroup( int nGroupMembers )
{
	return (1 + (0.1f * (nGroupMembers-1)));
}

int GCalculator::CalcFallingDamage(bool bIsFallenWater, float fFallingStartHeight, int nMaxHP)
{
	float fDamageFactor = GCalculator::CalcFallingDamageFactor(bIsFallenWater, fFallingStartHeight);
	if (fDamageFactor < 0 || 1 < fDamageFactor) return 0;

	return (int)GMath::Round(nMaxHP*fDamageFactor);
}

int GCalculator::CalcFallingDebuffID(float fFallingHeight)
{
	int nDebuffID = 0;

	if (600 <= fFallingHeight && fFallingHeight < 1600)		// 6m ~ 15m
	{
		nDebuffID = GConst::FALLING_DEBUFF_LV1_ID;
	}
	else if (1600 <= fFallingHeight && fFallingHeight < 3500)	// 16m ~ 34m
	{
		nDebuffID = GConst::FALLING_DEBUFF_LV2_ID;
	}

	return nDebuffID;
}

float GCalculator::CalcFallingDamageFactor(bool bIsFallenWater, float fFallingHeight)
{
	// 물 위에 떨어졌을 때
	if (bIsFallenWater)
	{
		if (fFallingHeight < 1600) return 0;						// ~15m			: 0
		else if (1600 <= fFallingHeight && fFallingHeight < 3600)	// 16m ~ 35m	: 1m당 0.2씩
		{
			int nFallingMeter = (int)((fFallingHeight-1500)*0.01);
			return (float)(nFallingMeter*0.02);
		}
		else if (3600 <= fFallingHeight && fFallingHeight < 5500)	// 36m ~ 54m	: 1m당 0.3씩
		{
			int nFallingMeter = (int)((fFallingHeight-3500)*0.01);
			return (float)(0.4f + (nFallingMeter*0.03));
		}
		else if (5500 <= fFallingHeight) return 1;					// 55m ~		: 1
	}
	// 땅 위에 떨어졌을 때
	else
	{
		if (fFallingHeight < 600) return 0;							// ~5m			: 0
		else if (600 <= fFallingHeight && fFallingHeight < 1600)	// 6m ~ 15m		: 1m당 0.2씩
		{
			int nFallingMeter = (int)((fFallingHeight-500)*0.01);
			return (float)(nFallingMeter*0.02);
		}
		else if (1600 <= fFallingHeight && fFallingHeight < 3500)	// 16m ~ 34m	: 1m당 0.4씩
		{
			int nFallingMeter = (int)((fFallingHeight-1500)*0.01);
			return (float)(0.2f + (nFallingMeter*0.04));
		}
		else if (3500 <= fFallingHeight) return 1;					// 35m ~		: 1
	}

	return 0;
}

CHANNEL_STATUS GCalculator::CalcChannelStatus(int nCurPlayerCount, int nMaxPlayerCount)
{
	float fRatio = (float)nCurPlayerCount / (float)nMaxPlayerCount * 100.0f;
	if (fRatio < 80)
		return CHANNEL_NORMAL;
	else if (fRatio < 95)
		return CHANNEL_CROWDED;

	return CHANNEL_FULL;
}