#pragma once

#include "ServerCommonUnitTestLib.h"
#include "TestDBTableStatus.h"

class UTestDatabase;

class SCOMMON_UNITTEST_API DBTestSeedData
{
private:
	UTestDatabase*			m_pDBMgr;
	wstring				m_strFileName;

	struct _ColDef
	{
		wstring		strColName;
		wstring		strValue;
	};
	struct _RecordDef
	{
		wstring			strTableName;
		vector<_ColDef>	vecColumns;
	};

	vector<_RecordDef>	m_vecRecords;

	void ParseInclude(MXmlElement* pElement, MXml& mx);
	void ParseDataset(MXmlElement* pElement, MXml& mx);
	void ParseRecord(MXmlElement* pRecordElement, MXml& mx);
	wstring GetValueFromDB(wstring& sql);
public:
	DBTestSeedData(UTestDatabase* pDBMgr) : m_pDBMgr(pDBMgr) {}
	~DBTestSeedData() {}
	bool Load(const wchar_t* szFileName);
	void Flush();
};