#pragma once

#include "ZDBManager.h"

class MockDBManager : public ZDBManager
{
public:
	MockDBManager();
	virtual ~MockDBManager();

	// ServerStatus
	virtual bool ServerStatusStart(const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm) override { return true; }
	virtual bool ServerStatusUpdate(const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable) override { return true; }
};
