#pragma once

class GAsyncTaskManager;
class GAsyncTask_MoveGameServer;
class GAsyncTask_MoveLoginServer;
class GAsyncTask_Logout;
class GASyncTask_LazyLoginMoveGameServer;

/// AsyncTask Factory
class GAsyncTaskFactory
{
private:
	static uint32 GetNowTime();
public:
	static GAsyncTask_MoveGameServer*			NewMoveGameServerTask(GAsyncTaskManager* pAsyncTaskMgr);
	static GAsyncTask_MoveLoginServer*			NewMoveLoginServerTask(GAsyncTaskManager* pAsyncTaskMgr);
	static GAsyncTask_Logout*					NewLogoutTask(GAsyncTaskManager* pAsyncTaskMgr, MUID uidPlayer);
	static GASyncTask_LazyLoginMoveGameServer*	NewTask_LazyLoginMoveGameServer(GAsyncTaskManager* pAsyncTaskMgr);

};
