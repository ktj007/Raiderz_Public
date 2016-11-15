#pragma once

#include "GFrameworkLib.h"
#include "GAsyncTaskID.h"
#include "MUID.h"
#include "MReferenceCounted.h"
#include "CSHelper.h"
#include "MCommand.h"
#include <map>
using namespace std;

class GFRAMEWORK_API GAsyncTask : public MReferenceCounted
{
protected:
	MUID			m_UID;
	uint32			m_nStartTime;
	bool			m_bDone;

	virtual void OnUpdate() {}

public:
	GAsyncTask(MUID uid, uint32 nNowTime);
	virtual ~GAsyncTask() {}
	void Update();

	void Done()
	{
		m_bDone = true;
	}

	MUID GetUID() { return m_UID; }
	bool CheckTimeOut(uint32 nNowTime);	// true로 반환하면 TimeOut되었다는 것을 뜻함
	bool IsDone() { return m_bDone; }
	virtual void OnCommand(minet::MCommand* pCommand) {}
	virtual GAsyncTaskID GetID() = 0;
	virtual uint32 GetTimeOut() { return 0; }	// 0이면 무제한
};

/// AsyncTask 매니저
class GFRAMEWORK_API GAsyncTaskManager : public MTestMemPool<GAsyncTaskManager>
{
protected:
	typedef map<MUID, GAsyncTask*>		GAsyncTaskMap;
	GAsyncTaskMap						m_TaskMap;
	MUIDGenerator						m_UIDGenerator;
public:
	GAsyncTaskManager();
	virtual ~GAsyncTaskManager();
	MUID NewUID();
	void Add(GAsyncTask* pAsyncTask);
	void Delete(MUID uidTask);
	void Update(uint32 nNowTime);
	GAsyncTask* FindTask(MUID uidTask);
	void Clear();
	size_t GetTaskCount() { return m_TaskMap.size(); }

	void OnCommand(minet::MCommand* pCommand);
};

