#pragma once 

#include "GTask.h"
#include "GMoveStrategy.h"

// Design Issue: 모든 길찾기 관련(인공지능 관련)코드는 Job으로 이동시킨다.  이 경우에 CombatMove는 간단히 위치만 고정시키는 일을 한다. (이름도 바꾼다)
//					전략적인 판단은 Job(인공지능)쪽이 담당, 직접적인 행위는 Task에서 다룬다.  원칙은 무조건 지키도록 한다.

class GModuleMovable;


class GTask_Move : public GTask, public MTestMemPool<GTask_Move>
{
public:
	DECLARE_TASK_ID(GTASK_MOVE);

	GTask_Move(GEntityNPC* pEntity, vec3 vTargetPos, float fLeastGap, GTaskMoveStrategy nStrategyType=MS_STRAIGHT);
	virtual ~GTask_Move();

	virtual const wchar_t* GetTaskName();

	vec3				GetGoalPos()			{ return m_vGoalPos; }

	// 마지막에 복구될 각도 설정
	void SetFinishDir(vec3 vFinishDir);

private:
	virtual bool OnStart();
	virtual GTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual void OnCancel();
	virtual void OnSuspend();
	virtual void OnResume();
	virtual bool Reset(GTask* pNewTask);

private:
	GModuleMovable* GetMovable() const;

private:
public:
	vec3				m_vGoalPos;
	float				m_fLeastGap;
	MovingWay			m_nWay;
	int					m_nChangeWayCounter;
	GTaskMoveStrategy	m_nStrategyType;
	vec3				m_vFinishDir;	// 마지막에 복구될 각도
};



