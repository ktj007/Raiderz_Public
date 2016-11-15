#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GSyncingCacheList.h"
#include "GDBTaskData.h"

class GEntityPlayer;

class GDBTaskMailGetItem : public GDBAsyncTask, public MMemPool<GDBTaskMailGetItem>
{
public:
	GDBTaskMailGetItem(const MUID& uidReqPlayer);
	~GDBTaskMailGetItem(void);

	enum
	{
		SQL_MAIL_GET_ITEM = 0
	};

	bool					Input(const GDBT_MAIL_GET_ITEM& data);

	virtual void			OnExecute(mdb::MDatabase& rfDB) override;

protected:
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

	bool	BeginCacheSync();
	bool	CacheSyncCharacter();
	void	EndCacheSync(bool bSuccess);

protected :
	struct _DATA
	{
		GDBT_MAIL_GET_ITEM m_MailItem;		
	};

	_DATA				m_Data;
	GSyncingCacheList	m_SyncingList;

private:
	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();
		void AddItemToPlayer(GEntityPlayer* pPlayer);
		void DelItemFromMail(GEntityPlayer* pPlayer);

	private :
		_DATA& m_Data;
	};

};