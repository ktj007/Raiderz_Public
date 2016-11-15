#ifndef _GDBTASK_ITEM_INC_STACK_AMT_H
#define _GDBTASK_ITEM_INC_STACK_AMT_H



#include "GDBTaskItemStackAmt.h"
#include "GDBTaskItemData.h"
#include "MMemPool.h"


class GDBTaskItemIncStackAmt : public GDBTaskItemStackAmt, public MMemPool<GDBTaskItemIncStackAmt>
{
public :
	GDBTaskItemIncStackAmt(const MUID& uidPlayer);
	~GDBTaskItemIncStackAmt() {}	

	enum
	{
		ITEM_INC_STACK_AMT = 0,
	};


	bool Input(GDBT_ITEM_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	GDBT_ITEM_DATA m_Data;
};


#endif