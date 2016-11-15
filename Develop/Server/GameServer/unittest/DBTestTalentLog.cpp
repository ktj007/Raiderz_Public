#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GDBTaskDataTalent.h"


SUITE(DBTEST_TALENT_LOG)
{
	const int64		GSN			= 1;
	const int		WORLD		= 2;
	const int64		CSN			= 3;
	const int		CODE		= 52;
	const int		CHAR_PTM	= 4;
	const int		TALENT_ID	= 10;
	const short		REMAIN_TP	= 11;

	LOGDB_TEST(USP_RZ_TALENT_LEARN_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_LEARN_LOG (%I64d, %d, %I64d, %d)}"
			, GSN, WORLD, CSN, TALENT_ID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT TALENT_ID FROM dbo.RZ_TALENT_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 601;"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(TALENT_ID, rs.Field(L"TALENT_ID").AsInt());
		rs.Close();
	}

	LOGDB_TEST(USP_RZ_TALENT_RESET_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_RESET_LOG (%I64d, %d, %I64d, %d)}"
			, GSN, WORLD, CSN, TALENT_ID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT TALENT_ID FROM dbo.RZ_TALENT_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 602;"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(TALENT_ID, rs.Field(L"TALENT_ID").AsInt());
		rs.Close();
	}

	LOGDB_TEST(USP_RZ_TALENT_RESET_ALL_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_RESET_ALL_LOG (%I64d, %d, %I64d)}"
			, GSN, WORLD, CSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT TALENT_ID FROM dbo.RZ_TALENT_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 603;"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK(rs.Field(L"TALENT_ID").IsNull());
		rs.Close();
	}
}