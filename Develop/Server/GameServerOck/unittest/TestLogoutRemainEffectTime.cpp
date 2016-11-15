#include "stdafx.h"
#include "GEffRemainTimeSqlBuilder.h"


SUITE(TEST_EFF_REMAIN_TIME_BUILDER)
{
	TEST(SET_EFF_REMAIN_TIME_FAIL_LOWFLOW)
	{
		GEffRemainTimeSqlBuilder ef;

		CHECK(!ef.Set(-1, 0, 0, 0, 0));
	}

	TEST(SET_EFF_REMAIN_TIME_FAIL_OVERFLOW)
	{
		GEffRemainTimeSqlBuilder ef;

		CHECK(!ef.Set(GEffRemainTimeSqlBuilder::MAX_EFF_REMAIN_COUNT + 1, 0, 0, 0, 0));
	}

	TEST(SET_EFF_REMAIN_TIME_SUCCESS)
	{
		class test_eff_remain_time
		{
		public :
			void Set(const int ORD_NUM, const int EFF_TYPE, const int ID, const int EXPI_CHAR_PTM)
			{
				m_ORD_NUM		= ORD_NUM;
				m_EFF_TYPE		= EFF_TYPE;
				m_ID			= ID;
				m_EXPI_CHAR_PTM = EXPI_CHAR_PTM;
			}

			int m_ORD_NUM;
			int m_EFF_TYPE;
			int m_ID;
			int m_EXPI_CHAR_PTM;
		};

		class MockEffRemainTimeBuilder : public GEffRemainTimeSqlBuilder
		{
		public :
			using GEffRemainTimeSqlBuilder::m_EffRemainTime;
		};

		MockEffRemainTimeBuilder	ef;
		test_eff_remain_time		tef[GEffRemainTimeSqlBuilder::MAX_EFF_REMAIN_COUNT];

		for (int i = 0; i < GEffRemainTimeSqlBuilder::MAX_EFF_REMAIN_COUNT; ++i)
		{
			tef[i].Set(i, i + 1, i + 2, i + 3);
			ef.Set(i, i, i + 1, i + 2, i + 3);
		}

		for (int i = 0; i < GEffRemainTimeSqlBuilder::MAX_EFF_REMAIN_COUNT; ++i)
		{
			CHECK_EQUAL(tef[i].m_ORD_NUM, ef.m_EffRemainTime[i].m_ORD_NUM);
			CHECK_EQUAL(tef[i].m_EFF_TYPE, ef.m_EffRemainTime[i].m_EFF_TYPE);
			CHECK_EQUAL(tef[i].m_ID, ef.m_EffRemainTime[i].m_ID);
			CHECK_EQUAL(tef[i].m_EXPI_CHAR_PTM, ef.m_EffRemainTime[i].m_EXPI_CHAR_PTM);
		}
	}
}