#include "stdafx.h"
#include "UTestDBRecordSet.h"
#include "UTestDatabaseContainer.h"
#include "UTestDatabase.h"
#include "SDBManager.h"


TDBRecordSet::TDBRecordSet(mdb::MDatabase* pDB) : m_pDB(pDB)
{

}

TDBRecordSet::TDBRecordSet() : m_pDB(UTestDB.GetDB())
{

}

mdb::MDBFieldValueType& TDBRecordSet::Field( const wstring& szName )
{
	return m_RecordSet.FieldW(szName);
}

mdb::MDBFieldValueType& TDBRecordSet::Field( const size_t nIndex )
{
	return m_RecordSet.FieldIndex(nIndex);
}

short TDBRecordSet::GetFieldCount() 
{
	return m_RecordSet.GetFieldCount();
}


bool TDBRecordSet::IsEOF()
{
	return m_RecordSet.IsEOF();
}


void TDBRecordSet::MoveNext()
{
	m_RecordSet.MoveNext();	
}


void TDBRecordSet::Close()
{
	m_RecordSet.Close();
}


uint32 TDBRecordSet::GetFetchedRowCount()
{
	return m_RecordSet.GetFetchedRowCount();
}


uint32 TDBRecordSet::GetFetchedCount()
{
	return m_RecordSet.GetFetchedCount();
}


mdb::MDBRecordSet& TDBRecordSet::GetRecordSet() 
{ 
	return m_RecordSet; 
}