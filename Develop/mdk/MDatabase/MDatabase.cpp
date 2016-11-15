#include "StdAfx.h"
#include "mdatabase.h"

MDatabase::MDatabase(void) : m_fnLogCallback( 0 )
{
	m_dwOptions = 0;
	m_dwOptions |= CDatabase::noOdbcDialog;
	m_dwOptions |= CDatabase::useCursorLib;
}

MDatabase::~MDatabase(void)
{
}


bool MDatabase::CheckOpen()
{
	if (!m_DB.IsOpen())
	{
		_ASSERT(0);
		WriteLog( "MDatabase::CheckOpen - Reconnet database\n" );
		return Connect(m_Desc);		
	}

	return true;
}

std::string MDatabase::BuildDSNString( const std::string strServer, const std::string strDatabaseName, const std::string strUserName, const std::string strPassword )
{
	// SQL2008 : "Driver={SQL Server Native Client 10.0};Server=myServerAddress;Database=myDataBase;Uid=myUsername;Pwd=myPassword;"
	// SQL2005 : "Driver={SQL Native Client};Server=myServerAddress;Database=myDataBase;Uid=myUsername;Pwd=myPassword;"
	// Âü°í : http://www.connectionstrings.com/

	std::string strDSNConnect = std::string("Driver={SQL Native Client};Server=") + strServer + 
					std::string(";Database=") + strDatabaseName +
					std::string(";Uid=") + strUserName +
					std::string(";Pwd=") + strPassword +
					std::string(";");

	return strDSNConnect;
}

bool MDatabase::Connect(MDatabaseDesc& desc)
{
	if (m_DB.m_hdbc && m_DB.IsOpen()) m_DB.Close();

	m_Desc = desc;
	std::string strDSNConnect = BuildDSNString(m_Desc.strServer, m_Desc.strDatabaseName, m_Desc.strUserName, m_Desc.strPassword);

	BOOL bRet = FALSE;
	if (strDSNConnect.empty()) 
	{
		try 
		{
			bRet = m_DB.Open(NULL);
		} 
		catch(CDBException* e) 
		{
			char szLog[ 256 ] = {0,};
			_snprintf_s( szLog, 255, "MDatabase::Connect - %s\n", e->m_strError );
			WriteLog( szLog );
		}
	} 
	else 
	{
		try 
		{
			bRet = m_DB.OpenEx( CString(strDSNConnect.c_str()), m_dwOptions );
		} 
		catch(CDBException* e) 
		{
			char szLog[ 512 ] = {0,};
			_snprintf_s( szLog, 512, "MDatabase::Connect - %s\n", e->m_strError );
			WriteLog( szLog );
		}
	}
	if (bRet == TRUE) 
	{
		m_DB.SetQueryTimeout(60);
		return true;
	} 
	else 
	{
		OutputDebugString("DATABASE Error \n");
		return false;
	}
}

void MDatabase::Disconnect()
{
	if (m_DB.IsOpen())
		m_DB.Close();
}


BOOL MDatabase::IsOpen() const
{
	return m_DB.IsOpen();
}

BOOL MDatabase::BeginTrans()
{
	return m_DB.BeginTrans();
}

BOOL MDatabase::CommitTrans()
{
	return m_DB.CommitTrans();
}

BOOL MDatabase::Rollback()
{
	return m_DB.Rollback();
}

void MDatabase::ExecuteSQL( LPCTSTR lpszSQL )
{
	m_DB.ExecuteSQL( lpszSQL );
}


void MDatabase::WriteLog( const string& strLog )
{
	if( 0 != m_fnLogCallback  )
	{
		m_fnLogCallback( strLog );
	}
}