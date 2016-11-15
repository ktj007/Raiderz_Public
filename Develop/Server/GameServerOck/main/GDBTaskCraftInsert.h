#ifndef _GDBTASK_CRAFT_INSERT_H
#define _GDBTASK_CRAFT_INSERT_H


#include "GDBTaskCraft.h"
#include "GDBTaskCraftData.h"
#include "MMemPool.h"


class GEntityPlayer;


class GDBTaskCraftInsert : public GDBTaskCraft, public MMemPool<GDBTaskCraftInsert>
{
public :
	GDBTaskCraftInsert(const MUID& uidReqPlayer);
	~GDBTaskCraftInsert();

	enum
	{
		CRAFT = 0,
	};

	bool Input(GDBT_CRAFT_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	struct _RESULT
	{
		_RESULT() : nIUID(0) {}

		int64 nIUID;
	};

	class Completer 
	{
	public :
		Completer(GDBT_CRAFT_DATA& Data, _RESULT& Res) : m_Data(Data), m_Result(Res) {}
		~Completer() {}

		void Do();

	private :
		void SetMoney();
		void RemoveMatItem();
		void AddProductItem();		

	private :
		GDBT_CRAFT_DATA& m_Data;
		_RESULT&				m_Result;
	};


protected :
	GDBT_CRAFT_DATA	m_Data;
	_RESULT					m_Result;
};

#endif