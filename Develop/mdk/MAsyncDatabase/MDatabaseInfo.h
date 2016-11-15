#pragma once


#include <sqltypes.h>


#include <string>
#include <vector>

#include "MADBLib.h"

namespace mdb
{
	class MSQLSTMT;


	struct MADB_API MDBSTATE
	{
		long			nNativeError;
		wchar_t			szSQLState[6];
		std::wstring	strStateInfo;

		std::string ToString();
		std::wstring ToStringW();
	};

	typedef std::vector<MDBSTATE> MDB_STATE_VEC;

	struct DBSTATE_LAST_STATE
	{
		DBSTATE_LAST_STATE();

		void Reset();

		bool bIsDisconnected;
	};


	class MADB_API MDatabaseStatus
	{
	public :
		MDatabaseStatus();
		virtual ~MDatabaseStatus();

		bool Init(MSQLSTMT& rfStmt);

		MDB_STATE_VEC&				GetStateVec();
		MDB_STATE_VEC&				GetUserStateVec();
		const DBSTATE_LAST_STATE&	GetLastChkState();

		bool InitSuccess();

		bool PushUserStatusInfo(const int nNativeError, const char* szSQLState, const std::string& strStateInfo);
		bool PushUserStatusInfoW(const int nNativeError, const wchar_t* szSQLState, const std::wstring& strStateInfo);
		

	protected :
		virtual void PreCheckState(const MDBSTATE& dbs);

	
	protected :
		bool				m_bInitedState;
		MDB_STATE_VEC		m_vecState;
		MDB_STATE_VEC		m_vecUserState;
		DBSTATE_LAST_STATE	m_DBLastState;
	};
}