#include "stdafx.h"
#include "SUnitTestDBBuilder.h"
#include "DBTestGlobal.h"
#include "UTestDatabase.h"
#include "MStringUtil.h"
#include "MLocale.h"

SUnitTestDBBuilder::SUnitTestDBBuilder() : m_bWasCreateDB(false)
{

}

SUnitTestDBBuilder::~SUnitTestDBBuilder()
{

}

bool SUnitTestDBBuilder::LoadDropTableOrder(const wstring& strFileName)
{
	MXml mx;
	if (!mx.LoadFile(MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str()))
		return false;

	MXmlHandle		docHandle	= mx.DocHandle();
	MXmlElement*	pElement	= docHandle.FirstChild("maiet").FirstChildElement().Element();

	for (; NULL != pElement; pElement=pElement->NextSiblingElement())
	{
		if (!_stricmp("TABLE", pElement->Value()))
		{
			MXmlAttribute* pColAttr =  pElement->FirstAttribute();
			if (NULL == pColAttr)
				continue;

			if (!_stricmp("Name", pColAttr->Name()))
			{
				string strDropTable;
				strDropTable = "DROP TABLE dbo." + mx.ToAnsi(pColAttr->Value()) + ";";
				
				
				m_strDropTable.append(MLocale::ConvAnsiToUCS2(strDropTable.c_str()).c_str());
			}			
		}		

	}

	return true;
}

bool SUnitTestDBBuilder::LoadScript( vector<wstring>& vecSQL, const wstring& strFielName )
{
	wstring strSQL;
	wchar_t szBuff[1024] = {0};

	FILE* fp = _wfopen(strFielName.c_str(), L"r");
	if (NULL == fp)
		return false;

	while (NULL != fgetws(szBuff, 1024, fp))
	{
		if (0 == wcsncmp(L"GO", cml2::ToUpper(szBuff).c_str(), 2))
		{
			strSQL.append(L"\n");
			vecSQL.push_back(strSQL);
			strSQL.clear();
			continue;
		}

		strSQL.append(szBuff);
	}

	fclose(fp);

	return true;
}

bool SUnitTestDBBuilder::DropTable()
{
	mdb::MDatabaseQuery q(GetUTestDB().GetDB());
	return q.Execute(MLocale::ConvUTF16ToAnsi(m_strDropTable.c_str()));
}

bool SUnitTestDBBuilder::ScriptFlush( vector<wstring>& vecSQL )
{
	for (size_t i = 0; i < vecSQL.size(); ++i)
	{
		mdb::MDatabaseQuery q(GetUTestDB().GetDB());
		if (!q.Execute(MLocale::ConvUTF16ToAnsi(vecSQL[i].c_str())))
			return false;

		q.Close();
	}

	return true;
}

bool SUnitTestDBBuilder::LoadTableScript( const wstring& strFielName )
{
	return LoadScript(m_vecTable, strFielName);
}

bool SUnitTestDBBuilder::LoadProcScript( const wstring& strFielName )
{
	return LoadScript(m_vecProc, strFielName);
}

bool SUnitTestDBBuilder::LoadFuncScript(const wstring& strFielName)
{
	return LoadScript(m_vecFunc, strFielName);	
}

bool SUnitTestDBBuilder::SQLFlush( const wstring& strSQL )
{
	mdb::MDatabaseQuery q(GetUTestDB().GetDB());
	if (!q.Execute(MLocale::ConvUTF16ToAnsi(strSQL.c_str())))
		return false;
	q.Close();

	return true;
}

void SUnitTestDBBuilder::MakeTableSQL()
{
	for (size_t i = 0; i < m_vecTable.size(); ++i)
	{
		m_strTable.append(m_vecTable[i]);
	}
}

wstring SUnitTestDBBuilder::GetMyComputerName()
{
	wchar_t	szBuffer[MAX_COMPUTERNAME_LENGTH + 1] = {0,};
	DWORD	dwNameSize = MAX_COMPUTERNAME_LENGTH + 1;

	int i;
	for (i = 0; i < ComputerNameMax; ++i)
	{
		if (TRUE != GetComputerNameEx((COMPUTER_NAME_FORMAT)i, szBuffer, &dwNameSize))
		{
			_ASSERT(false);
			return L"";
		}

		if (0 != dwNameSize)
			break;		
	}	

	_ASSERT(i < ComputerNameMax);

	return szBuffer;
}

const mdb::MDatabaseDesc SUnitTestDBBuilder::GetDesc()
{
	mdb::MDatabaseDesc desc = m_DatabaseDesc;
	desc.strDatabaseName += L"_" + GetMyComputerName();
	
	return desc;
}

void SUnitTestDBBuilder::SetDesc( const string& strODBCDriver, const string& strServer, const string& strDB, const string& strUser, const string& strPasswd )
{
	m_DatabaseDesc.strDriver		= MLocale::ConvAnsiToUTF16(strODBCDriver.c_str());
	m_DatabaseDesc.strServer		= MLocale::ConvAnsiToUTF16(strServer.c_str());
	m_DatabaseDesc.strDatabaseName	= MLocale::ConvAnsiToUTF16(strDB.c_str());
	m_DatabaseDesc.strUserName		= MLocale::ConvAnsiToUTF16(strUser.c_str());
	m_DatabaseDesc.strPassword		= MLocale::ConvAnsiToUTF16(strPasswd.c_str());
}

void SUnitTestDBBuilder::CreateDBContainer()
{
	m_UTestDBContainer.Create();
	if (!m_UTestDBContainer.GetUTestDB().ConnectTestDB(GetDesc()))
	{
		mlog("Unittest db connect fail.\n");
	}
}

void SUnitTestDBBuilder::ReleaseDBContainer()
{
	m_UTestDBContainer.Release();
}

void SUnitTestDBBuilder::CreateTestDB()
{
	if (m_bWasCreateDB)
	{
		DropTable();
		SQLFlush(m_strTable);

	}
	else
	{
		SQLFlush(m_strTable);
		ScriptFlush(m_vecFunc);
		ScriptFlush(m_vecProc);		
	}	

	m_bWasCreateDB = true;
}

UTestDatabase& SUnitTestDBBuilder::GetUTestDB()
{
	return m_UTestDBContainer.GetUTestDB();
}

void SUnitTestDBBuilder::ReleaseDB()
{
	DroDatabase();
}

void SUnitTestDBBuilder::DroDatabase()
{
	wstring str = L"USE tempdb; DROP DATABASE " + GetDesc().strDatabaseName + L";";

	m_UTestDBContainer.GetUTestDB().Execute(str.c_str());
	m_bWasCreateDB = false;
}