#ifndef _GDBTASK_ITEM_INSERT_H
#define _GDBTASK_ITEM_INSERT_H


#include "GDBAsyncTask.h"
#include "GDBTaskItemData.h"
#include "MMemPool.h"


class GDBTaskItemInsert : public GDBAsyncTask, public MMemPool<GDBTaskItemInsert>
{
public :
	GDBTaskItemInsert(const MUID& uidReqPlayer);
	~GDBTaskItemInsert();

	enum
	{
		ITEM_INSERT = 0
	};

	void Input(GDBT_ITEM_DATA& data);
		
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

	
protected :
	struct _RESULT
	{
		_RESULT()
		:m_nIUID(0){}

		IUID m_nIUID;
	};

	class Completer
	{
	public :
		Completer(GDBT_ITEM_DATA& data, _RESULT& result) : m_Data(data), m_Result(result) {}
		~Completer() {}

		void Do();

	private :
		GDBT_ITEM_DATA&	m_Data;
		_RESULT&				m_Result;
	};


protected :
	GDBT_ITEM_DATA	m_Data;
	_RESULT					m_Result;
};


#endif