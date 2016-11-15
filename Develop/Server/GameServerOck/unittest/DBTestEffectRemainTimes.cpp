#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"


SUITE(TEST_EFF_REAMIN_TIME)
{
	class dbt_test_eff_remain_time
	{
	public :
		class dbt_eff_remain_time
		{
		public :
			void Set(const int ORD_NUM, const int EFF_TYPE, const int ID, const int EXPI_CHAR_PTM)
			{
				m_ORD_NUM		= ORD_NUM;
				m_EFF_TYPE		= EFF_TYPE;
				m_ID			= ID;
				m_EXPI_CHAR_PTM = EXPI_CHAR_PTM;
			}

		public :
			int m_ORD_NUM;
			int m_EFF_TYPE;
			int m_ID;			
			int m_EXPI_CHAR_PTM;
		};

		void SetTestData()
		{
			for (int i = 0; i < 10; ++i)
			{
				m_EffRemainTime[i].Set(i, i / 5, i + 1, i + 10);
			}
		}

		dbt_eff_remain_time m_EffRemainTime[10];
	};

	GAMEDB_TEST(USP_RZ_EFF_REMAIN_TIME_INSERT)
	{
		const int64					GSN = 1;
		const int64					CSN = 2;
		dbt_test_eff_remain_time	ert;

		ert.SetTestData();

		CString strSQL;
		strSQL.Format(L"{CALL dbo.USP_RZ_EFF_REMAIN_TIME_INSERT (%d, %I64d, %I64d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d\
					   , %d, %d, %d, %d)}"
					   , GConfig::m_nMyWorldID
					   , GSN, CSN
					   , ert.m_EffRemainTime[0].m_ORD_NUM, ert.m_EffRemainTime[0].m_EFF_TYPE, ert.m_EffRemainTime[0].m_ID, ert.m_EffRemainTime[0].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[1].m_ORD_NUM, ert.m_EffRemainTime[1].m_EFF_TYPE, ert.m_EffRemainTime[1].m_ID, ert.m_EffRemainTime[1].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[2].m_ORD_NUM, ert.m_EffRemainTime[2].m_EFF_TYPE, ert.m_EffRemainTime[2].m_ID, ert.m_EffRemainTime[2].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[3].m_ORD_NUM, ert.m_EffRemainTime[3].m_EFF_TYPE, ert.m_EffRemainTime[3].m_ID, ert.m_EffRemainTime[3].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[4].m_ORD_NUM, ert.m_EffRemainTime[4].m_EFF_TYPE, ert.m_EffRemainTime[4].m_ID, ert.m_EffRemainTime[4].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[5].m_ORD_NUM, ert.m_EffRemainTime[5].m_EFF_TYPE, ert.m_EffRemainTime[5].m_ID, ert.m_EffRemainTime[5].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[6].m_ORD_NUM, ert.m_EffRemainTime[6].m_EFF_TYPE, ert.m_EffRemainTime[6].m_ID, ert.m_EffRemainTime[6].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[7].m_ORD_NUM, ert.m_EffRemainTime[7].m_EFF_TYPE, ert.m_EffRemainTime[7].m_ID, ert.m_EffRemainTime[7].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[8].m_ORD_NUM, ert.m_EffRemainTime[8].m_EFF_TYPE, ert.m_EffRemainTime[8].m_ID, ert.m_EffRemainTime[8].m_EXPI_CHAR_PTM
					   , ert.m_EffRemainTime[9].m_ORD_NUM, ert.m_EffRemainTime[9].m_EFF_TYPE, ert.m_EffRemainTime[9].m_ID, ert.m_EffRemainTime[9].m_EXPI_CHAR_PTM);
		UTestDB.Execute(strSQL);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT ORD_NUM, EFF_TYPE, ID, EXPI_CHAR_PTM FROM dbo.RZ_EFF_REMAIN_TIME WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d ORDER BY ORD_NUM;", GSN, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		for (int i = 0; !rs.IsEOF(); rs.MoveNext())
		{
			CHECK_EQUAL(ert.m_EffRemainTime[i].m_ORD_NUM, rs.Field(L"ORD_NUM").AsInt());
			CHECK_EQUAL(ert.m_EffRemainTime[i].m_EFF_TYPE, rs.Field(L"EFF_TYPE").AsInt());
			CHECK_EQUAL(ert.m_EffRemainTime[i].m_ID, rs.Field(L"ID").AsInt());
			CHECK_EQUAL(ert.m_EffRemainTime[i].m_EXPI_CHAR_PTM, rs.Field(L"EXPI_CHAR_PTM").AsInt());

			++i;
		}
		CHECK_EQUAL(10, rs.GetFetchedRowCount());
		rs.Close();
	}
}