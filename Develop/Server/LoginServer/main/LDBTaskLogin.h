#pragma once

#include "LDBAsyncTask.h"
#include "MMemPool.h"
#include "LDBTaskDataAccount.h"

struct ACCOUNT_INFO;


class LDBTaskLogin : public LDBAsyncTask, public MMemPool<LDBTaskLogin>
{
public :
	LDBTaskLogin(const MUID& uidReqPlayer);
	~LDBTaskLogin();

	enum
	{
		LOGIN = 0, 
	};


	void					Input(LDBT_ACC_LOGIN& data);


	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();


private :
	class _RESULT
	{
	public :
		_RESULT() : m_nGSN(0), m_bNEW_ACC(true) {}
		int64	m_nGSN;
		wstring m_strPWD;
		bool	m_bNEW_ACC;
	};

	class Completer
	{
	public :
		Completer(LDBT_ACC_LOGIN& data, _RESULT& result);
		void Do();

	private :
		void DebugCheckExistAccount();
		void InitAccountInfo();


	private :
		LDBT_ACC_LOGIN& m_Data;
		_RESULT&		m_Result;
	};


protected :
	LDBT_ACC_LOGIN	m_Data;
	_RESULT			m_Result;
};
