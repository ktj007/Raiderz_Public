#ifndef _DBTEST_HELPER_SERVER_STATUS_H
#define _DBTEST_HELPER_SERVER_STATUS_H


class DBTestHelper_ServerStauts
{
public :
	static bool InsertServer(const int nWorldID, const int nServerID, const wstring& strName = L"test_server", const wstring& strVersion = L"test_version"
						, const wstring& strIP = L"127.168.0.1", const int nPort = 0, const uint8 nType = SERVER_GAME
						, const int16 nMaxPlayerCnt = 2000, const uint8 nUpdateElapsedTm = 30, const uint8 nAllowDelayTm = 30);
	static bool InsertServerStatus(const int nWorldID, const int nServerID, const int16 nPlayerCnt, const int16 nBefPlayerCnt, const bool bServable = false);

};


#endif