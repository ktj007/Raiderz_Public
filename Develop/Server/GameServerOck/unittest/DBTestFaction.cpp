#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"


SUITE(DBTestFaction)
{
	GAMEDB_TEST(USP_RZ_CHAR_GET_FACTION)
	{
		const int64 nAID	= 1;
		const int64	nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nFactID = 1;
		const int16 nVal	= 42; 

		DBTestHelper::InsertFaction(GConfig::m_nMyWorldID, nAID, nCID, nFactID, nVal);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_FACTION (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nFactID, rs.Field(L"FACT_ID").AsInt());
		CHECK_EQUAL(nVal, rs.Field(L"VAL").AsShort());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_FACTION_INSERT)
	{
		const int64 nAID	= 1;
		const int64	nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nFactID = 1;
		const int16 nVal	= 42; 

		UTestDB.Execute(L"{CALL dbo.USP_RZ_FACTION_INSERT (%d, %I64d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nFactID, nVal);


		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT FACT_ID, VAL FROM dbo.RZ_FACTION WHERE CHAR_ID = %I64d;", nCID);
		CHECK_EQUAL(nFactID, rs.Field(L"FACT_ID").AsInt());
		CHECK_EQUAL(nVal, rs.Field(L"VAL").AsShort());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_FACTION_UPDATE)
	{
		const int64 nAID	= 1;
		const int64	nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nFactID = 1;
		const int16 nVal	= 42; 

		DBTestHelper::InsertFaction(GConfig::m_nMyWorldID, nAID, nCID, 0, 0);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_FACTION_UPDATE (%d, %I64d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nFactID, nVal);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT VAL FROM dbo.RZ_FACTION WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND FACT_ID = %d;"
			, nAID, nCID, nFactID);
		CHECK_EQUAL(nVal, rs.Field(L"VAL").AsShort());
		rs.Close();
	}
}