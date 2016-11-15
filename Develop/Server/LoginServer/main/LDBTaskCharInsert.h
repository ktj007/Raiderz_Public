#pragma once


#include "LDBAsyncTask.h"
#include "MMemPool.h"


class LPlayerObject;


class LDBTaskCharInsert : public LDBAsyncTask, public MMemPool<LDBTaskCharInsert>
{
public :
	LDBTaskCharInsert();
	~LDBTaskCharInsert();

	enum
	{
		INSERT_CHAR = 0,
	};


	void					Input(const MUID& uidPlayer, const int64 nAID);

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();

	
private :
	struct _DATA
	{
		MUID					uidPlayer;
		int64					nAID;
		bool					bIsCharNameDuplicated;
	};

	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();

	private :
		bool PreProcessDuplicatedName(LPlayerObject* pPlayerObject);		
		void RefreshCharacter(LPlayerObject* pPlayerObject);		
		void RouteResult(CCommandResultTable nResult);

	private :
		_DATA& m_Data;
	};

	
protected :
	_DATA m_Data;
};
