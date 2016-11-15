#ifndef _GDBTAKS_TALENT_RESET_ALL
#define _GDBTAKS_TALENT_RESET_ALL


#include "GDBAsyncTask.h"
#include "GDBTaskDataTalent.h"
#include "MMemPool.h"


class GDBTaskTalentResetAll : public GDBAsyncTask, public MMemPool<GDBTaskTalentResetAll>
{
public :
	GDBTaskTalentResetAll(const MUID& uidPlayer);
	~GDBTaskTalentResetAll();

	enum
	{
		TALENT_RESET_ALL = 0,
	};

	void					Input(GDBT_TALENT& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	

protected :
	class Completer
	{
	public :
		Completer(GDBT_TALENT& data) : m_Data(data) {}

		void Do();

	private :
		GDBT_TALENT& m_Data;
	};


protected :
	GDBT_TALENT m_Data;
};


#endif