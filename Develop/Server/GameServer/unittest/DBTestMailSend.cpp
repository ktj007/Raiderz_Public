#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

SUITE(RZ_MAIL_SEND)
{
	const static int	MAIL_TEST_DEF_CHAR_MONEY	= 100;
	const static int16	MAIL_TEST_ITEM_STACK_AMT	= 10;
	const static int	MAIL_TEST_USAGE_PERIOD		= 10000;
	const static uint8	MAIL_TEST_DURA				= 1;

	class ML_CHAR
	{
	public :
		ML_CHAR() : m_nGSN(0), m_nCSN(0), m_nCharPtm(0) {}
		ML_CHAR(const int64 nGSN, const wstring& strName, const int nCharPtm) 
			: m_nGSN(nGSN), m_strName(strName), m_nCharPtm(nCharPtm) 
		{
			m_nCSN = DBTestHelper::InsertCharacter(m_nGSN, GConfig::m_nMyWorldID, strName.c_str());
			UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET CHAR_PTM = %d, [MONEY] = %d WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;"
				, nCharPtm, MAIL_TEST_DEF_CHAR_MONEY, m_nGSN, m_nCSN);
		}

		int64	m_nGSN;
		int64	m_nCSN;
		wstring m_strName;
		int		m_nCharPtm;
	};

	class FIXTURE
	{
	public :
		FIXTURE() : m_Sndr(1, L"sndr", 5), m_Rcvr(2, L"rcvr", 10), m_nSndMoney(10), m_strTitle(L"test title")
		{
			m_nItemID_1 = 1;
			m_nItemID_2 = 2;
			m_nItemID_3 = 3;
			m_nItemID_4 = 4;
			m_nItemID_5 = 5;

			m_nIUID_1 = DBTestHelper::InsertItem(m_Sndr.m_nGSN, m_Sndr.m_nCSN, GConfig::m_nMyWorldID, m_nItemID_1, MAIL_TEST_ITEM_STACK_AMT, 1, 0, false, MAIL_TEST_USAGE_PERIOD);
			m_nIUID_2 = DBTestHelper::InsertItem(m_Sndr.m_nGSN, m_Sndr.m_nCSN, GConfig::m_nMyWorldID, m_nItemID_2, MAIL_TEST_ITEM_STACK_AMT, 1, 1, false, MAIL_TEST_USAGE_PERIOD);
			m_nIUID_3 = DBTestHelper::InsertItem(m_Sndr.m_nGSN, m_Sndr.m_nCSN, GConfig::m_nMyWorldID, m_nItemID_3, MAIL_TEST_ITEM_STACK_AMT, 1, 2, false, MAIL_TEST_USAGE_PERIOD);
			m_nIUID_4 = DBTestHelper::InsertItem(m_Sndr.m_nGSN, m_Sndr.m_nCSN, GConfig::m_nMyWorldID, m_nItemID_4, MAIL_TEST_ITEM_STACK_AMT, 1, 3, false, MAIL_TEST_USAGE_PERIOD);
			m_nIUID_5 = DBTestHelper::InsertItem(m_Sndr.m_nGSN, m_Sndr.m_nCSN, GConfig::m_nMyWorldID, m_nItemID_5, MAIL_TEST_ITEM_STACK_AMT, 1, 4, false, MAIL_TEST_USAGE_PERIOD);

			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET PERIOD = 0, USAGE_PERIOD = 0, EFF_END_DATE = NULL WHERE OWNER_ID = %I64d;", m_Sndr.m_nCSN);
		}

		enum
		{
			MAIL_TYPE_USER = 1
		  , MAIL_TYPE_NPC = 2
		};

	public :
		ML_CHAR m_Sndr;
		ML_CHAR m_Rcvr;
		int		m_nSndMoney;
		wstring	m_strTitle;

		int		m_nItemID_1;
		int		m_nItemID_2;
		int		m_nItemID_3;
		int		m_nItemID_4;
		int		m_nItemID_5;

		int64	m_nIUID_1;
		int64	m_nIUID_2;
		int64	m_nIUID_3;
		int64	m_nIUID_4;
		int64	m_nIUID_5;
	};

	const wstring strSQL = L"{CALL USP_RZ_MAIL_SEND (%d, %I64d, %I64d, N'%s', %d, %d, %d, N'%s', %d, N'%s', %d, N'%s', %d, %d\
													  , %d, %I64d, %d, %d, %d\
													  , %d, %I64d, %d, %d, %d\
													  , %d, %I64d, %d, %d, %d\
													  , %d, %I64d, %d, %d, %d\
													  , %d, %I64d, %d, %d, %d\
													  , %d)}";

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_MAIL_SEND_NO_TEXT_NO_ITEM)
	{
		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str()
			, GConfig::m_nMyWorldID
			, m_Sndr.m_nGSN, m_Sndr.m_nCSN, m_Sndr.m_strName.c_str(), m_Sndr.m_nCharPtm, MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, m_nSndMoney
			, m_Rcvr.m_strName.c_str(), MAIL_TYPE_USER, m_strTitle.c_str(), false, L"", 100, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		const int64 MAIL_UID = rs.Field(L"MAIL_ID").AsInt64();
		CHECK(0 < MAIL_UID);
		CHECK_EQUAL(m_Rcvr.m_nGSN, rs.Field(L"RCVR_ACCN_ID").AsInt64());
		CHECK_EQUAL(m_Rcvr.m_nCSN, rs.Field(L"RCVR_CHAR_ID").AsInt64());
		rs.Close();

		TDBRecordSet rsMail;
		UTestDB.Execute(rsMail, L"SELECT SNDR_NAME, TITLE, HAVE_TEXT, [MONEY], READ_FLAG, DEF_ITEM_ID FROM dbo.RZ_MAIL WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND MAIL_ID = %I64d AND MAIL_TYPE = %d;"
			, m_Rcvr.m_nGSN, m_Rcvr.m_nCSN, MAIL_UID, MAIL_TYPE_USER);
		CHECK(m_Sndr.m_strName == rsMail.Field(L"SNDR_NAME").AsWString());
		CHECK(m_strTitle == rsMail.Field(L"TITLE").AsWString());
		CHECK_EQUAL(false, rsMail.Field(L"HAVE_TEXT").AsBool());
		CHECK_EQUAL(m_nSndMoney, rsMail.Field(L"MONEY").AsInt());
		CHECK_EQUAL(false, rsMail.Field(L"READ_FLAG").AsBool());
		CHECK_EQUAL(0, rsMail.Field(L"DEF_ITEM_ID").AsInt());
		rsMail.Close();

		TDBRecordSet rsSndr;
		UTestDB.Execute(rsSndr, L"SELECT [MONEY] FROM dbo.RZ_CHARACTER WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", m_Sndr.m_nGSN, m_Sndr.m_nCSN);
		CHECK_EQUAL(MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, rsSndr.Field(L"MONEY").AsInt());
		rsSndr.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_MAIL_SEND_NO_ITEM)
	{
		wstring strText = L"test text";

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str()
			, GConfig::m_nMyWorldID
			, m_Sndr.m_nGSN, m_Sndr.m_nCSN, m_Sndr.m_strName.c_str(), m_Sndr.m_nCharPtm, MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, m_nSndMoney
			, m_Rcvr.m_strName.c_str(), MAIL_TYPE_USER, m_strTitle.c_str(), true, strText.c_str(), 100, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0, (int64)0, 0, 0, 0
			, 0);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		const int64 MAIL_UID = rs.Field(L"MAIL_ID").AsInt64();
		CHECK(0 < MAIL_UID);
		CHECK_EQUAL(m_Rcvr.m_nGSN, rs.Field(L"RCVR_ACCN_ID").AsInt64());
		CHECK_EQUAL(m_Rcvr.m_nCSN, rs.Field(L"RCVR_CHAR_ID").AsInt64());
		rs.Close();

		TDBRecordSet rsMail;
		UTestDB.Execute(rsMail, L"SELECT SNDR_NAME, TITLE, HAVE_TEXT, [MONEY], READ_FLAG, DEF_ITEM_ID FROM dbo.RZ_MAIL WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND MAIL_ID = %I64d AND MAIL_TYPE = %d;"
			, m_Rcvr.m_nGSN, m_Rcvr.m_nCSN, MAIL_UID, MAIL_TYPE_USER);
		CHECK(m_Sndr.m_strName == rsMail.Field(L"SNDR_NAME").AsWString());
		CHECK(m_strTitle == rsMail.Field(L"TITLE").AsWString());
		CHECK_EQUAL(true, rsMail.Field(L"HAVE_TEXT").AsBool());
		CHECK_EQUAL(m_nSndMoney, rsMail.Field(L"MONEY").AsInt());
		CHECK_EQUAL(false, rsMail.Field(L"READ_FLAG").AsBool());
		CHECK_EQUAL(0, rsMail.Field(L"DEF_ITEM_ID").AsInt());
		rsMail.Close();

		TDBRecordSet rsMailText;
		UTestDB.Execute(rsMailText, L"SELECT [TEXT] FROM dbo.RZ_MAIL_TEXT WHERE MAIL_ID = %I64d;", MAIL_UID);
		CHECK(strText == rsMailText.Field(L"TEXT").AsWString());
		rsMailText.Close();

		TDBRecordSet rsSndr;
		UTestDB.Execute(rsSndr, L"SELECT [MONEY] FROM dbo.RZ_CHARACTER WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", m_Sndr.m_nGSN, m_Sndr.m_nCSN);
		CHECK_EQUAL(MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, rsSndr.Field(L"MONEY").AsInt());
		rsSndr.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_MAIL_SEND_MOVE_ITEM)
	{
		wstring strText = L"test text";

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str()
			, GConfig::m_nMyWorldID
			, m_Sndr.m_nGSN, m_Sndr.m_nCSN, m_Sndr.m_strName.c_str(), m_Sndr.m_nCharPtm, MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, m_nSndMoney
			, m_Rcvr.m_strName.c_str(), MAIL_TYPE_USER, m_strTitle.c_str(), true, strText.c_str(), 100, 0
			, 0, m_nIUID_1, 0, MAIL_TEST_ITEM_STACK_AMT, MAIL_TEST_DURA
			, 1, m_nIUID_2, 0, MAIL_TEST_ITEM_STACK_AMT, MAIL_TEST_DURA
			, 2, m_nIUID_3, 0, MAIL_TEST_ITEM_STACK_AMT, MAIL_TEST_DURA
			, 3, m_nIUID_4, 0, MAIL_TEST_ITEM_STACK_AMT, MAIL_TEST_DURA
			, 4, m_nIUID_5, 0, MAIL_TEST_ITEM_STACK_AMT, MAIL_TEST_DURA
			, 5);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		const int64 MAIL_UID = rs.Field(L"MAIL_ID").AsInt64();
		CHECK(0 < MAIL_UID);
		CHECK_EQUAL(m_Rcvr.m_nGSN, rs.Field(L"RCVR_ACCN_ID").AsInt64());
		CHECK_EQUAL(m_Rcvr.m_nCSN, rs.Field(L"RCVR_CHAR_ID").AsInt64());
		rs.Close();

		TDBRecordSet rsMailItemCount;
		UTestDB.Execute(rsMailItemCount, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_MAIL_ITEM WHERE MAIL_ID = %I64d;", MAIL_UID);
		CHECK_EQUAL(5, rsMailItemCount.Field(L"CNT").AsInt());
		rsMailItemCount.Close();


		TDBRecordSet rsItemCount;
		UTestDB.Execute(rsItemCount, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND (ITEM_ID IS NOT NULL OR IUID > 0 OR CHAR_PTM IS NOT NULL OR STACK_AMT > 0);", m_Sndr.m_nCSN);
		CHECK_EQUAL(1, rsItemCount.GetFetchedRowCount());
		CHECK_EQUAL(0, rsItemCount.Field(L"CNT").AsInt());
		rsItemCount.Close();

	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_MAIL_SEND_SPLIT_ITEM)
	{
		wstring strText = L"test text";

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str()
			, GConfig::m_nMyWorldID
			, m_Sndr.m_nGSN, m_Sndr.m_nCSN, m_Sndr.m_strName.c_str(), m_Sndr.m_nCharPtm, MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, m_nSndMoney
			, m_Rcvr.m_strName.c_str(), MAIL_TYPE_USER, m_strTitle.c_str(), true, strText.c_str(), 100, 0
			, 0, m_nIUID_1, MAIL_TEST_ITEM_STACK_AMT - 1, 1, MAIL_TEST_DURA
			, 1, m_nIUID_2, MAIL_TEST_ITEM_STACK_AMT - 2, 2, MAIL_TEST_DURA
			, 2, m_nIUID_3, MAIL_TEST_ITEM_STACK_AMT - 3, 3, MAIL_TEST_DURA
			, 3, m_nIUID_4, MAIL_TEST_ITEM_STACK_AMT - 4, 4, MAIL_TEST_DURA
			, 4, m_nIUID_5, MAIL_TEST_ITEM_STACK_AMT - 5, 5, MAIL_TEST_DURA
			, 5);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		const int64 MAIL_UID = rs.Field(L"MAIL_ID").AsInt64();
		CHECK(0 < MAIL_UID);
		CHECK_EQUAL(m_Rcvr.m_nGSN, rs.Field(L"RCVR_ACCN_ID").AsInt64());
		CHECK_EQUAL(m_Rcvr.m_nCSN, rs.Field(L"RCVR_CHAR_ID").AsInt64());
		rs.Close();

		TDBRecordSet rsItemCount;
		UTestDB.Execute(rsItemCount, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND STACK_AMT + ITEM_ID = 10", m_Sndr.m_nCSN);
		CHECK_EQUAL(1, rsItemCount.GetFetchedRowCount());
		CHECK_EQUAL(5, rsItemCount.Field(L"CNT").AsInt());
		rsItemCount.Close();

		TDBRecordSet rsMailItemCount;
		UTestDB.Execute(rsMailItemCount, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_MAIL_ITEM WHERE MAIL_ID = %I64d;", MAIL_UID);
		CHECK_EQUAL(5, rsMailItemCount.Field(L"CNT").AsInt());
		rsMailItemCount.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_MAIL_SEND_INVALID_RCVR_NAME)
	{
		wstring strText				= L"test text";
		wstring strInvalidRcvrName	= L"BAD NAME";

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str()
			, GConfig::m_nMyWorldID
			, m_Sndr.m_nGSN, m_Sndr.m_nCSN, m_Sndr.m_strName.c_str(), m_Sndr.m_nCharPtm, MAIL_TEST_DEF_CHAR_MONEY - m_nSndMoney, m_nSndMoney
			, strInvalidRcvrName.c_str(), MAIL_TYPE_USER, m_strTitle.c_str(), true, strText.c_str(), 100, 0
			, 0, m_nIUID_1, MAIL_TEST_ITEM_STACK_AMT - 1, 1, MAIL_TEST_DURA
			, 1, m_nIUID_2, MAIL_TEST_ITEM_STACK_AMT - 2, 2, MAIL_TEST_DURA
			, 2, m_nIUID_3, MAIL_TEST_ITEM_STACK_AMT - 3, 3, MAIL_TEST_DURA
			, 3, m_nIUID_4, MAIL_TEST_ITEM_STACK_AMT - 4, 4, MAIL_TEST_DURA
			, 4, m_nIUID_5, MAIL_TEST_ITEM_STACK_AMT - 5, 5, MAIL_TEST_DURA
			, 5);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(-1, rs.Field(L"MAIL_ID").AsInt64());
		CHECK_EQUAL(-1, rs.Field(L"RCVR_ACCN_ID").AsInt64());
		CHECK_EQUAL(-1, rs.Field(L"RCVR_CHAR_ID").AsInt64());
		rs.Close();
	}
}