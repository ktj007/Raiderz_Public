#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"


SUITE(DBTestCutscene)
{
	GAMEDB_TEST(USP_RZ_CUTSCENE_INSERT)
	{
		const int64 nAID		= 1;
		const int64 nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nCutsceneID	= 999;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CUTSCENE_INSERT (%d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nCutsceneID));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT CUTSCN_ID FROM dbo.RZ_CUTSCENE WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND CUTSCN_ID = %d", nAID, nCID, nCutsceneID));
		CHECK_EQUAL(1, rs.GetFetchedCount());
		CHECK_EQUAL(nCutsceneID, rs.Field(L"CUTSCN_ID").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_GET_CUTSECNE)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nCutscnID = 1;

		DBTestHelper::InsertCutscene(GConfig::m_nMyWorldID, nAID, nCID, nCutscnID);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_CUTSECNE (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nCutscnID, rs.Field(L"CUTSCN_ID").AsInt());
		rs.Close();
	}

}