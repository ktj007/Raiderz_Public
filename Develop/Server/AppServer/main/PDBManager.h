#pragma once

#include "SDBManager.h"
#include "MAsyncDatabase.h"

class SAsyncDB;
class SDBAsyncTask;

class PDBManager : public SDBManager
{
public:
	PDBManager();
	virtual ~PDBManager();

	bool Init(mdb::MDatabaseDesc& dbGameDBDesc
		, mdb::MDatabaseDesc& dbLogDBDesc);
	void Release();
	void Update();

	bool InitGameDB(mdb::MDatabaseDesc& dbGameDBDesc);
	void ReleaseGameDB();

	bool InitLogDB(mdb::MDatabaseDesc& dbLogDBDesc);
	void ReleaseLogDB();

	bool Post(SDBAsyncTask* pTask);

	virtual bool ServerStatusStart(const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm);
	virtual bool ServerStatusUpdate(const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable);

protected :
	SAsyncDB* m_pGameDB;
	SAsyncDB* m_pLogDB;
};
