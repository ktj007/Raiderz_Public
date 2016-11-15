#include "StdAfx.h"
#include "TDatabase.h"
#include "TQuestConfig.h"

using namespace System::Windows::Forms;

TDatabase::TDatabase(void) : m_fnLogCallback( 0 )
{
	m_strDSNConnect = "";
	TDatabaseGlobal::m_DBConnection = gcnew OleDbConnection;

	this->Begin();
}

TDatabase::~TDatabase(void)
{
	this->End();
}

bool TDatabase::Begin()
{
	if (false == this->IsOpen())
	{
		String^ strDNS = this->BuildDSNString(gcnew String(TQuestConfig::m_szDB_DNS), gcnew String(TQuestConfig::m_szDB_UserName), gcnew String(TQuestConfig::m_szDB_Password));

		if (true == this->Connect(strDNS))
		{
			mlog("DBMS connected\n");
		}
		else
		{
			mlog("Can't Connect To DBMS\n");
			return false;
		}
	}

	return true;
}

void TDatabase::End()
{
	this->Disconnect();
}


bool TDatabase::CheckOpen()
{
	bool ret = true;
	if (true == IsOpen())
	{
		ret = Connect(gcnew String(m_strDSNConnect.c_str()));
		WriteLog( "TDatabase::CheckOpen - Reconnet database\n" );
	}

	return ret;
}

String^ TDatabase::BuildDSNString(String^ strDSN, String^ strUserName, String^ strPassword)
{
	String^ strDSNConnect = String::Format(
		"Provider = SQLOLEDB.1;User ID={0};Pwd={1};Data Source={2};Connect Timeout= 30", strUserName, strPassword, strDSN);

	return strDSNConnect;
}

bool TDatabase::Connect(String^ strDSNConnect)
{
	if (false == IsOpen())
	{
		TDatabaseGlobal::m_DBConnection->Close();
	}


	m_strDSNConnect = MStringToCharPointer(strDSNConnect);

	if (0 >= strDSNConnect->Length)
	{
		try
		{
			TDatabaseGlobal::m_DBConnection->Open();
		}
		catch(OleDbException^ e)
		{
			char szLog[ 256 ] = {0,};
			_snprintf_s( szLog, 255, "TDatabase::Connect - %s\n", e->Errors);
			WriteLog( szLog );
		}
	} 
	else
	{
		try
		{
			TDatabaseGlobal::m_DBConnection->ConnectionString = strDSNConnect;
			TDatabaseGlobal::m_DBConnection->Open();
		}
		catch(OleDbException^ e)
		{
			char szLog[ 256 ] = {0,};
			_snprintf_s( szLog, 255, "TDatabase::Connect - %s\n", MStringToCharPointer(e->Errors->ToString()));
			WriteLog( szLog );
			try
			{
				TDatabaseGlobal::m_DBConnection->Open();
			}
			catch(OleDbException^ e)
			{
				char szLog2[ 256 ] = {0,};
				_snprintf_s( szLog2, 255, "TDatabase::Connect - %s\n", MStringToCharPointer(e->Errors->ToString()));
				WriteLog( szLog2 );

				MessageBox::Show(e->Errors[0]->Message);
			}
		}
	}

	return IsOpen();
}

void TDatabase::Disconnect()
{
	if (true == IsOpen())
	{
		TDatabaseGlobal::m_DBConnection->Close();
	}
}


bool TDatabase::IsOpen()
{
	if (ConnectionState::Connecting == TDatabaseGlobal::m_DBConnection->State || 
		ConnectionState::Executing == TDatabaseGlobal::m_DBConnection->State ||
		ConnectionState::Fetching == TDatabaseGlobal::m_DBConnection->State ||
		ConnectionState::Open == TDatabaseGlobal::m_DBConnection->State)
	{
		return true;
	}

	return false;
}


void TDatabase::ExecuteSQL(String^ strSQL)
{
	OleDbCommand^ cmd = gcnew OleDbCommand(strSQL, TDatabaseGlobal::m_DBConnection);

	try
	{
		INT32 recordsAffected = cmd->ExecuteNonQuery();
	}
	catch(...)
	{
		throw;
	}
}

void TDatabase::ExecuteSQL(String^ strSQL, DataSet^& outDs, String^ strTable)
{
	OleDbDataAdapter^ da = gcnew OleDbDataAdapter(strSQL, TDatabaseGlobal::m_DBConnection);

	try
	{
		da->Fill(outDs, strTable);
	}
	catch(...)
	{
		throw;
	}
}



void TDatabase::WriteLog( const string& strLog )
{
	if( 0 != m_fnLogCallback  )
	{
		m_fnLogCallback( strLog );
	}
}