#include "stdafx.h"
#include "GExpCalculator.h"
#include "GTestForward.h"
#include "GFatigueSystem.h"
#include "CSDef_Fatigue.h"

SUITE(ExpCalculator)
{
	class FExpCalculator
	{
	public:
		static const int m_MaxExpectedData = 21;
		static const int m_nNPCGrade = 3;
		static const int m_nMaxLevel = 50;

		static int m_nExpectedXP[m_nMaxLevel][m_MaxExpectedData];

	};


// 소울헌트-각종수식시뮬레이션.xls - "경험치 획득"
int FExpCalculator::m_nExpectedXP[FExpCalculator::m_nMaxLevel][FExpCalculator::m_MaxExpectedData] =
{ {0,0,0,0,0,0,0,0,0,0,30,35,44,54,65,77,90,97,105,112,120},
{0,0,0,0,0,0,0,0,0,30,35,40,49,60,71,84,97,105,112,120,127},
{0,0,0,0,0,0,0,0,24,35,40,45,55,66,78,91,105,112,120,127,135},
{0,0,0,0,0,0,0,15,28,40,45,50,60,72,84,98,112,120,127,135,142},
{0,0,0,0,0,0,6,17,32,45,50,55,66,78,91,105,120,127,135,142,150},
{0,0,0,0,0,3,7,20,36,50,55,60,71,84,97,112,127,135,142,150,157},
{0,0,0,0,0,3,8,22,40,55,60,65,77,90,104,119,135,142,150,157,165},
{0,0,0,0,0,4,9,25,44,60,65,70,82,96,110,126,142,150,157,165,172},
{0,0,0,0,0,4,10,27,48,65,70,75,88,102,117,133,150,157,165,172,180},
{0,0,0,0,0,5,11,30,52,70,75,80,93,108,123,140,157,165,172,180,187},
{0,0,0,0,0,5,12,32,56,75,80,85,99,114,130,147,165,172,180,187,195},
{0,0,0,0,0,6,13,35,60,80,85,90,104,120,136,154,172,180,187,195,202},
{0,0,0,0,0,6,14,37,64,85,90,95,110,126,143,161,180,187,195,202,210},
{0,0,0,0,0,7,15,40,68,90,95,100,115,132,149,168,187,195,202,210,217},
{0,0,0,0,0,7,16,42,72,95,100,105,121,138,156,175,195,202,210,217,225},
{0,0,0,0,0,8,17,45,76,100,105,110,126,144,162,182,202,210,217,225,232},
{0,0,0,0,0,8,18,47,80,105,110,115,132,150,169,189,210,217,225,232,240},
{0,0,0,0,0,9,19,50,84,110,115,120,137,156,175,196,217,225,232,240,247},
{0,0,0,0,0,9,20,52,88,115,120,125,143,162,182,203,225,232,240,247,255},
{0,0,0,0,0,10,21,55,92,120,125,130,148,168,188,210,232,240,247,255,262},
{0,0,0,0,0,10,22,57,96,125,130,135,154,174,195,217,240,247,255,262,270},
{0,0,0,0,0,11,23,60,100,130,135,140,159,180,201,224,247,255,262,270,277},
{0,0,0,0,0,11,24,62,104,135,140,145,165,186,208,231,255,262,270,277,285},
{0,0,0,0,0,12,25,65,108,140,145,150,170,192,214,238,262,270,277,285,292},
{0,0,0,0,0,12,26,67,112,145,150,155,176,198,221,245,270,277,285,292,300},
{0,0,0,0,0,13,27,70,116,150,155,160,181,204,227,252,277,285,292,300,307},
{0,0,0,0,0,13,28,72,120,155,160,165,187,210,234,259,285,292,300,307,315},
{0,0,0,0,0,14,29,75,124,160,165,170,192,216,240,266,292,300,307,315,322},
{0,0,0,0,0,14,30,77,128,165,170,175,198,222,247,273,300,307,315,322,330},
{0,0,0,0,0,15,31,80,132,170,175,180,203,228,253,280,307,315,322,330,337},
{0,0,0,0,0,15,32,82,136,175,180,185,209,234,260,287,315,322,330,337,345},
{0,0,0,0,0,16,33,85,140,180,185,190,214,240,266,294,322,330,337,345,352},
{0,0,0,0,0,16,34,87,144,185,190,195,220,246,273,301,330,337,345,352,360},
{0,0,0,0,0,17,35,90,148,190,195,200,225,252,279,308,337,345,352,360,367},
{0,0,0,0,0,17,36,92,152,195,200,205,231,258,286,315,345,352,360,367,375},
{0,0,0,0,0,18,37,95,156,200,205,210,236,264,292,322,352,360,367,375,382},
{0,0,0,0,0,18,38,97,160,205,210,215,242,270,299,329,360,367,375,382,390},
{0,0,0,0,0,19,39,100,164,210,215,220,247,276,305,336,367,375,382,390,397},
{0,0,0,0,0,19,40,102,168,215,220,225,253,282,312,343,375,382,390,397,405},
{0,0,0,0,0,20,41,105,172,220,225,230,258,288,318,350,382,390,397,405,412},
{0,0,0,0,0,20,42,107,176,225,230,235,264,294,325,357,390,397,405,412,420},
{0,0,0,0,0,21,43,110,180,230,235,240,269,300,331,364,397,405,412,420,427},
{0,0,0,0,0,21,44,112,184,235,240,245,275,306,338,371,405,412,420,427,435},
{0,0,0,0,0,22,45,115,188,240,245,250,280,312,344,378,412,420,427,435,442},
{0,0,0,0,0,22,46,117,192,245,250,255,286,318,351,385,420,427,435,442,450},
{0,0,0,0,0,23,47,120,196,250,255,260,291,324,357,392,427,435,442,450,457},
{0,0,0,0,0,23,48,122,200,255,260,265,297,330,364,399,435,442,450,457,465},
{0,0,0,0,0,24,49,125,204,260,265,270,302,336,370,406,442,450,457,465,472},
{0,0,0,0,0,24,50,127,208,265,270,275,308,342,377,413,450,457,465,472,480},
{0,0,0,0,0,25,51,130,212,270,275,280,313,348,383,420,457,465,472,480,487}
};

	TEST_FIXTURE(FExpCalculator, TestCalcXPFromNPC)
	{
		for (int nPCLevel = 1; nPCLevel <= m_nMaxLevel; nPCLevel++)
		{
			int idx = 0;
			for (int nNPCLevel = nPCLevel-10; nNPCLevel <= nPCLevel+10; nNPCLevel++)
			{
				if (nNPCLevel >= 1) 
				{
					ASSERT_EQUAL(m_nExpectedXP[nPCLevel-1][idx],
						GExpCalculator::CalcXPFromNPC(nPCLevel, nNPCLevel, m_nNPCGrade));
				}

				idx++;
			}

		}
	}

	class FExpCalculator_LevelUp : public FExpCalculator
	{
	public:
	};

// 소울헌트-각종수식시뮬레이션.xls - "레벨업"
// 1 lv - 1000(다음 레벨업 필요 경험치), 2lv - 2400 , 3lv - 4200 , 4lv - 6400 , 5lv - 9000 
// 46lv - 460000 , 47lv - 479400 , 48lv - 499200 , 49lv - 519400 , 50lv - 540000

	TEST_FIXTURE(FExpCalculator_LevelUp, TestExpCalculator_CheckLevelUp_NoLevelUp)
	{
		int nNewLevel;
		int nCurrentLevel = 1;
		int nExp = 0;

		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK_EQUAL(nNewLevel, nCurrentLevel);

		for (int i = 0; i < 1000; i++)
		{
			nCurrentLevel = 1; nExp = i;
			nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
			ASSERT_EQUAL(nCurrentLevel, nNewLevel);
		}

		nCurrentLevel = 2; nExp = 1100;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK_EQUAL(nNewLevel, nCurrentLevel);

		nCurrentLevel = 5; nExp = 8999;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK_EQUAL(nNewLevel, nCurrentLevel);

		// 최대 레벨이면 레벨업하지 않는다.
		nCurrentLevel = MAX_LEVEL; nExp = INT_MAX;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK_EQUAL(nNewLevel, nCurrentLevel);
	}

	TEST_FIXTURE(FExpCalculator_LevelUp, TestExpCalculator_CheckLevelUp_LevelUp_Once)
	{
		int nNewLevel, nNewExp;
		int nCurrentLevel = 1;
		int nExp = 1000;

		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(2,	nNewLevel);
		CHECK_EQUAL(0,	nNewExp);


		nCurrentLevel = 1;	nExp = 1500;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(2,	nNewLevel);
		CHECK_EQUAL(500,	nNewExp);

		// 49lv - 519400
		nCurrentLevel = 49;	nExp = 519400;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(50,	nNewLevel);
		CHECK_EQUAL(0,	nNewExp);

		nCurrentLevel = 49;	nExp = 519400 + 50509;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(50,	nNewLevel);
		CHECK_EQUAL(50509,	nNewExp);

	}

	TEST_FIXTURE(FExpCalculator_LevelUp, TestExpCalculator_CheckLevelUp_LevelUp_Once_ForCBT2)
	{
		int nNewLevel;
		int nCurrentLevel = 1;
		int nExp = 1000;

		nCurrentLevel = CBT2_LIMIT_MAX_LEVEL;	nExp = 15000000;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp);
		CHECK_EQUAL(nNewLevel,	nCurrentLevel);
	}

	TEST_FIXTURE(FExpCalculator_LevelUp, TestExpCalculator_CheckLevelUp_LevelUp_Manytimes)
	{
		return;

		int nNewLevel, nNewExp;
		int nCurrentLevel = 1;
		int nExp = 1000 + 2400;

		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(3,	nNewLevel);
		CHECK_EQUAL(0,	nNewExp);

		nCurrentLevel = 1;	nExp = 1000 + 2400 + 4200 + 11;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(4,	nNewLevel);
		CHECK_EQUAL(11,	nNewExp);


		nCurrentLevel = 46;	nExp = 460000 + 479400 + 1;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(48,	nNewLevel);
		CHECK_EQUAL(1,	nNewExp);

		nCurrentLevel = 1;	nExp = INT_MAX;
		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp);
		nNewExp = GExpCalculator::CalcNewExp(nCurrentLevel, nExp);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(MAX_LEVEL,	nNewLevel);
	}


	TEST_FIXTURE(FExpCalculator_LevelUp, TestExpCalculator_CheckLevelUp_LevelUp_Manytimes_Negative)
	{
		int nNewLevel;
		int nCurrentLevel = MAX_LEVEL - 1;
		int nExp = INT_MAX;

		nNewLevel = GExpCalculator::CalcNewLevel(nCurrentLevel, nExp, MAX_LEVEL);
		CHECK(nNewLevel > nCurrentLevel);
		CHECK_EQUAL(MAX_LEVEL,	nNewLevel);
	}

	TEST_FIXTURE(FExpCalculator_LevelUp, TestCalcExpPercent)
	{
		for (int nLevel = 1; nLevel < MAX_LEVEL; nLevel++)
		{
			int nRequiredXP = GExpCalculator::GetLevelUpRequiredXP(nLevel);

			ASSERT_EQUAL(0, GExpCalculator::CalcExpPercent(0, nLevel));
			ASSERT_EQUAL(50, GExpCalculator::CalcExpPercent(nRequiredXP / 2, nLevel));
			ASSERT_EQUAL(100, GExpCalculator::CalcExpPercent(nRequiredXP, nLevel));
		}

		// 만랩은 무조건 0
		ASSERT_EQUAL(0, GExpCalculator::CalcExpPercent(0, MAX_LEVEL));
		ASSERT_EQUAL(0, GExpCalculator::CalcExpPercent(INT_MAX, MAX_LEVEL));
	}

	TEST(CalcBeneficiaryCountFactor)
	{
		int nCotributorCount=0;

		nCotributorCount = 1;
		CHECK_EQUAL(1.0f, GExpCalculator::CalcBeneficiaryCountFactor(nCotributorCount));

		nCotributorCount = 2;
		CHECK_EQUAL(1.0f + nCotributorCount * 0.1f, GExpCalculator::CalcBeneficiaryCountFactor(nCotributorCount));

		nCotributorCount = 3;
		CHECK_EQUAL(1.0f + nCotributorCount * 0.1f, GExpCalculator::CalcBeneficiaryCountFactor(nCotributorCount));
	}

	TEST(CalcFatigueFactor)
	{
		short nFatigueQuantity; 
		
		nFatigueQuantity = WORST_FATIGUE;
		CHECK_EQUAL(1, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = WORST_FATIGUE + 1;
		CHECK_EQUAL(1, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = BAD_FATIGUE;
		CHECK_EQUAL(1, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = BAD_FATIGUE + 1;
		CHECK_EQUAL(1.5f, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = NORMAL_FATIGUE;
		CHECK_EQUAL(1.5f, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = NORMAL_FATIGUE + 1;
		CHECK_EQUAL(1.8f, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = GOOD_FATIGUE;
		CHECK_EQUAL(1.8f, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = GOOD_FATIGUE + 1;
		CHECK_EQUAL(2, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));

		nFatigueQuantity = BEST_FATIGUE;
		CHECK_EQUAL(2, GExpCalculator::CalcFatigueFactor(nFatigueQuantity));
	}
}