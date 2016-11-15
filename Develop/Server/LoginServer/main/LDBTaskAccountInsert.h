#ifndef _LDBTASK_ACCOUNT_INSERT_H
#define _LDBTASK_ACCOUNT_INSERT_H



#include "LDBAsyncTask.h"
#include "MMemPool.h"
#include "LDBTaskDataAccount.h"

class LDBTaskAccountInsert : public LDBAsyncTask, public MMemPool<LDBTaskAccountInsert>
{
public :
	LDBTaskAccountInsert(const MUID& uidPlayer);
	~LDBTaskAccountInsert();

	enum
	{
		ACCOUNT_INSERT = 0
	};

	void Input(LDBT_ACC_INSERT& data);

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();


protected :
	class _RESULT
	{
	public :
		int64 m_GSN;
	};

	class Completer
	{
	public :
		Completer(LDBT_ACC_INSERT& data, _RESULT& result) : m_Data(data), m_Result(result) {}

		void Do();
		void InitAccountInfo();		

	protected :
		LDBT_ACC_INSERT&	m_Data;
		_RESULT&			m_Result;
	};


protected :
	LDBT_ACC_INSERT m_Data;
	_RESULT			m_Result;	
};



#endif