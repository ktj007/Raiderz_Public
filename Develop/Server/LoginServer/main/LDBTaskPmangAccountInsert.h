#pragma once

#include "LDBAsyncTask.h"
#include "MMemPool.h"
#include "PmDef.h"


class LDBTaskPmangAccountInsert : public LDBAsyncTask, public MMemPool<LDBTaskPmangAccountInsert>
{
public :
	LDBTaskPmangAccountInsert(const MUID& uidPlayer);
	~LDBTaskPmangAccountInsert();

	enum
	{
		ACCOUNT_INSERT = 0
	};

	void Input(const std::wstring strUserID, const std::wstring strPassword, const PmUserData& pmangUserData);

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();


protected :
	struct _DATA
	{
		MUID		uidPlayer;
		std::wstring strUserID;
		std::wstring strPassword;
		PmUserData	 pmangUserData;

		AID		m_nGSN;
		int64	m_nCONN_SN;
	};

	class Completer
	{
	public :
		Completer(_DATA& Data) : m_Data(Data) {}

		void Do();
		void AddNewPlayer();

	protected :
		_DATA& m_Data;
	};


protected :
	_DATA m_Data;
};
