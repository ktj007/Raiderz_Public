#ifndef _SDBTASK_EXECUTER_H
#define _SDBTASK_EXECUTER_H


#include <string>
#include "MLocale.h"

static const int GDB_MAX_REPEAT_QUERY_COUNT = 3;


template<typename SrcType>
class SDBTaskExecuter
{
public :
	SDBTaskExecuter(SrcType& Src) : m_Src(Src), m_pStatus(NULL) {}
	~SDBTaskExecuter() 
	{ 
		SAFE_DELETE(m_pStatus); 
	}

	mdb::MDatabase* GetDB() { return m_Src.GetDatabase(); }

	mdb::MDatabaseStatus* GetStatus() 
	{ 
		mdb::MDatabaseStatus* pStatus = m_pStatus; 
		m_pStatus = NULL; 
		return pStatus; 
	}
	
	bool Init()
	{
		_ASSERT(NULL != GetDB());

		if (NULL == GetDB())	return false;

		return true;
	}

	bool Execute(const std::wstring& strSQL)
	{
		int nReturn = 0;

		return Execute(strSQL, nReturn, false);
	}

	bool Execute(const std::wstring& strSQL, int& nRefReturn)
	{
		return Execute(strSQL, nRefReturn, true);
	}

	bool ExecuteW(const std::wstring& strSQL)
	{
		int nReturn = 0;

		return ExecuteW(strSQL, nReturn, false);
	}

	bool ExecuteW(const std::wstring& strSQL, int& nRefReturn)
	{
		return ExecuteW(strSQL, nRefReturn, true);
	}

	bool CheckDBConnection()
	{
		if (!GetDB()->IsConnected())
		{
			if (!ReconnectDB())
				return false;		
		}

		return true;
	}

	bool ReconnectDB()
	{
		if (!GetDB()->Reconnect())
		{
			if (NULL != m_pStatus)
				m_pStatus->PushUserStatusInfo(0, "-----", "RECONNECT DB FAIL!!!");

			return false;		
		}

		if (NULL != m_pStatus)
			m_pStatus->PushUserStatusInfo(0, "-----", "RECONNECT DB SUCCESS!!!");

		return true;
	}

	bool BuildStatus()
	{
		if (NULL == m_pStatus)
			m_pStatus = new mdb::MDatabaseStatus;

		if (!m_pStatus->Init(m_Src.GetSQLSTMT()))
		{
			SAFE_DELETE(m_pStatus);
			m_pStatus = NULL;
			return false;
		}

		return true;
	}

	bool IsDisconnectedDBStatus()
	{
		if (NULL == m_pStatus)
			return false;

		return m_pStatus->GetLastChkState().bIsDisconnected;
	}

private :
	bool Execute(const std::wstring& strSQL, int& nRefReturn, bool bHaveReturn)
	{
		if (!Init()) 				return false;
		if (strSQL.empty())			return false;
		if (!CheckDBConnection())	return false;

		for (int i = 0; i < GDB_MAX_REPEAT_QUERY_COUNT; ++i)
		{
			if (bHaveReturn)
			{
				if (m_Src.Execute(MLocale::ConvUTF16ToAnsi(strSQL.c_str()), nRefReturn))
					return true;
			}
			else
			{
				if (m_Src.Execute(MLocale::ConvUTF16ToAnsi(strSQL.c_str())))
					return true;
			}			

			if (!BuildStatus())
				return false;

			m_Src.Close();

			if (IsDisconnectedDBStatus())
			{
				if (!ReconnectDB())
					return false;
			}		
			else // 디비와 접속이 끊어졌을 경우만 쿼리를 다시 실행한다.
			{
				return false;
			}
		}

		return true;
	}

	bool ExecuteW(const std::wstring& strSQL, int& nRefReturn, bool bHaveReturn)
	{
		if (!Init()) 				return false;
		if (strSQL.empty())			return false;
		if (!CheckDBConnection())	return false;

		for (int i = 0; i < GDB_MAX_REPEAT_QUERY_COUNT; ++i)
		{
			if (bHaveReturn)
			{
				if (m_Src.ExecuteW(strSQL, nRefReturn))
					return true;
			}
			else
			{
				if (m_Src.ExecuteW(strSQL))
					return true;
			}

			if (!BuildStatus())
				return false;

			m_Src.Close();

			if (IsDisconnectedDBStatus())
			{
				if (!ReconnectDB())
					return false;
			}		
			else // 디비와 접속이 끊어졌을 경우만 쿼리를 다시 실행한다.
			{
				return false;
			}
		}

		return true;
	}

private :
	mdb::MDatabaseStatus*	m_pStatus;
	SrcType&				m_Src;
};


#endif