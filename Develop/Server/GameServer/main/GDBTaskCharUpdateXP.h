#ifndef _GDBTASK_CHAR_SET_XP_H
#define _GDBTASK_CHAR_SET_XP_H


#include "GDBAsyncTask.h"
#include "GDBTaskCharData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GDBTaskCharUpdateXP : public GDBAsyncTask, public MMemPool<GDBTaskCharUpdateXP>
{
public :
	GDBTaskCharUpdateXP(const MUID& uidReqPlayer);

	enum
	{
		UPDATE_XP = 0,
	};

	bool Input(GDBT_CHAR_XP_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	virtual void			_OnPreCompleted() override;


protected :
	bool UpdateCache(const GDBT_CHAR_XP_DATA& data);


protected :
	class Completer
	{
	public :
		Completer(GDBT_CHAR_XP_DATA& data) : m_Data(data) {}

		void Do();

	private :
		GDBT_CHAR_XP_DATA& m_Data;
	};


protected :
	GDBT_CHAR_XP_DATA m_Data;
	GSyncingCacheList m_SyncingList;
};


#endif