#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"


SUITE(DBTestConnection)
{
	// 필요해지면 만들면서 테스트코드도 만들자.
	// 

	LOGDB_TEST(USP_RZ_CONN_LOG_INSERT)
	{
		const int64		GSN		= 1;
		const int		WORLD	= 2;
		const wstring strIP(L"192.168.0.1");

		UTestDB.Execute(L"{CALL dbo.USP_RZ_CONN_LOG (%I64d, %d, '%s')}"
			, GSN, WORLD, strIP.c_str());

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_ID, CONN_IP FROM dbo.RZ_CONN_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CODE = 101;"
			, GSN, WORLD);
		CHECK_EQUAL(1, rsLog.GetFetchedRowCount());
		CHECK(rsLog.Field(L"CHAR_ID").IsNull());
		CHECK(strIP == rsLog.Field(L"CONN_IP").AsWString());		
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_CHAR_SELECT_LOG_INSERT)
	{
		const int64		GSN			= 1;
		const int		WORLD		= 2;
		const int64		CSN			= 3;
		const int		CHAR_PTM	= 4;
		const int		CUR_MONEY	= 5;
		const int		FIELD_ID	= 6;
		
		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_SELECT_LOG (%I64d, %d, %I64d, %d, %d, %d)}", GSN, WORLD, CSN, CHAR_PTM, CUR_MONEY, FIELD_ID);

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_PTM, CUR_MONEY, FIELD_ID FROM dbo.RZ_CONN_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 102;"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(1, rsLog.GetFetchedRowCount());
		CHECK_EQUAL(CHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(CUR_MONEY, rsLog.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(FIELD_ID, rsLog.Field(L"FIELD_ID").AsInt());
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_DISCONN_LOG_INSERT)
	{
		const int64 GSN				= 1;
		const int	WORLD			= 2;
		const int64	CSN				= 3;
		const int	CHAR_PTM		= 4;
		const int	DELTA_CHAR_PTM	= 5;
		const int	CUR_MONEY		= 6;
		const int	SUM_DELTA_MONEY = 7;
		const int	FIELD_ID		= 8;
		

		UTestDB.Execute(L"{CALL dbo.USP_RZ_DISCONN_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d)}"
			, GSN, WORLD, CSN, CHAR_PTM, DELTA_CHAR_PTM, CUR_MONEY, SUM_DELTA_MONEY, FIELD_ID);

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, CUR_MONEY, SUM_DELTA_MONEY, FIELD_ID FROM dbo.RZ_CONN_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 103"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(CHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(DELTA_CHAR_PTM, rsLog.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(CUR_MONEY, rsLog.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(SUM_DELTA_MONEY, rsLog.Field(L"SUM_DELTA_MONEY").AsInt());
		CHECK_EQUAL(FIELD_ID, rsLog.Field(L"FIELD_ID").AsInt());
		rsLog.Close();
	}
}