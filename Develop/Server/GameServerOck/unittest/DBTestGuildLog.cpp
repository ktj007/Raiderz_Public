#include "stdafx.h"
#include "DBTestGlobal.h"

SUITE(DBTEST_GUILD_LOG)
{
	const int64		GSN			= 1;
	const int		WORLD		= 2;
	const int64		CSN			= 3;
	const int		CODE		= 4;
	const int64		GUILD_ID	= 5;
	const wstring	NAME		= L"test";


	LOGDB_TEST(USPP_RZ_GUILD_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_LOG (%I64d, %d, %I64d, %d, %I64d, N'%s')}"
			, GSN, WORLD, CSN, CODE, GUILD_ID, NAME.c_str());

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT GUILD_ID, NAME FROM dbo.RZ_GUILD_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = %d"
			, GSN, WORLD, CSN, CODE);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(GUILD_ID, rs.Field(L"GUILD_ID").AsInt64());
		CHECK(NAME == rs.Field(L"NAME").AsWString());
		rs.Close();
	}

	LOGDB_TEST(USP_RZ_GUILD_MEMBER_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_MEMBER_LOG (%I64d, %d, %I64d, %d, %I64d)}"
			, GSN, WORLD, CSN, CODE, GUILD_ID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT GUILD_ID FROM dbo.RZ_GUILD_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = %d"
			, GSN, WORLD, CSN, CODE);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(GUILD_ID, rs.Field(L"GUILD_ID").AsInt64());		
		rs.Close();
	}	
}