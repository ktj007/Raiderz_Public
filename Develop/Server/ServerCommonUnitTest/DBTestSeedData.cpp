#include "stdafx.h"
#include "DBTestSeedData.h"
#include "UTestDatabase.h"
#include "FileInfo.h"
#include "MLocale.h"

bool DBTestSeedData::Load( const wchar_t* szFileName )
{
	MXml mx;
	if (!mx.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str()))
	{
		return false;
	}

	m_strFileName = szFileName;

	MXmlHandle		docHandle	= mx.DocHandle();
	MXmlElement*	pElement	= docHandle.FirstChild("maiet").FirstChildElement().Element();

	for (; NULL != pElement; pElement=pElement->NextSiblingElement())
	{
		if (!_stricmp("include", pElement->Value()))
		{
			ParseInclude(pElement, mx);
		}		
		else if (!_stricmp("dataset", pElement->Value()))
		{
			ParseDataset(pElement, mx);
		}		
	}


	return true;
}

void DBTestSeedData::ParseInclude( MXmlElement* pElement, MXml& mx )
{
	string strInclFileName;

	if (_Contents(strInclFileName, pElement, &mx))
	{
		char path[_MAX_PATH];

		GetPurePath(path, MLocale::ConvUTF16ToAnsi(m_strFileName.c_str()).c_str());
		
		wstring strFullPath = MLocale::ConvAnsiToUTF16(path);
		strFullPath = strFullPath + MLocale::ConvAnsiToUTF16(strInclFileName.c_str());

		DBTestSeedData* pNewSeedData = new DBTestSeedData(m_pDBMgr);
		if (pNewSeedData->Load(strFullPath.c_str()))
		{
			m_vecRecords.insert(m_vecRecords.end(), pNewSeedData->m_vecRecords.begin(), pNewSeedData->m_vecRecords.end());
		}
		delete pNewSeedData;
	}
}

void DBTestSeedData::ParseDataset( MXmlElement* pElement, MXml& mx )
{
	MXmlElement* pRecordElement = pElement->FirstChildElement();

	for( pRecordElement; pRecordElement != NULL; pRecordElement=pRecordElement->NextSiblingElement() )
	{
		ParseRecord(pRecordElement, mx);
	}

}

void DBTestSeedData::ParseRecord( MXmlElement* pRecordElement, MXml& mx )
{
	wstring strSQL;

	_RecordDef recordDef;

	recordDef.strTableName = MLocale::ConvAnsiToUTF16(pRecordElement->Value());

	MXmlAttribute* pColAttr = pRecordElement->FirstAttribute();
	for( pColAttr; pColAttr != NULL; pColAttr=pColAttr->Next())
	{
		_ColDef col;
		col.strColName = MLocale::ConvAnsiToUTF16(mx.ToAnsi(pColAttr->Name()).c_str());
		col.strValue = MLocale::ConvAnsiToUTF16(mx.ToAnsi(pColAttr->Value()).c_str());

		recordDef.vecColumns.push_back(col);
	}

	m_vecRecords.push_back(recordDef);
}

void DBTestSeedData::Flush()
{
	wstring strFlushSQL;
	for (size_t i = 0; i < m_vecRecords.size(); i++)
	{
		wstring strSQL;

		if (m_vecRecords[i].vecColumns.empty()) return;

		strSQL = L"INSERT INTO " + m_vecRecords[i].strTableName + L"(";

		for (size_t j = 0; j < m_vecRecords[i].vecColumns.size(); j++)
		{
			_ColDef& col = m_vecRecords[i].vecColumns[j];

			strSQL = strSQL + col.strColName;
			if (j != m_vecRecords[i].vecColumns.size() - 1)
			{
				strSQL = strSQL + L",";
			}
		}
		strSQL = strSQL + L") VALUES(";


		for (size_t j = 0; j < m_vecRecords[i].vecColumns.size(); j++)
		{
			_ColDef& col = m_vecRecords[i].vecColumns[j];

			if (!col.strValue.empty() && col.strValue[0] == L'@')
			{
				col.strValue = GetValueFromDB(col.strValue);
			}

			strSQL = strSQL + L"'" + col.strValue + L"'";

			if (j != m_vecRecords[i].vecColumns.size() - 1)
			{
				strSQL = strSQL + L",";
			}
		}

		strSQL = strSQL + L");";

		strFlushSQL.append(strSQL);
	}

	m_pDBMgr->Execute(strFlushSQL.c_str());
}

wstring DBTestSeedData::GetValueFromDB( wstring& sql )
{
	wstring strValue;
	sql.erase(0,1);

	TDBRecordSet recordSet;
	m_pDBMgr->Execute(recordSet, sql.c_str());

	if (recordSet.GetFieldCount() <= 0) return strValue;

	for(; !recordSet.IsEOF(); recordSet.MoveNext())
	{
		strValue = recordSet.Field(0).AsWString();
		break;
	}

	return strValue;
}
