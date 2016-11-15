#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GTimeCalculator.h"

SUITE(WORLD)
{
	const int		WORLD				= 1;
	const wstring	NAME				= L"TEST_WORLD";
	const wstring	IP					= L"127.0.0.1";
	const uint8		TYPE				= 2;
	const int		MAX_PLAYER_COUNT	= 3;
	const int		CUR_PLAYER_COUNT	= 4;
	const uint8		SERVABLE			= 5;
	const uint8		STATE				= 6;
	const int		MAX_ELAPSED_TIME	= 7;
	const wstring	REG_DATE			= GTimeCalculator::GetTimeAsString(GTimeSec(0));

	ACCDB_TEST(USP_RZ_WORLD_INSERT)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_WORLD_INSERT (%d, N'%s', '%s', %d, %d, %d, %d, %d)}"
			, WORLD, NAME.c_str(), IP.c_str(), TYPE, MAX_PLAYER_COUNT, SERVABLE, STATE, MAX_ELAPSED_TIME);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT NAME, IP, TYPE, CUR_PLAYER_COUNT, MAX_PLAYER_COUNT, SERVABLE, STATE, MAX_ELAPSED_TIME \
							FROM dbo.RZ_WORLD \
							WHERE WORLD_ID = %d", WORLD);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK(NAME == rs.Field(L"NAME").AsWString());
		CHECK(IP == rs.Field(L"IP").AsWString());
		CHECK_EQUAL(TYPE, rs.Field(L"TYPE").AsByte());
		CHECK_EQUAL(0, rs.Field(L"CUR_PLAYER_COUNT").AsInt());
		CHECK_EQUAL(MAX_PLAYER_COUNT, rs.Field(L"MAX_PLAYER_COUNT").AsInt());
		CHECK_EQUAL(SERVABLE, rs.Field(L"SERVABLE").AsByte());
		CHECK_EQUAL(STATE, rs.Field(L"STATE").AsByte());
		CHECK_EQUAL(MAX_ELAPSED_TIME, rs.Field(L"MAX_ELAPSED_TIME").AsInt());
		rs.Close();
	}

	ACCDB_TEST(USP_RZ_WORLD_UPDATE)
	{
		UTestDB.Execute(L"INSERT INTO dbo.RZ_WORLD(WORLD_ID, NAME, IP) VALUES (%d, N'%s', N'');"
			, WORLD, NAME.c_str());

		UTestDB.Execute(L"{CALL dbo.USP_RZ_WORLD_UPDATE (%d, %d, %d)}"
			, WORLD, CUR_PLAYER_COUNT, SERVABLE);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT CUR_PLAYER_COUNT, SERVABLE, LAST_UPDATE_DATE FROM dbo.RZ_WORLD WHERE WORLD_ID = %d;"
			, WORLD);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(CUR_PLAYER_COUNT, rs.Field(L"CUR_PLAYER_COUNT").AsInt());
		CHECK_EQUAL(SERVABLE, rs.Field(L"SERVABLE").AsByte());
		CHECK(!rs.Field(L"LAST_UPDATE_DATE").IsNull());
		rs.Close();
	}

	struct T_WORLD
	{
		T_WORLD(int nWorld, wstring& strName, wstring& strIP, uint8 nType, int nCurPlayerCount, int nMaxPlayerCount, uint8 nServable, uint8 nState, int nMaxElapsedTime, wstring& strEndDate)
			: m_nWorld(nWorld), m_strName(strName), m_strIP(strIP), m_nType(nType), m_nCurPlayerCount(nCurPlayerCount), m_nMaxPlayerCount(nMaxPlayerCount), m_nMaxElapsedTime(nMaxElapsedTime), m_strEndDate(strEndDate)
			, m_nServable(nServable), m_nState(nState)
		{
			
		}

		int			m_nWorld;
		wstring		m_strName;
		wstring		m_strIP;
		uint8		m_nType;
		int			m_nCurPlayerCount;
		int			m_nMaxPlayerCount;
		uint8		m_nServable;
		uint8		m_nState;
		int			m_nMaxElapsedTime;
		wstring		m_strEndDate;
	};

	void InsertWorld(const T_WORLD& tw)
	{
		UTestDB.Execute(L"INSERT INTO dbo.RZ_WORLD(WORLD_ID, NAME, IP, TYPE, CUR_PLAYER_COUNT, MAX_PLAYER_COUNT, SERVABLE, STATE, LAST_UPDATE_DATE, MAX_ELAPSED_TIME, START_DATE, END_DATE) \
						 VALUES (%d, N'%s', '%s', %d, %d, %d, %d, %d, GETDATE(), %d, GETDATE(), '%s')"
						 , tw.m_nWorld, tw.m_strName.c_str(), tw.m_strIP.c_str(), tw.m_nType
						 , tw.m_nCurPlayerCount, tw.m_nMaxPlayerCount, tw.m_nServable, tw.m_nState, tw.m_nMaxElapsedTime, tw.m_strEndDate.c_str());
	}

	ACCDB_TEST(USP_RZ_WORLD_GET_LIST)
	{
		const T_WORLD w1(1, wstring(L"w1"), wstring(L"ip1"), 1, 2, 3, 1, 0, 30, wstring(L"9999-12-31 00:00"));
		const T_WORLD w2(2, wstring(L"w2"), wstring(L"ip2"), 2, 3, 4, 1, 1, 30, wstring(L"9999-12-31 00:00"));		

		InsertWorld(w1);
		InsertWorld(w2);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_WORLD_GET_LIST}");
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(w2.m_nWorld, rs.Field(L"WORLD_ID").AsInt());
		CHECK(w2.m_strName == rs.Field(L"NAME").AsWString());
		CHECK(w2.m_strIP == rs.Field(L"IP").AsWString());
		CHECK_EQUAL(w2.m_nType, rs.Field(L"TYPE").AsByte());
		CHECK_EQUAL(w2.m_nCurPlayerCount, rs.Field(L"CUR_PLAYER_COUNT").AsInt());
		CHECK_EQUAL(w2.m_nMaxPlayerCount, rs.Field(L"MAX_PLAYER_COUNT").AsInt());
		CHECK_EQUAL(w2.m_nServable, rs.Field(L"SERVABLE").AsByte());
		CHECK_EQUAL(0, rs.Field(L"EXPIRE").AsByte());
		rs.Close();
	}
}