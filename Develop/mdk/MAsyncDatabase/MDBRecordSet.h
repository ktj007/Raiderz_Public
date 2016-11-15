#pragma once

#include <unordered_map>
#include <string>
#include <utility>


using namespace std;


#include "MDatabaseQuery.h"
#include "MDatabaseInfo.h"
#include "MDBFieldValueTypeHeaderList.h"


namespace mdb
{
	static const int MDB_MAX_FIELD_NAME_LEN = 64;
	static const int MDB_DEFAULT_STRING_LEN = 1024;


	class MDatabase;
	

	class MADB_API MDBRecordSet : public MDatabaseQuery
	{
	public :
		friend class MDBField;
		friend class MDBRecordSetConfigure;


		typedef std::tr1::unordered_map<size_t, MDBFieldValueType*>		MDB_FIELD_HMAP;
		typedef std::pair<MDB_FIELD_HMAP::iterator, bool>				MDB_FIELD_HMAP_RETURN;
		typedef std::vector<MDBFieldValueType*>							MDB_FIELD_VEC;


	public :
		MDBRecordSet();
		MDBRecordSet(MDatabase* pDB);		
		virtual ~MDBRecordSet();


		virtual bool			Execute(const string& strSQL) override;
		virtual bool			ExecuteW(const wstring& strSQL) override;

		virtual bool			Execute(const string& strSQL, int& nRefReturn) override;
		virtual bool			ExecuteW(const wstring& strSQL, int& nRefReturn) override;

		virtual bool			Open(const string& strSQL);
		virtual bool			OpenW(const wstring& strSQL);

		virtual bool			Open(const string& strSQL, int& nRefReturn);
		virtual bool			OpenW(const wstring& strSQL, int& nRefReturn);

		bool					InitOpen(MDatabase* pDB, size_t nSqlLen);
		bool					Close();

		MDBFieldValueType&		Field(const string& strName);
		MDBFieldValueType&		FieldW(const wstring& strName);
		MDBFieldValueType&		FieldHash(const size_t nHash);
		MDBFieldValueType&		FieldIndex(const size_t nIndex);

		bool					GetData(const short nIndex
									, SQLPOINTER pTargetValue
									, const SQLSMALLINT nValueType 
									, const SQLSMALLINT nValueLength
									, long& nReadLength);

		long					GetFetchedRowCount();
		int						GetFetchedCount();
		short					GetFieldCount();


		void					StartMovingRecordSet();
		bool					MoveNext();
		bool					IsOpen();
		bool					IsEOF();

		size_t					MakeHashValue(const string& str);
		size_t					MakeHashValueW(const wstring& str);

	public :
		virtual bool			Open(MDatabase* pDB, const string& strSQL, int& nRefReturn, bool bHaveReturn);
		virtual bool			OpenW(MDatabase* pDB, const wstring& strSQL, int& nRefReturn, bool bHaveReturn);

	private :
		bool					InitEnv();
		void					InitData();
		bool					BuildField();
		bool					BuildFieldValue();
		bool					InsertField(const wstring& strFieldName, const SQLSMALLINT SqlType, const SQLSMALLINT nIndex);		
		void					SetEOF(const bool nState);
		int						GetFieldCountFromODBC();

		MDBFieldValueType&		GetNullField();					
		void					ReleaseField();


	protected :
		MDB_FIELD_HMAP			m_FieldMap;
		MDB_FIELD_VEC			m_FieldVec;	// 컬럼 순서대로 일기 위해서 필요. 
											// 순서대로 읽지 않으면 실패함...
		
		bool					m_bIsEOF;
		SQLINTEGER				m_nFetchedCount;
		int						m_nFetchedRowCount;

		bool					m_bIsOpened;
		bool					m_bIsClosed;

		MDBFieldValueType*		m_pNullField;		
	};
}