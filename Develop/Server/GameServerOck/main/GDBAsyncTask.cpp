#include "stdafx.h"
#include "GDBAsyncTask.h"
#include "GDBTaskGlobal.h"
#include "MDatabaseDefine.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GSystem.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "MDatabaseInfo.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GConfig.h"
#include "GConst.h"


GDBTaskExecCounter GDBAsyncTask::m_Conter;

GDBAsyncTask::GDBAsyncTask(const MUID& uidReqPlayer, const SDBT_DBTYPE DBType, const SDBTASK_ID TaskID, const size_t nReserveSQLSize) 
: SDBAsyncTask(DBType, TaskID, nReserveSQLSize), m_uidReqPlayer(uidReqPlayer), m_dwPostTm(0), m_bSync(true)
{	
}


GDBAsyncTask::~GDBAsyncTask()
{
}

const MUID& GDBAsyncTask::GetReqPlayer()
{
	return m_uidReqPlayer;
}

void GDBAsyncTask::IncreaseRefCount(const MUID& uidReqPlayer)
{
	if (0 == uidReqPlayer)
		return;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidReqPlayer);
	if (NULL == pPlayer)
		return;

	if (!pPlayer->IsType(ETID_PLAYER))
		return;

	pPlayer->GetDBAsyncMonitor().IncreaseRefCount();
}

void GDBAsyncTask::DecreaseRefCount(const MUID& uidReqPlayer)
{
	if (0 == uidReqPlayer)
		return;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidReqPlayer);
	if (NULL == pPlayer)
		return;

	if (!pPlayer->IsType(ETID_PLAYER))
		return;

	pPlayer->GetDBAsyncMonitor().DecreaseRefCount();
}

DWORD GDBAsyncTask::GetExecuteElapsedTime()
{
	return gsys.pSystem->GetNowTime() - m_dwPostTm;
}

void GDBAsyncTask::SetPostTm( const DWORD dwTime )
{
	m_dwPostTm = dwTime;
}

void GDBAsyncTask::_OnPreCompleted()
{
	DecreaseRefCount(m_uidReqPlayer);

	AddTaskCount();	
}

void GDBAsyncTask::OnPrePost()
{
	IncreaseRefCount(GetReqPlayer());
	SetPostTm(gsys.pSystem->GetNowTime());
}

bool GDBAsyncTask::OnPost()
{
	return false;
}

void GDBAsyncTask::OnFailPost()
{
	DecreaseRefCount(GetReqPlayer());
}

DWORD GDBAsyncTask::GetWorkTm( const DWORD dwNowTm )
{
	return dwNowTm - m_dwPostTm;
}

GDBTaskSortedList GDBAsyncTask::GetExecCountDecrSortedList()
{
	return m_Conter.GetDecrSortedList();
}

mdb::MDB_THRTASK_RESULT GDBAsyncTask::OnCompleted()
{
	_OnPreCompleted();

	mdb::MDB_THRTASK_RESULT Res = mdb::MDBTR_SUCESS;

	if (!CheckResult())
		Res = _OnFailed();
	else
		Res = _OnCompleted();

	_OnEndCompleted();

	return Res;
}

void GDBAsyncTask::_OnEndCompleted()
{

}

bool GDBAsyncTask::IsSync()
{
	return m_bSync;
}

void GDBAsyncTask::SetSync( const bool bSync )
{
	m_bSync = bSync;
}

void GDBAsyncTask::AddTaskCount()
{
	if (GConfig::m_bDBTaskCount)
		m_Conter.AddCount(GetID(), GetWorkTm(gsys.pSystem->GetNowTime()));
}
