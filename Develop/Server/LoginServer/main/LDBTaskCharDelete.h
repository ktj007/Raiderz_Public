#ifndef _LDBTASK_DELETECHARACTER_H
#define _LDBTASK_DELETECHARACTER_H


#include "LDBAsyncTask.h"
#include "MMemPool.h"


class SAccountCharList;


class LDBTaskCharDelete : public LDBAsyncTask, public MMemPool<LDBTaskCharDelete>
{
public :
	LDBTaskCharDelete();
	~LDBTaskCharDelete();


	enum
	{
		CHAR_DELETE = 0,
	};

	enum RETURN
	{
		FAIL_GUILDMASTER = -1,
		SUCCESS = 0,		
	};


	void Input(const MUID& uidPlayer, const int nIndex);


	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();

	
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
		Completer(_DATA& Data, int& nReturn);
		void Do();

	private :
		void RebuildCharList(const int nIndex, SAccountCharList& CharList);
		void RouteResult(CCommandResultTable nCRT, int nIndex);

	private :
		_DATA&	m_Data;
	};


protected :
	_DATA m_Data;
};

#endif