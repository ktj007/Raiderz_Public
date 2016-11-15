#include "stdafx.h"
#include "GDBManager.h"
#include "CCommandResultTable.h"


// 계정 - 기본 정보 읽기
// 캐릭터
// 아이템
// 퀘스트

GDBManager::GDBManager()
{
	m_strDSNConnect = "";
	m_DB.SetLogCallback( LogCallback );
}

GDBManager::~GDBManager()
{

}

void GDBManager::LogCallback( const string& strLog )
{
	mlog( strLog.c_str() );
}

std::string GDBManager::BuildDSNString(const std::string strDSN, const std::string strUserName, const std::string strPassword)
{
	//return m_DB.BuildDSNString("", strDSN, strUserName, strPassword );
	return "";
}

bool GDBManager::Connect(std::string strDSNConnect)
{
	//if( m_DB.Connect(strDSNConnect) )
	//{
	//	m_strDSNConnect = strDSNConnect;
	//	return true;
	//}

	return false;
}
void GDBManager::Disconnect()
{
	m_DB.Disconnect();
}

bool GDBManager::Execute(LPCTSTR szSQL)
{
	if (!m_DB.CheckOpen())
	{
		_ASSERT(0);
		return false;
	}

	try 
	{
#ifdef _DEBUG
		mlog0("DB SQL - %s\n", szSQL);
#endif
		m_DB.ExecuteSQL( szSQL );
	} 
	catch(CDBException* e)
	{
		mlog("DB Query Failed( %s ) - [ %s(%d) ]\n", szSQL, e->m_strError, e->m_nRetCode);
		_ASSERT(0);
		return false;
	}

	return true;
}


bool GDBManager::Execute(CODBCRecordset& outRecordSet, LPCTSTR szSQL)
{
	if (!m_DB.CheckOpen())
	{
		_ASSERT(0);
		return false;
	}

	try 
	{
		outRecordSet.Open(szSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		mlog("DB Query Failed( %s ) - [ %s ]\n", szSQL, e->m_strError);
		_ASSERT(0);
		return false;
	}

	if ((outRecordSet.IsOpen() == FALSE)) 
	{
		_ASSERT(0);
		return false;
	}

	return true;
}

char g_szDB_PING[] = "SELECT NULL";

bool GDBManager::Ping()
{
	CString strSQL;
	strSQL.Format(g_szDB_PING);

	if (Execute(strSQL) == false)
	{
		return false;
	}

	return true;
}

bool GDBManager::BeginTrans()
{
	return (0 == m_DB.BeginTrans()) ? false : true;
}

bool GDBManager::CommitTrans()
{
	return (0 == m_DB.CommitTrans()) ? false : true;
}

bool GDBManager::Rollback()
{
	return (0 == m_DB.Rollback()) ? false : true;
}

void GDBManager::StrZoneDescToVecZoneDesc(string& srcStrZoneDesc, vector<int>& dstVecZoneDesc)
{
	char szZoneDescription[1024+1];
	strncpy_s(szZoneDescription, srcStrZoneDesc.c_str(), min(1024, srcStrZoneDesc.size()));

	char szDelim[] = " ";
	char* szFiledID = strtok(szZoneDescription, szDelim);
	while (NULL != szFiledID)
	{
		int nFieldID = atoi(szFiledID);
		dstVecZoneDesc.push_back(nFieldID);
		szFiledID = strtok(NULL, szDelim);
	}
	string str;
}

SYSTEMTIME GDBManager::COleDateTimeToSYSTEMTIME(COleDateTime& srcDt)
{
	SYSTEMTIME st;
	st.wDay = srcDt.GetDay();
	st.wDayOfWeek = srcDt.GetDayOfWeek();
	st.wHour = srcDt.GetHour();
	st.wMinute = srcDt.GetMinute();
	st.wMonth = srcDt.GetMonth();
	st.wSecond = srcDt.GetSecond();
	st.wYear = srcDt.GetYear();

	return st;
}
