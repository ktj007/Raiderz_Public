#ifndef _GDBTASK_ITEM_DEC_STACK_AMT_H
#define _GDBTASK_ITEM_DEC_STACK_AMT_H



#include "GDBTaskItemStackAmt.h"
#include "GDBTaskItemData.h"
#include "MMemPool.h"


class GDBTaskItemDecStackAmt : public GDBTaskItemStackAmt, public MMemPool<GDBTaskItemDecStackAmt>
{
public :
	GDBTaskItemDecStackAmt(const MUID& uidPlayer);
	~GDBTaskItemDecStackAmt() {}	

	enum
	{
		ITEM_DEC_STACK_AMT = 0,
	};


	bool Input(GDBT_ITEM_DEC_STACK_AMT_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	GDBT_ITEM_DEC_STACK_AMT_DATA m_Data;
};


#endif