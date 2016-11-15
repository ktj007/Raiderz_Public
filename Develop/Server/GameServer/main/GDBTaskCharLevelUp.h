#ifndef _GDBTASK_CHAR_LEVEL_UP_H
#define _GDBTASK_CHAR_LEVEL_UP_H


#include "GDBAsyncTask.h"
#include "GDBTaskCharData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GEntityPlayer;


class GDBTaskCharLevelUp : public GDBAsyncTask, public MMemPool<GDBTaskCharLevelUp>
{
public :
	GDBTaskCharLevelUp(const MUID& uidReqPlayer);

	enum
	{
		LEVEL_UP = 0,
	};

	bool Input(GDBT_CHAR_LEVEL_UP_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	virtual void			_OnPreCompleted() override;

protected :
	bool SyncCharacter();

protected :
	class Completer
	{
	public :
		Completer(GDBT_CHAR_LEVEL_UP_DATA& data) : m_Data(data) {}
		void Do();

	protected :
		GDBT_CHAR_LEVEL_UP_DATA& m_Data;
	};

protected :
	GDBT_CHAR_LEVEL_UP_DATA m_Data;
	GSyncingCacheList		m_SyncingList;
};


#endif