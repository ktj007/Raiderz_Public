#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "TestDBTableStatus.h"
#include "DBTestHelper.h"
#include "GConfig.h"


SUITE(DBTestGuild)
{
	GAMEDB_TEST(USP_RZ_GUILD_INSERT)
	{
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nMoney		= 1234;
		const wchar_t* szGName	= L"test_guild";

		TDBRecordSet rsGuildIns;
		CHECK(UTestDB.Execute(rsGuildIns, L"{CALL dbo.USP_RZ_GUILD_INSERT (%d, %I64d, %I64d, %d, N'%s')}"
			, GConfig::m_nMyWorldID, nAID, nCID, nMoney, szGName));
		const int64 nGID = rsGuildIns.Field(L"GUILD_ID").AsInt64();
		CHECK(0 < nGID);
		rsGuildIns.Close();

		TDBRecordSet rsGuild;
		UTestDB.Execute(rsGuild, L"SELECT NAME FROM dbo.RZ_GUILD WHERE WORLD_ID = %d AND GUILD_ID = %I64d;", GConfig::m_nMyWorldID, nGID);
		CHECK(szGName ==  rsGuild.Field(L"NAME").AsWString());
		rsGuild.Close();

		TDBRecordSet rsChar;
		UTestDB.Execute(rsChar, L"SELECT MONEY FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID);
		CHECK_EQUAL(nMoney, rsChar.Field(L"MONEY").AsInt());		
		rsChar.Close();

		TDBRecordSet rsm;
		UTestDB.Execute(rsm, L"SELECT GUILD_ID FROM dbo.RZ_GUILD_MEMBER WHERE CHAR_ID = %I64d AND GRADE = 1"
			, nCID);
		CHECK_EQUAL(1, rsm.GetFetchedRowCount());
		CHECK_EQUAL(nGID, rsm.Field(L"GUILD_ID").AsInt64());
		rsm.Close();
		
	}

	GAMEDB_TEST(USP_RZ_GUILD_DELETE)
	{
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const wchar_t* szGName	= L"test_guild";
		const int64	nGID		= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, szGName);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nCID, GMG_MASTER);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_DELETE (%d, %I64d, %I64d, %I64d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nGID));

		TDBRecordSet rsGuild;
		UTestDB.Execute(rsGuild, L"SELECT UNREG_DATE FROM dbo.RZ_GUILD WHERE GUILD_ID = %I64d;", nGID);
		CHECK_EQUAL(1, rsGuild.GetFetchedRowCount());
		CHECK(!rsGuild.Field(L"UNREG_DATE").IsNull());
		rsGuild.Close();	

		TDBRecordSet rsm;
		UTestDB.Execute(rsm, L"SELECT COUNT(*) CNT FROM dbo.RZ_GUILD_MEMBER WHERE GUILD_ID = %I64d;", nGID);
		CHECK_EQUAL(1, rsm.GetFetchedRowCount());
		CHECK_EQUAL(0, rsm.Field(L"CNT").AsInt());
		rsm.Close();
	}

	GAMEDB_TEST(USP_RZ_GUILD_GET_INFO)
	{
		const int64 nAID = 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const wchar_t* szGName	= L"test_guild";
		const int nStorageMoney = 10;
		const int64	nGID		= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, szGName, nStorageMoney);		
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nCID, GMG_MASTER);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{ CALL dbo.USP_RZ_GUILD_GET_INFO (%d, %I64d)}", GConfig::m_nMyWorldID, nGID);
		CHECK(szGName == rs.Field(L"NAME").AsWString());
		CHECK_EQUAL(nStorageMoney, rs.Field(L"STORAGE_MONEY").AsInt());
		rs.Close();		
	}

	GAMEDB_TEST(USP_RZ_GUILD_GET_MEMBER)
	{
		const int64 nAID1 = 1;
		const int64 nAID2 = 2;
		const int64	nMaster	= DBTestHelper::InsertCharacter(nAID1, GConfig::m_nMyWorldID, L"master");
		const int64	nMember = DBTestHelper::InsertCharacter(nAID2, GConfig::m_nMyWorldID, L"member");
		const wchar_t* szGName = L"test_guild";
		const int64	nGID	= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, szGName);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID1, nMaster, GMG_MASTER);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID2, nMember, GMG_NORMAL);

		TDBRecordSet rsGMem;
		bool bCheck1 = false;
		bool bCheck2 = false;
		CHECK(UTestDB.Execute(rsGMem, L"{CALL dbo.USP_RZ_GUILD_GET_MEMBER (%d, %I64d)}", GConfig::m_nMyWorldID, nGID));
		for (; !rsGMem.IsEOF(); rsGMem.MoveNext())
		{
			if (nMaster == rsGMem.Field(L"CHAR_ID").AsInt())
			{
				CHECK(L"master" == rsGMem.Field(L"NAME").AsWString());
				CHECK_EQUAL((uint8)GMG_MASTER, rsGMem.Field(L"GRADE").AsByte());
				bCheck1 = true;
			}
			else
			{
				CHECK(L"member" == rsGMem.Field(L"NAME").AsWString());
				CHECK_EQUAL((uint8)GMG_NORMAL, rsGMem.Field(L"GRADE").AsByte());
				bCheck2 = true;
			}
		}
		CHECK(bCheck1);
		CHECK(bCheck2);
		CHECK_EQUAL(2, rsGMem.GetFetchedRowCount());
		rsGMem.Close();
	}

	GAMEDB_TEST(USP_RZ_GUILD_MEMBER_INSERT)
	{
		const int64 nAID		= 1;
		const int64	nMaster		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"master");
		const int64	nMember		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"member");
		const wchar_t* szGName	= L"test_guild";
		const int64	nGID		= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, szGName);

		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nMaster, GMG_MASTER);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_MEMBER_INSERT (%d, %I64d, %I64d, %I64d)}"
			, GConfig::m_nMyWorldID, nAID, nMember, nGID));

		TDBRecordSet rsr1;
		UTestDB.Execute(rsr1, L"SELECT GRADE FROM dbo.RZ_GUILD_MEMBER WHERE CHAR_ID = %I64d AND GUILD_ID = %I64d"
			, nMember, nGID);
		CHECK_EQUAL(1, rsr1.GetFetchedRowCount());
		CHECK_EQUAL(100, rsr1.Field(L"GRADE").AsInt64());
		rsr1.Close();		
	}

	GAMEDB_TEST(USP_RZ_GUILD_MEMBER_DELETE)
	{
		const int64 nAID		= 1;
		const int64	nMaster		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"master");
		const int64	nMember		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"member");
		const wchar_t* szGName	= L"test_guild";
		const int64	nGID		= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, szGName);

		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nMaster, GMG_MASTER);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nMember, GMG_NORMAL);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_MEMBER_DELETE (%d, %I64d, %I64d, %I64d)}"
			, GConfig::m_nMyWorldID, nAID, nMember, nGID));

		TDBRecordSet rsc;
		UTestDB.Execute(rsc, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_GUILD_MEMBER \
								 WHERE CHAR_ID = %I64d AND GUILD_ID = %I64d"
								 , nMember, nGID);
		CHECK_EQUAL(0, rsc.Field(L"CNT").AsInt());		
		rsc.Close();		
	}

	GAMEDB_TEST(USP_RZ_GUILD_CHANGE_MASTER)
	{
		const int64 nAID1 = 1;
		const int64 nAID2 = 2;
		const int64	nMaster		= DBTestHelper::InsertCharacter(nAID1, GConfig::m_nMyWorldID, L"master");
		const int64	nMember		= DBTestHelper::InsertCharacter(nAID2, GConfig::m_nMyWorldID, L"member");
		const int	nCharPtm	= 23;
		const wchar_t* szGName	= L"test_guild";
		const int64	nGID		= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, szGName);

		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID1, nMaster, GMG_MASTER);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID2, nMember, GMG_NORMAL);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_CHANGE_MASTER (%d, %I64d, %I64d, %I64d)}"
			, GConfig::m_nMyWorldID, nGID, nMaster, nMember));

		TDBRecordSet rsms1;
		UTestDB.Execute(rsms1, L"SELECT GRADE FROM dbo.RZ_GUILD_MEMBER \
								   WHERE CHAR_ID = %I64d AND GUILD_ID = %I64d;", nMaster, nGID);
		CHECK_EQUAL(100, rsms1.Field(L"GRADE").AsByte());		
		rsms1.Close();

		TDBRecordSet rsms2;
		UTestDB.Execute(rsms2, L"SELECT GRADE FROM dbo.RZ_GUILD_MEMBER \
								WHERE CHAR_ID = %I64d AND GUILD_ID = %I64d;", nMember, nGID);
		CHECK_EQUAL(1, rsms2.Field(L"GRADE").AsByte());		
		rsms2.Close();
	}

	GAMEDB_TEST(USP_RZ_GUILD_UPDATE_STORAGE_MONEY)
	{
		const int64		CSN				= DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID);
		const int		BEF_MONEY_FROM	= 10;
		const int		NEXT_MONEY_FROM = 20;

		const int		BEF_MONEY_TO	= 30;
		const int64		GUILD_ID		= DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, L"TEST GUIILD", BEF_MONEY_TO);
		const int		NEXT_MONEY_TO	= 20;

		UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET MONEY = %d WHERE CHAR_ID = %I64d;", BEF_MONEY_FROM, CSN);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_UPDATE_STORAGE_MONEY (%d, %I64d, %d, %d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, CSN, BEF_MONEY_FROM, NEXT_MONEY_FROM
			, GUILD_ID, BEF_MONEY_TO, NEXT_MONEY_TO);

		TDBRecordSet rsc;
		UTestDB.Execute(rsc, L"SELECT MONEY FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", CSN);
		CHECK_EQUAL(1, rsc.GetFetchedRowCount());
		CHECK_EQUAL(NEXT_MONEY_FROM, rsc.Field(L"MONEY").AsInt());
		rsc.Close();

		TDBRecordSet rsg;
		UTestDB.Execute(rsg, L"SELECT STORAGE_MONEY FROM dbo.RZ_GUILD WHERE GUILD_ID = %I64d;", GUILD_ID);
		CHECK_EQUAL(1, rsg.GetFetchedRowCount());
		CHECK_EQUAL(NEXT_MONEY_TO, rsg.Field(L"STORAGE_MONEY").AsInt());
		rsg.Close();
	}
}

