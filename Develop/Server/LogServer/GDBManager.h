#ifndef MMATCHDBMGR_H
#define MMATCHDBMGR_H

#include <windows.h>
#include "MDatabase.h"

#include "CSChrInfo.h"
#include "CTransData.h"

class GPlayerQuests;
class GTalentInfo;
class CODBCRecordset;
class GItemContainer;
class GQuestInfoMgr;
class GRecipeInfo;
class GItemData;

/// DB 관리자
class GDBManager 
{
private:
	std::string		m_strDSNConnect;


protected :
	MDatabase		m_DB;


private:
	// 트랜잭션
	virtual	bool BeginTrans();
	virtual	bool CommitTrans();
	virtual	bool Rollback();	

public:
	GDBManager();
	virtual ~GDBManager();

	MDatabase* GetDatabase()	{ return &m_DB; }

	std::string BuildDSNString(const std::string strDSN, const std::string strUserName, const std::string strPassword);
	bool Connect(std::string strDSNConnect);
	void Disconnect();
	static void LogCallback( const string& strLog );

	virtual bool Execute(LPCTSTR szSQL);
	virtual bool Execute(CODBCRecordset& outRecordSet, LPCTSTR szSQL);

	SYSTEMTIME COleDateTimeToSYSTEMTIME(COleDateTime& srcDt);
	void StrZoneDescToVecZoneDesc(string& srcStrZoneDesc, vector<int>& dstVecZoneDesc);

public:
	virtual bool Ping();
};


#endif