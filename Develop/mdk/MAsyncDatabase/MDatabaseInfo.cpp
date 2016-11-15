#include "stdafx.h"
#include "MDatabaseInfo.h"
#include "MDatabaseDefine.h"
#include "MSQLSTMT.h"
#include "MLocale.h"


namespace mdb
{
	DBSTATE_LAST_STATE::DBSTATE_LAST_STATE()
	{
		Reset();
	}

	void DBSTATE_LAST_STATE::Reset()
	{
		bIsDisconnected = false;
	}

	std::string MDBSTATE::ToString()
	{
		return MLocale::ConvUTF16ToAnsi(ToStringW().c_str());
	}

	std::wstring MDBSTATE::ToStringW()
	{
		wchar_t szState[8192];
		_snwprintf_s(szState, 8192, 8191, L"NativeErr:%d, SQLSTATE[%s], StateInfo:%s\0"
			, nNativeError
			, szSQLState
			, strStateInfo.c_str());

		return std::wstring(szState);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	MDatabaseStatus::MDatabaseStatus() : m_bInitedState(false)
	{

	}

	MDatabaseStatus::~MDatabaseStatus()
	{

	}

	bool MDatabaseStatus::Init( MSQLSTMT& rfStmt )
	{
		_ASSERT(SQL_NULL_HSTMT != rfStmt.GetSTMT());

		if (SQL_NULL_HSTMT == rfStmt.GetSTMT())
		{
			PushUserStatusInfo(0, "00000", "NULL SQL Handler");
			return false;
		}

		m_DBLastState.Reset();
		m_vecState.clear();
		m_vecUserState.clear();
		m_bInitedState = false;
		
		wchar_t			szSqlState[6];
		wchar_t			szStateInfo[MDB_MAX_MESSAGE_LENGTH];
		short			nStateInfoLen;
		long			nNativeError;
		
		SQLSMALLINT i = 1;

		while (SQL_NO_DATA != SQLGetDiagRecW(SQL_HANDLE_STMT
			, rfStmt.GetSTMT()
			, i++
			, (SQLWCHAR*)szSqlState
			, &nNativeError
			, (SQLWCHAR*)szStateInfo
			, SQL_MAX_MESSAGE_LENGTH
			, &nStateInfoLen))
		{
			MDBSTATE dbs;

			dbs.nNativeError = nNativeError;
			dbs.strStateInfo = szStateInfo;

			wcsncpy_s(dbs.szSQLState, szSqlState, 6);

			PreCheckState(dbs);

			m_vecState.push_back(dbs);
		}

		m_bInitedState = true;

		return true;
	}

	MDB_STATE_VEC& MDatabaseStatus::GetStateVec()
	{
		return m_vecState;
	}

	MDB_STATE_VEC& MDatabaseStatus::GetUserStateVec()
	{
		return m_vecUserState;
	}

	void MDatabaseStatus::PreCheckState( const MDBSTATE& dbs )
	{
		static const wstring strDisconnectedState(L"08S01");

		if (strDisconnectedState == dbs.szSQLState)
		{
			m_DBLastState.bIsDisconnected = true;
		}		
	}

	const DBSTATE_LAST_STATE& MDatabaseStatus::GetLastChkState()
	{
		return m_DBLastState;
	}

	bool MDatabaseStatus::InitSuccess()
	{
		return m_bInitedState;
	}

	bool MDatabaseStatus::PushUserStatusInfo( const int nNativeError, const char* szSQLState, const std::string& strStateInfo )
	{
		return PushUserStatusInfoW(nNativeError, MLocale::ConvAnsiToUCS2(szSQLState).c_str(), MLocale::ConvAnsiToUCS2(strStateInfo.c_str()));
	}

	bool MDatabaseStatus::PushUserStatusInfoW( const int nNativeError, const wchar_t* szSQLState, const std::wstring& strStateInfo )
	{
		if (5 != wcslen(szSQLState))
			return false;

		MDBSTATE dbs;

		dbs.nNativeError = nNativeError;
		dbs.strStateInfo = strStateInfo;

		wcsncpy_s(dbs.szSQLState, 6, szSQLState, 5);
		dbs.szSQLState[5] = L'\0';

		m_vecUserState.push_back(dbs);

		return true;
	}
}
