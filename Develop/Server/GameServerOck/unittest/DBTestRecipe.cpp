#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

SUITE(DBTestRecipe)
{
	GAMEDB_TEST(USP_RZ_CHAR_GET_RECIPE)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nRecpID	= 1;
		
		DBTestHelper::InsertRecp(GConfig::m_nMyWorldID, nAID, nCID, nRecpID);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_RECIPE (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nRecpID, rs.Field(L"RECP_ID").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_RECP_DELETE)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nRecpID	= 1;

		DBTestHelper::InsertRecp(GConfig::m_nMyWorldID, nAID, nCID, nRecpID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_RECP_DELETE (%d, %I64d, %I64d, %d)}", GConfig::m_nMyWorldID, nAID, nCID, nRecpID));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT COUNT(*) cnt FROM dbo.RZ_RECIPE WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", nAID, nCID);
		CHECK_EQUAL(0, rs.Field(L"cnt").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_RECP_INSERT)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nRecpID	= 1;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_RECP_INSERT (%d, %I64d, %I64d, %d)}", GConfig::m_nMyWorldID, nAID, nCID, nRecpID));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT RECP_ID FROM dbo.RZ_RECIPE WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", nAID, nCID);
		CHECK_EQUAL(nRecpID, rs.Field(L"RECP_ID").AsInt());
		rs.Close();
	}
}