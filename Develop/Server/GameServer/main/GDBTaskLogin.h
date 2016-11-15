#ifndef _GDBTASKLOGIN_H
#define _GDBTASKLOGIN_H


#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskDataAccount.h"


struct ACCOUNT_INFO;


class GDBTaskLogin : public GDBAsyncTask, public MMemPool<GDBTaskLogin>
{
public :
	GDBTaskLogin(const MUID& uidReqPlayer);
	~GDBTaskLogin();

	enum
	{
		LOGIN = 0, 
	};


	void					Input(GDBT_ACC_LOGIN& data);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


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
		Completer(GDBT_ACC_LOGIN& data, _RESULT& result);
		void Do();

	private :
		void DebugCheckExistAccount();
		void InitAccountInfo();
		void PostLoginResCommand(const MUID& uidPlayer, const int nRes);


	private :
		GDBT_ACC_LOGIN& m_Data;
		_RESULT&		m_Result;
	};


protected :
	GDBT_ACC_LOGIN	m_Data;
	_RESULT			m_Result;
};


#endif