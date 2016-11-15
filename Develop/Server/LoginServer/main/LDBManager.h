#ifndef MMATCHDBMGR_H
#define MMATCHDBMGR_H

#include "SDBManager.h"
#include "MAsyncDatabase.h"

class SAccountCharList;
class LDBTask;
class LDBAsyncTask;
class SAsyncDB;
class LDBT_ACC_INSERT;
class LDBT_ACC_LOGIN;
class LDBT_CONN_LOG;


struct PmUserData;
struct DBP_CHAR_INSERT;

/// DB 관리자
class LDBManager : public SDBManager
{
protected :
	SAsyncDB*	m_pAccountDB;
	SAsyncDB*	m_pGameDB;
	SAsyncDB*	m_pLogDB;


private :
	bool InitAccountDB();
	bool InitGameDB();
	bool InitLogDB();

	void RegistTraceTaskID();

	void ReleaseAccountDB();
	void ReleaseGameDB();
	void ReleaseLogDB();


public:
	LDBManager();
	virtual ~LDBManager();

	void Update();
	void Release();
	mdb::MDB_AYSNC_RESULT InitAsyncDB();

	bool Post(LDBAsyncTask* pTask);

	//// 서버 정보 관련 -------------------------------------------
	virtual bool GetServerStatusList(const int nWorldID, const std::vector<int>& vecServerID);

	//// 계정 관련 ------------------------------------------------
	virtual bool LoginGetInfo(LDBT_ACC_LOGIN& data);
	virtual bool PmangLoginGetInfo(const MUID& uidPlayer, const wstring& strUserID, const PmUserData& pmangUserData);
	virtual bool ConnectLog(LDBT_CONN_LOG& data);

	virtual bool InsertAccount(LDBT_ACC_INSERT& data);
	virtual bool PmangInsertAccount(const MUID& uidPlayer, const PmUserData& pmangUserData);
	virtual void UpdaateCreateDt(const int64 nAID);
	
	
	//// 캐릭터 관리 관련 ----------------------------------------	
	virtual bool GetAccountCharList(const MUID& uidPlayer, const int64 nAID);
	virtual bool DeleteCharacter(const MUID& uidPlayer, const int64 nAID, const int64 nCID, const int nIndex);
	virtual bool InsertCharacter(const DBP_CHAR_INSERT& rfParam);	

	virtual bool ServerStatusStart(const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm);
	virtual bool ServerStatusUpdate(const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable);
	//// 월드 관련 ----------------------------------------------
	virtual bool WorldGetList(void);
};

#endif
