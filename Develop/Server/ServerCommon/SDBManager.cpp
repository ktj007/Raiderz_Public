#include "stdafx.h"
#include "SDBManager.h"
#include "CSLogListener.h"
#include "MDBRecordSet.h"
#include "MLocale.h"

SDBManager::SDBManager()
: m_pListener(NULL)
{
}

SDBManager::~SDBManager()
{

}

void SDBManager::LogCallback( const wstring& strLog )
{
	mlog( strLog.c_str() );
}


bool SDBManager::Ping()
{
	return Execute(L"SELECT NULL");
}

bool SDBManager::Connect( mdb::MDatabaseDesc& DBDesc)
{
	m_DBDesc = DBDesc;

	return m_DB.Connect(DBDesc.BuildDSNString());		
}


void SDBManager::Disconnect()
{
	m_DB.Disconnect();
}

void SDBManager::ErrorLog(const wchar_t *pFormat, ...)
{
	wchar_t temp[8192];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);


	if (m_pListener)
	{
		m_pListener->OnLog(MLocale::ConvTCHARToAnsi(temp).c_str());
	}
	
	MLog3(temp);
}

bool SDBManager::Execute(const wchar_t* szSQL )
{
	bool isExecute = false;
	mdb::MDatabaseQuery dbq(&m_DB);	
	
	isExecute = dbq.Execute(MLocale::ConvUTF16ToAnsi(szSQL));

	if (!isExecute)
	{
		WriteDBInfo(&m_DB);		
	}

	return isExecute;	
}

bool SDBManager::Execute( mdb::MDBRecordSet& rs, const wchar_t* szSQL )
{
	bool isOpen = false;

	isOpen = rs.Open(MLocale::ConvUTF16ToAnsi(szSQL));

	if (!isOpen)
	{
		WriteDBInfo(rs.GetDatabase());		
	}

	return isOpen;
}

void SDBManager::SetListener( CSLogListener* pListener )
{
	m_pListener = pListener;
}

void SDBManager::WriteDBInfo( mdb::MDatabase* pDB )
{
	if (NULL == pDB)
	{
		ErrorLog(L"DB ERROR : NULL POINT.\n");
	}

	mdb::MSQLSTMT stmt;
	if (!stmt.Init(&m_DB))
	{
		ErrorLog(L"DB ERROR : init database statement failed.\n");
		return;
	}

	mdb::MDatabaseStatus st;
	if (!st.Init(stmt))
	{
		ErrorLog(L"DB ERROR : init database info failed.\n");
		return;
	}

	wchar_t szError[8192];

	mdb::MDB_STATE_VEC& vState = st.GetStateVec();
	for (size_t i = 0; i < vState.size(); ++i)
	{
		_snwprintf_s(szError, 8192, 8191, L"  DB ERROR : NUM(%d), %s\n"
			, i
			, vState[i].ToStringW().c_str());

		ErrorLog(L"%s\n", szError);
	}
}


