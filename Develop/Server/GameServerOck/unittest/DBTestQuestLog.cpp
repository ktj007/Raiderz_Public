#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GTimeCalculator.h"
#include "GDB_CODE.h"


SUITE(DBTET_QUEST_LOG)
{
	const int64		nGSN			= 1;
	const int		nWORLD			= 2;
	const int64		nCSN			= 3;
	const int		nCHAR_PTM		= 4;
	const int		nQUEST_ID		= 5;
	const wstring	strACCP_DATE	= L"2010-8-31 5:57:48";
	const int		nEXP			= 6;
	const uint8		nLEV			= 7;
	const int		nFIELD_ID		= 8;

	LOGDB_TEST(USP_RZ_QUEST_FAIL_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_FAIL_LOG (%I64d, %d, %I64d, %d, %d, '%s', %d)}"
			, nGSN, nWORLD, nCSN, nCHAR_PTM, nQUEST_ID, strACCP_DATE.c_str(), nFIELD_ID);

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_PTM, QUEST_ID, ACCP_DATE, FIELD_ID \
								FROM dbo.RZ_QUEST_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 503;"
								, nGSN, nWORLD, nCSN);
		CHECK_EQUAL(nCHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nQUEST_ID, rsLog.Field(L"QUEST_ID").AsInt());
		CHECK(strACCP_DATE == rsLog.Field(L"ACCP_DATE").AsWString());
		CHECK_EQUAL(nFIELD_ID, rsLog.Field(L"FIELD_ID").AsInt());
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_QUEST_DONE_LOG)
	{
		const int nMONEY		= 10;
		const int nDELTA_MONEY	= 4;

		UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_DONE_LOG (%I64d, %d, %I64d, %d, %d, '%s')}"
			, nGSN, nWORLD, nCSN, nCHAR_PTM, nQUEST_ID, strACCP_DATE.c_str());

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_PTM, QUEST_ID, ACCP_DATE \
								FROM dbo.RZ_QUEST_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 505;"
								, nGSN, nWORLD, nCSN);
		CHECK_EQUAL(nCHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nQUEST_ID, rsLog.Field(L"QUEST_ID").AsInt());
		CHECK(strACCP_DATE == rsLog.Field(L"ACCP_DATE").AsWString());		
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_QUEST_COMPLETE_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_COMPLETE_LOG (%I64d, %d, %I64d, %d, %d, '%s', %d)}"
			, nGSN, nWORLD, nCSN, nCHAR_PTM, nQUEST_ID, strACCP_DATE.c_str(), nFIELD_ID);

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_PTM, QUEST_ID, ACCP_DATE, FIELD_ID \
								FROM dbo.RZ_QUEST_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 502;"
								, nGSN, nWORLD, nCSN);
		CHECK_EQUAL(nCHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nQUEST_ID, rsLog.Field(L"QUEST_ID").AsInt());
		CHECK(strACCP_DATE == rsLog.Field(L"ACCP_DATE").AsWString());
		CHECK_EQUAL(nFIELD_ID, rsLog.Field(L"FIELD_ID").AsInt());
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_QUEST_GIVEUP_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_GIVEUP_LOG (%I64d, %d, %I64d, %d, %d, '%s', %d)}"
			, nGSN, nWORLD, nCSN, nCHAR_PTM, nQUEST_ID, strACCP_DATE.c_str(), nFIELD_ID);

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT CHAR_PTM, QUEST_ID, ACCP_DATE, FIELD_ID \
								FROM dbo.RZ_QUEST_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 504;"
								, nGSN, nWORLD, nCSN);
		CHECK_EQUAL(nCHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nQUEST_ID, rsLog.Field(L"QUEST_ID").AsInt());
		CHECK(strACCP_DATE == rsLog.Field(L"ACCP_DATE").AsWString());
		CHECK_EQUAL(nFIELD_ID, rsLog.Field(L"FIELD_ID").AsInt());
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_QPVP_MAIL_LOG)
	{
		const int64		MAIL_UID		= 5;
		const wstring	REG_DATE		= GTimeCalculator::GetTimeAsString(GTimeSec(0));
		const uint8		RECV_MAIL_TYPE	= 3;
		const int		EVENT_ID		= 5;
		
		UTestDB.Execute(L"{CALL dbo.USP_RZ_QPVP_MAIL_LOG (%I64d, %d, %I64d, %s, %d, %d, %d, %I64d, %d)}"
			, nGSN, nWORLD, nCSN, REG_DATE.c_str(), nCHAR_PTM, EVENT_ID, nFIELD_ID, MAIL_UID, RECV_MAIL_TYPE);

		// MAIL LOG
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT REG_DATE, CHAR_PTM, MAIL_TYPE, MAIL_ID \
								 FROM dbo.RZ_MAIL_LOG \
								 WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 1202;"
								 , nGSN, nWORLD, nCSN);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK(REG_DATE == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			CHECK_EQUAL(nCHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(RECV_MAIL_TYPE, rs.Field(L"MAIL_TYPE").AsByte());
			CHECK_EQUAL(MAIL_UID, rs.Field(L"MAIL_ID").AsInt64());
			rs.Close();
		}

		// QUEST LOG
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT CHAR_PTM, QUEST_ID, EVENT_ID, FIELD_ID, MAIL_ID, REG_DATE \
								 FROM dbo.RZ_QUEST_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = %d;"
								 , nGSN, nWORLD, nCSN, GDB_CODE::CD_L_QPVP_REWARD);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK(REG_DATE == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			CHECK_EQUAL(nCHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(0, rs.Field(L"QUEST_ID").AsInt());
			CHECK_EQUAL(EVENT_ID, rs.Field(L"EVENT_ID").AsByte());
			CHECK_EQUAL(nFIELD_ID, rs.Field(L"FIELD_ID").AsInt());
			CHECK_EQUAL(MAIL_UID, rs.Field(L"MAIL_ID").AsInt64());
			rs.Close();
		}		
	}

	LOGDB_TEST(USP_RZ_QPVP_INVEN_LOG)
	{
		const int64		MAIL_UID		= 5;
		const wstring	REG_DATE		= GTimeCalculator::GetTimeAsString(GTimeSec(0));
		const int		EVENT_ID		= 5;		

		UTestDB.Execute(L"{CALL dbo.USP_RZ_QPVP_INVEN_LOG (%I64d, %d, %I64d, %s, %d, %d, %d)}"
			, nGSN, nWORLD, nCSN, REG_DATE.c_str(), nCHAR_PTM, EVENT_ID, nFIELD_ID);

		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT CHAR_PTM, QUEST_ID, EVENT_ID, FIELD_ID, MAIL_ID, REG_DATE \
								 FROM dbo.RZ_QUEST_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = %d;"
								 , nGSN, nWORLD, nCSN, GDB_CODE::CD_L_QPVP_REWARD);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK(REG_DATE == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			CHECK_EQUAL(nCHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(0, rs.Field(L"QUEST_ID").AsInt());
			CHECK_EQUAL(EVENT_ID, rs.Field(L"EVENT_ID").AsByte());
			CHECK_EQUAL(nFIELD_ID, rs.Field(L"FIELD_ID").AsInt());
			CHECK(rs.Field(L"MAIL_ID").IsNull());
			rs.Close();
		}		
	}
}