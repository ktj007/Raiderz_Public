#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper_ServerStatus.h"


SUITE(DBTestServerStatus)
{
	GAMEDB_TEST(USP_RZ_SERVER_START)
	{
		const int		nWorldID			= 1;
		const int		nServerID			= 2;
		const wstring	wstrName(L"test_server");
		const wstring	strVersion			= L"test_version";
		const wstring	strIP				= L"127.0.0.1";
		const int		nPort				= 0;
		const uint8		nType				= SERVER_GAME;
		const int16		nMaxPlayerCnt		= 2000;
		const int16		nUpdateElapsedTm	= 30;
		const int16		nAllowDelayTm		= 30;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_SERVER_START (%d, %d, '%s', '%s', '%s', %d, %d, %d, %d, %d)}"
			, nWorldID, nServerID, wstrName.c_str(), strVersion.c_str(), strIP.c_str(), nPort, nType, nMaxPlayerCnt, nUpdateElapsedTm, nAllowDelayTm));

		TDBRecordSet rsServer;
		UTestDB.Execute(rsServer, L"SELECT NAME, CAST([VERSION] AS VARCHAR(512)) AS [VERSION], IP, PORT, [TYPE], MAX_PLAYER_COUNT, UPD_ELAPS_TIME, ALLOW_DELAY_TIME\
								  FROM dbo.RZ_SERVER\
								  WHERE WORLD_ID = %d AND SERVER_ID = %d;", nWorldID, nServerID);
		CHECK_EQUAL(1, rsServer.GetFetchedRowCount());
		CHECK(strVersion == rsServer.Field(L"VERSION").AsWString());
		CHECK(strIP == rsServer.Field(L"IP").AsWString());
		CHECK_EQUAL(nPort, rsServer.Field(L"PORT").AsInt());
		CHECK_EQUAL(SERVER_GAME, rsServer.Field(L"TYPE").AsByte());
		CHECK_EQUAL(nMaxPlayerCnt, rsServer.Field(L"MAX_PLAYER_COUNT").AsShort());
		CHECK_EQUAL(nUpdateElapsedTm, rsServer.Field(L"UPD_ELAPS_TIME").AsShort());
		CHECK_EQUAL(nAllowDelayTm, rsServer.Field(L"ALLOW_DELAY_TIME").AsShort());
		rsServer.Close();

		TDBRecordSet rsServerStatus;
		UTestDB.Execute(rsServerStatus, L"SELECT PLAYER_COUNT, SERVABLE FROM dbo.RZ_SERVER_STATUS WHERE WORLD_ID = %d AND SERVER_ID = %d;", nWorldID, nServerID);
		CHECK_EQUAL(1, rsServerStatus.GetFetchedRowCount());
		CHECK_EQUAL(false, rsServerStatus.Field(L"SERVABLE").AsBool());
		CHECK_EQUAL(0, rsServerStatus.Field(L"PLAYER_COUNT").AsShort());
		rsServerStatus.Close();
	}

	GAMEDB_TEST(USP_RZ_SERVER_UPDATE)
	{
		const int	nWorldID			= 1;
		const int	nServerID			= 2;
		const int16 nFirstPlayerCnt		= 10;
		const int16 nSecondPlayerCnt	= 20;
		const bool	bServable			= true;
		
		DBTestHelper_ServerStauts::InsertServer(nWorldID, nServerID);
		DBTestHelper_ServerStauts::InsertServerStatus(nWorldID, nServerID, nFirstPlayerCnt, 0);

		CHECK(UTestDB.Execute(L"{CALL USP_RZ_SERVER_UPDATE (%d, %d, %d, %d)}", nWorldID, nServerID, nSecondPlayerCnt, bServable));

		TDBRecordSet rsServer;
		UTestDB.Execute(rsServer, L"SELECT PLAYER_COUNT, SERVABLE FROM dbo.RZ_SERVER_STATUS WHERE WORLD_ID = %d AND SERVER_ID = %d;", nWorldID, nServerID);
		CHECK_EQUAL(nSecondPlayerCnt, rsServer.Field(L"PLAYER_COUNT").AsShort());
		CHECK_EQUAL(bServable, rsServer.Field(L"SERVABLE").AsBool());
		rsServer.Close();
	}
}