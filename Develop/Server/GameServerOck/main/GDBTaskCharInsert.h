#ifndef _GDBTASK_INSERTCHARACTER_H
#define _GDBTASK_INSERTCHARACTER_H


#include "GDBAsyncTask.h"


class GPlayerObject;
enum CCommandResultTable;

class GDBTaskCharInsert : public GDBAsyncTask, public MTestMemPool<GDBTaskCharInsert>
{
public :
	GDBTaskCharInsert(const MUID& uidReqPlayer);
	~GDBTaskCharInsert();

	enum
	{
		INSERT_CHAR = 0,
	};


	void					Input(const int64 nAID);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

	
private :

private :
	struct _DATA
	{
		MUID	uidPlayer;
		int64	nAID;
		bool	bIsCharNameDuplicated;
	};

	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();

	private :
		bool PreProcessDuplicatedName(GPlayerObject* pPlayerObject);
		void RefreshCharacter(GPlayerObject* pPlayerObject);
		void RouteResult(CCommandResultTable nResult);
	private :
		_DATA& m_Data;
	};


protected :
	_DATA m_Data;
};


#endif