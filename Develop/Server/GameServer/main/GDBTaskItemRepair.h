#ifndef _GDBTASK_ITEM_REAIR_H
#define _GDBTASK_ITEM_REAIR_H


#include "GDBAsyncTask.h"
#include "GSyncingCacheList.h"
#include "GDBTaskNPCShopData.h"
#include "MMemPool.h"


class GDBTaskItemRepair : public GDBAsyncTask, public MMemPool<GDBTaskItemRepair>
{
public :
	GDBTaskItemRepair(const MUID& uidPlayer);
	~GDBTaskItemRepair();

	enum 
	{
		ITEM_REPAIR = 0,
	};

	bool Input(GDBT_REPAIR_ITEM& data);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	virtual void			_OnPreCompleted()	override;
	bool					BeginCharCacheSync(const MUID& uidPlayer);
	bool					BeginItemCacheSync(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);


protected :
	class Completer
	{
	public :
		Completer(GDBT_REPAIR_ITEM& Data) : m_Data(Data) {}

		void Do();

	private :
		void SetMoney(GEntityPlayer* pPlayer);
		void SetItemDura(GEntityPlayer* pPlayer);
		void Route(GEntityPlayer* pPlayer);

	private :
		GDBT_REPAIR_ITEM& m_Data;
	};

protected :
	GSyncingCacheList	m_SyncingList;
	GDBT_REPAIR_ITEM	m_Data;
};



#endif