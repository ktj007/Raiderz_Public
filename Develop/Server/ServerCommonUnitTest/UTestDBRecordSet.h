#pragma once

#include "ServerCommonUnitTestLib.h"
#include "../MAsyncDatabase/MDBRecordSet.h"

class mdb::MDBField;


class SCOMMON_UNITTEST_API TDBRecordSet
{
public:
	friend class UTestDatabase;

	TDBRecordSet(mdb::MDatabase* pDB);
	TDBRecordSet();

	mdb::MDBRecordSet&	GetRecordSet();

	mdb::MDBFieldValueType& Field(const wstring& szName);
	mdb::MDBFieldValueType& Field(const size_t nIndex);

	
	short				GetFieldCount();
	bool				IsEOF();
	void				MoveNext();
	void				Close();
	uint32				GetFetchedRowCount();
	uint32				GetFetchedCount();


private:
	mdb::MDBRecordSet	m_RecordSet;
	mdb::MDatabase*		m_pDB;
	wstring				m_strSQL;
};
