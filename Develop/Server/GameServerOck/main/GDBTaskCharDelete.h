#ifndef _GDBTASK_DELETECHARACTER_H
#define _GDBTASK_DELETECHARACTER_H


#include "GDBAsyncTask.h"


class SAccountCharList;
enum CCommandResultTable;


class GDBTaskCharDelete : public GDBAsyncTask, public MTestMemPool<GDBTaskCharDelete>
{
public :
	GDBTaskCharDelete(const MUID& uidReqPlayer);
	~GDBTaskCharDelete();


	enum
	{
		CHAR_DELETE = 0,
	};

	enum RETURN : int
	{
		FAIL_GUILDMASTER = -1,
		SUCCESS = 0,		
	};


	void Input(const int nIndex);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	

private :
	struct _DATA
	{
		MUID	uidPlayer;
		int		nIndex;
		int		nReturn;
	};

	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();
		
	private :
		void RouteResult(CCommandResultTable nCRT, int nIndex);

	private :
		_DATA& m_Data;		
	};


protected :
	_DATA	m_Data;
};

#endif