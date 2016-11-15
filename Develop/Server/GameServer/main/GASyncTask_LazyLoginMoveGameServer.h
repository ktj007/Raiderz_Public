#pragma once

#include "GAsyncTask.h"

class GASyncTask_LazyLoginMoveGameServer : public GAsyncTask, public MTestMemPool<GASyncTask_LazyLoginMoveGameServer>
{
public:
	static const float TIMEOUT_SEC;
public:
	GASyncTask_LazyLoginMoveGameServer(MUID uid, uint32 nNowTime);
	virtual ~GASyncTask_LazyLoginMoveGameServer() {}

	GAsyncTaskID	GetID() override { return ASYNC_TASK_LAZY_LOGIN_MOVE_GAMESERVER; }

	void			Init(MUID uidPlayerSession, MUID uidConnectionKey, MUID uidRealPlayerUID, bool bOnPmang=false, wstring strPmangStatIndex=L"");

protected:
	void			OnUpdate() override;

private:
	MUID			m_uidPlayerSession;
	MUID			m_uidConnectionKey;
	MUID			m_uidRealPlayerUID;
	bool			m_bOnPmang;
	wstring			m_strPmangStatIndex;

	MRegulator		m_rgltUpdate;
	MRegulator		m_rgltTimeout;
	uint32			m_nLastTime;

	void			OnTimeout();

};
