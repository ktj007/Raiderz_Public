#ifndef _GTASK_H
#define _GTASK_H

#include <queue>
#include <list>
using namespace std;

#include "GTaskID.h"
#include "GMessage.h"

class GEntityNPC;
class GJob;

/// GJob
/// - 역활: 세부 Task들의 상위 클래스
/// - 협력: GEntitNPC, GNPCInfo
class GTask
{
	friend GJob;
private:
	GJob*				m_pParent;
protected:
	GEntityNPC*			m_pOwner;
		
	virtual bool OnStart() = 0;							///< 태스크 시작시
	virtual GTaskResult OnRun(float fDelta) = 0;		///< 태스크 실행
	virtual void OnComplete() = 0;						///< 테스크 완료시
	virtual void OnCancel() = 0;						///< 태스크 취소시
	virtual void OnSuspend()	{}						///< 태스크 중단시
	virtual void OnResume()		{}						///< 중단된 테스크 재개시
public:
	GTask(GEntityNPC* pOwner);
	virtual ~GTask() {}
	virtual GMessageResult	OnMessage(GMessage& msg)	{ return MR_FALSE; }

	bool Start();
	GTaskResult Run(float fDelta);
	void Complete();
	void Cancel();
	GEntityNPC* GetOwner() { return m_pOwner; }
	virtual GTaskID GetID() = 0;
	virtual const wchar_t* GetTaskName() = 0;

	virtual bool Reset(GTask* pNewTask);				///< ret: 리셋됐는지 여부
};


#define DECLARE_TASK_ID(_ID)	GTaskID GetID() { return _ID; }

#endif