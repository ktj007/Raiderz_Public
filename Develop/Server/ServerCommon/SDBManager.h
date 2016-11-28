#pragma once

#include "MDatabase.h"
#include "MDatabaseDesc.h"
#include "ServerCommonLib.h"
#include "MTstring.h"

namespace mdb
{
	class MDBRecordSet;
}

class CSLogListener;

/// DB 관리자
class SCOMMON_API SDBManager 
{
private:
	// 비동기 모델이 고려되지 않았음. 여기 말고도 GDBTask에 리스너를 넣어줘야 함.
	CSLogListener*	m_pListener;

	void WriteDBInfo(mdb::MDatabase* pDB);

protected :
	mdb::MDatabaseDesc	m_DBDesc;
	mdb::MDatabase		m_DB;

public:
	SDBManager();
	virtual ~SDBManager();

	virtual bool Connect(mdb::MDatabaseDesc& DBDesc);
	virtual void Disconnect();
	virtual bool Execute(const wchar_t* szSQL);
	virtual bool Execute(mdb::MDBRecordSet& rs, const wchar_t* szSQL);
	virtual bool Ping();

	mdb::MDatabase* GetDatabase()	{ return &m_DB; }
	static void LogCallback( const wstring& strLog );	

	void ErrorLog(const wchar_t *pFormat, ...);
	void SetListener(CSLogListener* pListener);
};
