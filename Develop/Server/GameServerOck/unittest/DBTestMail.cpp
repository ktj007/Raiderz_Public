#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

SUITE(RZ_MAIL)
{
	const int64		GSN			= 1;
	const int64		CSN			= 1;
	const uint8		TYPE		= 1;
	const wstring	SNDR_NAME	= L"TEST SENDER";
	const wstring	TITLE		= L"TEST TITLE";
	const bool		HAVE_TEXT	= true;
	const int		MONEY		= 100;
	const int		DEF_ITEM_ID	= 1;
	
	GAMEDB_TEST(USP_RZ_MAIL_GET_SUMMARY)
	{
		const int64 MAIL_UID_1 = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN, TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, false, DEF_ITEM_ID);
		const int64 MAIL_UID_2 = DBTestHelper::InsertDeleteMail(GConfig::m_nMyWorldID, GSN, CSN);
		const int64 MAIL_UID_3 = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN, TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, true, DEF_ITEM_ID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_MAIL_GET_SUMMARY (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, GSN, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(1, rs.Field(L"UNREAD_MAIL_COUNT").AsInt());
		CHECK_EQUAL(2, rs.Field(L"TOTAL_MAIL_COUNT").AsInt());
		CHECK_EQUAL(MAIL_UID_3, rs.Field(L"TOP_MAIL_ID").AsInt64());
		CHECK_EQUAL(MAIL_UID_1, rs.Field(L"BOTTOM_MAIL_ID").AsInt64());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_GET_LIST_UNREAD)
	{
		const int64 MAIL_UID		= DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN, TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, false, DEF_ITEM_ID);
		const int64 MAIL_UID_DEL	= DBTestHelper::InsertDeleteMail(GConfig::m_nMyWorldID, GSN, CSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_MAIL_GET_LIST (%d, %I64d, %I64d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, GSN, CSN, MAIL_UID_DEL, MAIL_UID, 8);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(MAIL_UID, rs.Field(L"MAIL_ID").AsInt64());
		CHECK_EQUAL(TYPE, rs.Field(L"MAIL_TYPE").AsByte());
		CHECK(SNDR_NAME == rs.Field(L"SNDR_NAME").AsWString());
		CHECK(TITLE == rs.Field(L"TITLE").AsWString());
		CHECK_EQUAL(HAVE_TEXT, rs.Field(L"HAVE_TEXT").AsBool());
		CHECK_EQUAL(MONEY, rs.Field(L"MONEY").AsInt());
		CHECK_EQUAL(false, rs.Field(L"READ_FLAG").AsBool());
		CHECK_EQUAL(DEF_ITEM_ID, rs.Field(L"DEF_ITEM_ID").AsInt());
		rs.MoveNext();
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_GET_LIST_READ)
	{
		const int64 MAIL_UID		= DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN, TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, true, DEF_ITEM_ID);
		const int64 MAIL_UID_DEL	= DBTestHelper::InsertDeleteMail(GConfig::m_nMyWorldID, GSN, CSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_MAIL_GET_LIST (%d, %I64d, %I64d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, GSN, CSN, MAIL_UID_DEL, MAIL_UID, 8);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(MAIL_UID, rs.Field(L"MAIL_ID").AsInt64());
		CHECK_EQUAL(TYPE, rs.Field(L"MAIL_TYPE").AsByte());
		CHECK(SNDR_NAME == rs.Field(L"SNDR_NAME").AsWString());
		CHECK(TITLE == rs.Field(L"TITLE").AsWString());
		CHECK_EQUAL(HAVE_TEXT, rs.Field(L"HAVE_TEXT").AsBool());
		CHECK_EQUAL(MONEY, rs.Field(L"MONEY").AsInt());
		CHECK_EQUAL(true, rs.Field(L"READ_FLAG").AsBool());
		CHECK_EQUAL(DEF_ITEM_ID, rs.Field(L"DEF_ITEM_ID").AsInt());		
		rs.MoveNext();
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_GET_LIST_NO_HAS_MSG)
	{
		const int64 MAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN, TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, true, DEF_ITEM_ID);
		UTestDB.Execute(L"UPDATE dbo.RZ_MAIL SET HAVE_TEXT = 0 WHERE MAIL_ID = %I64d;", MAIL_UID);
		
		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_MAIL_GET_LIST (%d, %I64d, %I64d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, GSN, CSN, MAIL_UID + 1, MAIL_UID, 8);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(MAIL_UID, rs.Field(L"MAIL_ID").AsInt64());
		CHECK_EQUAL(TYPE, rs.Field(L"MAIL_TYPE").AsByte());
		CHECK(SNDR_NAME == rs.Field(L"SNDR_NAME").AsWString());
		CHECK(TITLE == rs.Field(L"TITLE").AsWString());
		CHECK_EQUAL(false, rs.Field(L"HAVE_TEXT").AsBool());
		CHECK_EQUAL(MONEY, rs.Field(L"MONEY").AsInt());
		CHECK_EQUAL(true, rs.Field(L"READ_FLAG").AsBool());
		CHECK_EQUAL(DEF_ITEM_ID, rs.Field(L"DEF_ITEM_ID").AsInt());		
		rs.MoveNext();
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_GET_LIST_MAIL_COUNT)
	{
		const int MAIL_COUNT = 8;
		const int64 BOTTOM_MAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN);
		for (int i = 0; i < 10; ++i)
		{
			DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN);
		}
		const int64 TOP_MAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN);
		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_MAIL_GET_LIST (%d, %I64d, %I64d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, GSN, CSN, TOP_MAIL_UID, BOTTOM_MAIL_UID, MAIL_COUNT);
		int mc = 0;
		for (; !rs.IsEOF(); rs.MoveNext())
		{
			++mc;
		}
		CHECK_EQUAL(MAIL_COUNT, mc);
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_DELETE)
	{
		const int64 MAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN);
		UTestDB.Execute(L"{CALL dbo.USP_RZ_MAIL_DELETE (%d, %I64d, %I64d, %I64d)}", GConfig::m_nMyWorldID, GSN, CSN, MAIL_UID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT ISNULL(COUNT(*), 0) AS CNT FROM dbo.RZ_MAIL WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;"
			, GSN, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"CNT").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_GET_TEXT)
	{
		const int64 MAIL_UID = 1;
		wstring strTEXT = L"TEST TEXT";
		UTestDB.Execute(L"INSERT INTO dbo.RZ_MAIL_TEXT(WORLD_ID, MAIL_ID, TEXT, DEL_DATE) VALUES (%d, %I64d, '%s', GETDATE());"
			, GConfig::m_nMyWorldID, MAIL_UID, strTEXT.c_str());

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT TEXT FROM dbo.RZ_MAIL_TEXT WHERE MAIL_ID = %I64d;", MAIL_UID);
		CHECK(strTEXT == rs.Field(L"TEXT").AsWString());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_MAIL_SET_READ)
	{
		const int64 MAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN);
		UTestDB.Execute(L"UPDATE dbo.RZ_MAIL SET READ_FLAG = 0;");

		UTestDB.Execute(L"{CALL dbo.USP_RZ_MAIL_SET_READ (%d, %I64d, %I64d, %I64d)}", GConfig::m_nMyWorldID, GSN, CSN, MAIL_UID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT READ_FLAG FROM dbo.RZ_MAIL WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND MAIL_ID = %I64d;"
			, GSN, CSN, MAIL_UID);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(true, rs.Field(L"READ_FLAG").AsBool());
		rs.Close();
	}
}
