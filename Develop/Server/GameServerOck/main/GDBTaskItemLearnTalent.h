#ifndef _GDBTASK_ITEM_LEARN_TALENT_H
#define _GDBTASK_ITEM_LEARN_TALENT_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataTalent.h"
#include "MMemPool.h"


class GDBTaskItemLearnTalent : public GDBAsyncTask, public MMemPool<GDBTaskItemLearnTalent>
{
public :
	GDBTaskItemLearnTalent(const MUID& uidReqPlayer);
	~GDBTaskItemLearnTalent();

	enum 
	{
		ITEM_LEARN_TALENT = 0,
	};

	bool Input(GDBT_ITEM_LEARN_TALENT& data);

	bool					BeginItemCacheSync( const uint8 nSlotType, const int16 nSlotID, const IUID nIUID );
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	virtual void			_OnPreCompleted() override;


protected :
	class Completer
	{
	public :
		Completer(GDBT_ITEM_LEARN_TALENT& data) : m_Data(data) {}

		void Do();

	private :
		GDBT_ITEM_LEARN_TALENT& m_Data;
	};

protected :
	GDBT_ITEM_LEARN_TALENT m_Data;
};


#endif