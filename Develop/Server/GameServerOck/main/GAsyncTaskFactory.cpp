#include "stdafx.h"
#include "GSystem.h"
#include "GGlobal.h"
#include "GAsyncTaskFactory.h"
#include "GAsyncTask.h"
#include "GAsyncTask_MoveGameServer.h"
#include "GAsyncTask_MoveLoginServer.h"
#include "GAsyncTask_Logout.h"
#include "GASyncTask_LazyLoginMoveGameServer.h"


uint32 GAsyncTaskFactory::GetNowTime()
{
	return gsys.pSystem->GetNowTime();
}

GAsyncTask_MoveGameServer* GAsyncTaskFactory::NewMoveGameServerTask(GAsyncTaskManager* pAsyncTaskMgr)
{
	MUID uidNew = pAsyncTaskMgr->NewUID();
	GAsyncTask_MoveGameServer* pNewTask = new GAsyncTask_MoveGameServer(uidNew, GetNowTime());

	pAsyncTaskMgr->Add(pNewTask);

	return pNewTask;
}

GAsyncTask_MoveLoginServer* GAsyncTaskFactory::NewMoveLoginServerTask(GAsyncTaskManager* pAsyncTaskMgr)
{
	MUID uidNew = pAsyncTaskMgr->NewUID();
	GAsyncTask_MoveLoginServer* pNewTask = new GAsyncTask_MoveLoginServer(uidNew, GetNowTime());

	pAsyncTaskMgr->Add(pNewTask);

	return pNewTask;
}

GAsyncTask_Logout* GAsyncTaskFactory::NewLogoutTask(GAsyncTaskManager* pAsyncTaskMgr, MUID uidPlayer)
{
	MUID uidNew = pAsyncTaskMgr->NewUID();
	GAsyncTask_Logout* pNewTask = new GAsyncTask_Logout(uidNew, GetNowTime(), uidPlayer);	
	
	pAsyncTaskMgr->Add(pNewTask);

	return pNewTask;
}

GASyncTask_LazyLoginMoveGameServer*	GAsyncTaskFactory::NewTask_LazyLoginMoveGameServer(GAsyncTaskManager* pAsyncTaskMgr)
{
	MUID uidNew = pAsyncTaskMgr->NewUID();

	GASyncTask_LazyLoginMoveGameServer* pNewTask = NULL;
	pNewTask = new GASyncTask_LazyLoginMoveGameServer(uidNew, GetNowTime());

	pAsyncTaskMgr->Add(pNewTask);

	return pNewTask;
}