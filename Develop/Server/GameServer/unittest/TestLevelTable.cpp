#include "stdafx.h"
#include "SLevelTable.h"

SUITE(LevelTable)
{
	class TestLevelTable : public SLevelTable
	{
	public:
		using SLevelTable::CalcLevelUpRequiredXP;
		using SLevelTable::GetIndex;
	};


	class FLevelTable
	{
	public:
		FLevelTable()
		{
		}
		const static int m_nMaxLevel = 50;
		static int m_nExpectedRequiredXP[m_nMaxLevel+1];
	};

// 소울헌트-각종수식시뮬레이션.xls - "레벨업"
int FLevelTable::m_nExpectedRequiredXP[FLevelTable::m_nMaxLevel+1] = 
{ 0
, 1000 
, 2400 
, 4200 
, 6400 
, 9000 
, 12000 
, 15400 
, 19200 
, 23400 
, 28000 
, 33000 
, 38400 
, 44200 
, 50400 
, 57000 
, 64000 
, 71400 
, 79200 
, 87400 
, 96000 
, 105000 
, 114400 
, 124200 
, 134400 
, 145000 
, 156000 
, 167400 
, 179200 
, 191400 
, 204000 
, 217000 
, 230400 
, 244200 
, 258400 
, 273000 
, 288000 
, 303400 
, 319200 
, 335400 
, 352000 
, 369000 
, 386400 
, 404200 
, 422400 
, 441000 
, 460000 
, 479400 
, 499200 
, 519400 
, 540000
};

	TEST(TestLevelTable_GetIndex)
	{
		TestLevelTable levelTable;

		CHECK_EQUAL(0, levelTable.GetIndex(0));

		CHECK_EQUAL(0, levelTable.GetIndex(1));
		CHECK_EQUAL(1, levelTable.GetIndex(2));

		CHECK_EQUAL(MAX_LEVEL-1, levelTable.GetIndex(MAX_LEVEL));
		CHECK_EQUAL(MAX_LEVEL-1, levelTable.GetIndex(MAX_LEVEL + 555));
	}

	TEST_FIXTURE(FLevelTable, TestLevelTable_CalcLevelUpRequiredXP)
	{
		TestLevelTable levelTable;

		for (int nLevel = 1; nLevel <= m_nMaxLevel; nLevel++)
		{
			CHECK_EQUAL(m_nExpectedRequiredXP[nLevel], levelTable.CalcLevelUpRequiredXP(nLevel));
		}
	}

	TEST_FIXTURE(FLevelTable, TestLevelTable_GetRequiredXP)
	{
		TestLevelTable levelTable;

		for (int nLevel = 1; nLevel <= m_nMaxLevel; nLevel++)
		{
			CHECK_EQUAL(m_nExpectedRequiredXP[nLevel], levelTable.GetLevelUpRequiredXP(nLevel));
		}
	}

}