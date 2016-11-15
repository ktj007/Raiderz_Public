#pragma once


#include <string>
#include <vector>

using namespace std;

#include "MADBLib.h"

namespace mdb
{
	class MDatabase;
	

	enum MDB_THRTASK_RESULT
	{
		MDBTR_SUCESS = 0	// 작업이 완료되었다면 기본값으로 성공을 리턴한다.(실패를 하더라도 이 값을 넘겨준다.)
							// 이 값이 리턴이 되면 MDBThreadTask객체는 삭제된다.

		, MDBTR_MOVE = 1	// 만약 OnCompleted에서 바로 처리하지 않고 곳에 보관을 하기 위해서 가져가야 한다면
							// 이 값을 리턴한다.
							// 그러면 MDBThreadTask객체를 삭제하지 않고 리스트에서 제거 한다.
							// 객체 관리(삭제)의 모든 책임은 객체를 가져간 곳으로 이동된다.
	};


	typedef vector<wstring> MDB_SQL_VEC;

	class MADB_API MDBThreadTask
	{
	public :
		friend class MAsyncDatabase;
		friend class MDBThread;

		MDBThreadTask(const size_t nReserveSQLSize = 1);
		virtual ~MDBThreadTask();


		const MDB_SQL_VEC&			GetSQLList();
		const string				GetSQL(const size_t nIndex);		
		const wstring				GetSQLW(const size_t nIndex);
		const size_t				GetQuerySize();

		virtual void				OnExecute(MDatabase& rfDB)	= 0;	///< MultiThread에서 호출됨.
																		///< 비동기로 처리할 작업을 하는 곳.

		virtual MDB_THRTASK_RESULT	OnCompleted()				= 0;	///< MAsyncDatabase와 같은 쓰레드에서 호출됨.
																		///< 완료된 작업을 정리하는 곳.


		MDBThreadTask&				PushSQL(const string& strSQL);
		MDBThreadTask&				PushSQLW(const wstring& strSQL);

	
	private :
		MDB_SQL_VEC					m_vecSQL;

		static const wstring		m_strNULLQuery;
	};
}
