#pragma once

#include "LDBAsyncTask.h"
#include "PmDef.h"
#include "MMemPool.h"


struct ACCOUNT_INFO;


class LDBTaskPmangLogin : public LDBAsyncTask, public MMemPool<LDBTaskPmangLogin>
{
public :
	LDBTaskPmangLogin(const MUID& uidReqPlayer);
	~LDBTaskPmangLogin();

	enum
	{
		LOGIN = 0, 
	};


	void					Input(const wstring& strUserID, const PmUserData& pmangUserData);

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();


private :
	struct _DATA
	{
		MUID			uidPlayer;
		wstring			strUserID;
		PmUserData		pmangUserData;

		int64			nDBAID;
		wstring			strDBPassword;		
		bool			bNewAcc;
	};

	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();

	private :
		void DebugCheckExistAccount();
		void AddNewPlayer();


	private :
		_DATA m_Data;
	};


protected :
	_DATA m_Data;
};
