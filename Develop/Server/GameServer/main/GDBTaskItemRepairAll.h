#ifndef _GDBTASK_ITEM_REPAIR_ALL_H
#define _GDBTASK_ITEM_REPAIR_ALL_H


#include "GDBTaskNPCShop.h"
#include "MMemPool.h"


class GDBTaskItemRepairAll : public GDBAsyncTask, public MMemPool<GDBTaskItemRepairAll>
{
public :
	GDBTaskItemRepairAll(const MUID& uidPlayer);
	~GDBTaskItemRepairAll();

	enum
	{
		ITEM_REPAIR_ALL = 0,
	};

	bool Input(GDBT_REPAIR_ALL_ITEM& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	virtual void			_OnPreCompleted()	override;
	bool					BeginCharCacheSync(const MUID& uidPlayer);
	bool					BeginItemsCacheSync(GDBT_REPAIR_ALL_ITEM& data);


protected :
	struct _DATA
	{
		MUID			uidPlayer;
		int				nMoney;
		vector<int64>	vecIUID;
	};

	class Completer
	{
	public :
		Completer(GDBT_REPAIR_ALL_ITEM& Data) : m_Data(Data) {}

		void Do();

	private :
		void SetMoney(GEntityPlayer* pPlayer);
		void SetItemDura(GEntityPlayer* pPlayer);
		void Route(GEntityPlayer* pPlayer);

	private :
		GDBT_REPAIR_ALL_ITEM& m_Data;
	};


protected :
	GSyncingCacheList		m_SyncingList;
	GDBT_REPAIR_ALL_ITEM	m_Data;
};


#endif