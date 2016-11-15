#include "stdafx.h"
#include "DBTestHelper_ServerStatus.h"
#include "DBTestGlobal.h"
#include "DBTestHelper.h"

bool DBTestHelper_ServerStauts::InsertServer( const int nWorldID, const int nServerID, const wstring& strName /*= "test_server"*/
											 , const wstring& strVersion /*= "test_version" */, const wstring& strIP /*= "127.168.0.1"*/, const int nPort /*= 0*/
											 , const uint8 nType /*= SERVER_GAME */, const int16 nMaxPlayerCnt /*= 2000*/
											 , const uint8 nUpdateElapsedTm /*= 30*/, const uint8 nAllowDelayTm /*= 30*/ )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_SERVER(WORLD_ID, SERVER_ID, START_DATE, NAME, [VERSION], IP, PORT, [TYPE], MAX_PLAYER_COUNT, UPD_ELAPS_TIME, ALLOW_DELAY_TIME) \
						   VALUES (%d, %d, GETDATE(), N'%s', '%s', '%s', %d, %d, %d, %d, %d);"
						   , nWorldID, nServerID, strName.c_str(), strVersion.c_str(), strIP.c_str(), nPort, nType, nMaxPlayerCnt, nUpdateElapsedTm, nAllowDelayTm);
}

bool DBTestHelper_ServerStauts::InsertServerStatus( const int nWorldID, const int nServerID, const int16 nPlayerCnt, const int16 nBefPlayerCnt, const bool bServable /*= false*/ )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_SERVER_STATUS(WORLD_ID, SERVER_ID, START_DATE, PLAYER_COUNT, UPD_DATE, SERVABLE) \
						   VALUES (%d, %d, GETDATE(), %d, GETDATE(), %d);"
		, nWorldID, nServerID, nPlayerCnt, bServable);
}