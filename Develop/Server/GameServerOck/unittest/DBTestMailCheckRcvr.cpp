#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(RZ_MAIL_CHECK_RCVR)
{
	class ML_CHAR
	{
	public :
		ML_CHAR() : m_nGSN(0), m_nCSN(0), m_nCharPtm(0) {}
		ML_CHAR(const int64 nGSN, const wstring& strName, const int nCharPtm) 
			: m_nGSN(nGSN), m_strName(strName), m_nCharPtm(nCharPtm) 
		{
			m_nCSN = DBTestHelper::InsertCharacter(m_nGSN, GConfig::m_nMyWorldID, strName.c_str());
		}

		int64	m_nGSN;
		int64	m_nCSN;
		wstring m_strName;
		int		m_nCharPtm;
	};

	class FIXTURE
	{
	public :
		FIXTURE() : m_Sndr(1, L"sndr", 5), m_Rcvr(2, L"rcvr", 10)
		{
		}

		enum
		{
			MAIL_TYPE_USER = 1
			, MAIL_TYPE_NPC = 2
		};

		void SetGM(ML_CHAR& ch)
		{
			UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET PLAYER_GRADE = 1 WHERE CHAR_ID = %I64d;", ch.m_nCSN);
		}

	public :
		ML_CHAR m_Sndr;
		ML_CHAR m_Rcvr;		
	};

	const wstring strSQL = L"{CALL USP_RZ_MAIL_CHECK_RCVR (%d, %I64d, %d, N'%s', %d, %d)}";

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_BAD_NAME)
	{
		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, L"BAD NAME", MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(1, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_UNREG_CHAR)
	{
		UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET UNREG_DATE = '2010-01-01' WHERE CHAR_ID = %I64d;", m_Rcvr.m_nCSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(1, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_TO_ME)
	{
		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Sndr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(2, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_PLAYER_SEND_TO_GM)
	{
		SetGM(m_Rcvr);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(3, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_GM_SEND_TO_GM)
	{
		SetGM(m_Rcvr);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_GM, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_TESTER_SEND_TO_GM)
	{
		SetGM(m_Rcvr);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_TESTER, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_DEVELOPER_SEND_TO_GM)
	{
		SetGM(m_Rcvr);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_DEVELOPER, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_OVER_MAX_MAIL_COUNT)
	{
		const int64 nMAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, m_Rcvr.m_nGSN, m_Rcvr.m_nCSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(4, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_OVER_MAX_MAIL_COUNT_NPC_MAIL)
	{
		const int64 nMAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, m_Rcvr.m_nGSN, m_Rcvr.m_nCSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Rcvr.m_strName.c_str(), MT_NPC, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_OVER_MAX_MAIL_COUNT_GM_MAIL)
	{
		const int64 nMAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, m_Rcvr.m_nGSN, m_Rcvr.m_nCSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Rcvr.m_strName.c_str(), MT_GM, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, RZ_MAIL_CHECK_RCVR_OK)
	{
		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL.c_str(), GConfig::m_nMyWorldID, m_Sndr.m_nCSN, PLAYER_GRADE_NORMAL, m_Rcvr.m_strName.c_str(), MT_PLAYER, 1);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"ERRNO").AsInt());
		rs.Close();
	}
	
}