#ifndef _GDBTASK_CRAFT_UPDATE_H
#define _GDBTASK_CRAFT_UPDATE_H


#include "GDBTaskCraft.h"
#include "GDBTaskCraftData.h"


class GDBTaskCraftUpdate : public GDBTaskCraft, public MMemPool<GDBTaskCraftUpdate>
{
public :
	GDBTaskCraftUpdate(const MUID& uidPlayer);
	~GDBTaskCraftUpdate();

	enum 
	{
		CRAFT = 0,
	};

	bool Input(GDBT_CRAFT_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB);

	mdb::MDB_THRTASK_RESULT	_OnFailed();
	mdb::MDB_THRTASK_RESULT	_OnCompleted();	

	bool BeginCacheSync(GDBT_CRAFT_DATA& data);


protected :
	class Completer
	{
	public :
		Completer(GDBT_CRAFT_DATA& Data) : m_Data(Data) {}

		void Do();

	private :
		void SetMoney();
		void RemoveMatItem();
		void MergeProductItem();

	private :
		GDBT_CRAFT_DATA& m_Data;
	};


protected :
	GDBT_CRAFT_DATA m_Data;
};


#endif