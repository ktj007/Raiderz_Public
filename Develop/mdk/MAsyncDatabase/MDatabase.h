#pragma once


#include <windows.h>
#include <sql.h>


#include <string>
#include "MADBLib.h"

namespace mdb
{
	class MSQLSTMT;


	class MADB_API MDatabase
	{
	public :
		friend class MSQLSTMT;


		MDatabase();
		virtual ~MDatabase();

		bool				Connect(const std::string& strDSN);
		bool				ConnectW(const std::wstring& strDSN);
		bool				Reconnect();
		void				Disconnect();

		bool				IsConnected();
		bool				BeginTran();
		bool				CommitTran();
		bool				RollbackTran();

		const std::wstring&	GetDSN();
		SQLINTEGER			GetDriverConcurrency();
		bool				IsScrollable();


		std::string MCleanSQLString( char* szSQL );

	protected :
		bool				SetDriverConcurrency();
		bool				CheckDriverScrollable();

		void				Reset();
		

	protected :
		SQLHDBC				m_hDBC;
		SQLHENV				m_hEnv;

		bool				m_bIsScrollable;
		SQLINTEGER			m_nDriverConcurrency;

		bool				m_bConnected;
		bool				m_bIsTransaction;

		std::wstring		m_strDSN;		
	};
}