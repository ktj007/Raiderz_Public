#ifndef _GDBTASK_H
#define _GDBTASK_H

#include "SDBTask.h"
#include "SDBTaskID.h"
#include "GDBTaskExecCounter.h"
#include "MMemPool.h"


#include <vector>


namespace mdb
{
	class MDatabase;
}


class GDBAsyncTask : public SDBAsyncTask
{
public :
	GDBAsyncTask(const MUID& uidReqPlayer, const SDBT_DBTYPE DBType, const SDBTASK_ID TaskID, const size_t nReserveSQLSize = 1);
	virtual ~GDBAsyncTask();

	const MUID&						GetReqPlayer();
	DWORD							GetExecuteElapsedTime();
	DWORD							GetWorkTm(const DWORD dwNowTm);
	void							SetPostTm(const DWORD dwTime);

	mdb::MDB_THRTASK_RESULT			OnCompleted()					override;

	static GDBTaskSortedList		GetExecCountDecrSortedList();

	bool							IsSync();
	void							SetSync(const bool bSync);	

public :
	virtual void					OnPrePost();
	virtual bool					OnPost();
	virtual void					OnFailPost();


protected :
	virtual void					IncreaseRefCount(const MUID& uidReqPlayer);
	virtual void					DecreaseRefCount(const MUID& uidReqPlayer);
	virtual void					AddTaskCount();


protected :
	// OnCompleted함수에서 호출되는 함수들.
	virtual void					_OnPreCompleted();								// 결과 검사를 하기전에 실행 되는 부분. 결과와 상관없이 항상 실행된다.
	virtual mdb::MDB_THRTASK_RESULT	_OnCompleted()	{ return mdb::MDBTR_SUCESS; }	// 성공 처리는 이걸 재정의 해서 처리.
	virtual mdb::MDB_THRTASK_RESULT	_OnFailed()		{ return mdb::MDBTR_SUCESS; }	// 실패 처리는 이걸 상속 받아서 처리.
	virtual void					_OnEndCompleted();								// 마지막에 무조건 실행되어야 할 경우 재정의 한다.
	

protected :
	MUID							m_uidReqPlayer;
	DWORD							m_dwPostTm;
	bool							m_bSync;
	
	static GDBTaskExecCounter		m_Conter;
};



#endif