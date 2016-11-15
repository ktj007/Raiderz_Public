#ifndef _GDBTASK_LEANTALENT_H
#define _GDBTASK_LEANTALENT_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataTalent.h"
#include "CSPalette.h"
#include "MMemPool.h"


#include <vector>
using std::vector;


class GDBTaskTalentLearn : public GDBAsyncTask, public MMemPool<GDBTaskTalentLearn>
{
public :
	typedef vector<pair<PALETTE_NUM, PALETTE_SLOT>> PALETTE_NUM_AND_SLOT_VEC;

	enum
	{
		TALENT_LEAN = 0,
	};

	GDBTaskTalentLearn(const MUID& uidReqPlayer);

	void Input(GDBT_TALENT& data);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

	
private :
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