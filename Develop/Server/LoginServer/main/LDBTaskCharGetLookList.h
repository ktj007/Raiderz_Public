#pragma once

#include "LDBAsyncTask.h"
#include "SCommonBodyCharGetLookList.h"
#include "MMemPool.h"


class LDBTaskCharGetLookList : public LDBAsyncTask, public MMemPool<LDBTaskCharGetLookList>
{
public :
	LDBTaskCharGetLookList();
	~LDBTaskCharGetLookList();

	enum
	{
		CHAR_SIMPLE_INFO = 0,
		CHAR_EQUIPMENT_INFO,
	};

	void								Input(const MUID& uidPlayer);

	void								OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT				_OnCompleted();


protected :
	bool								GetSimpleInfo(mdb::MDatabase& rfDB);
	bool								GetEquipInfo(mdb::MDatabase& rfDB);

	
private :
	class Completer
	{
	public :
		Completer(SCommonBodyCharGetLookList& commonBody);
		void Do();

	private :
		SCommonBodyCharGetLookList	m_commonBody;
	};
	

protected :
	SCommonBodyCharGetLookList	m_commonBody;
};
