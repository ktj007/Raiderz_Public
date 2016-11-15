#ifndef _GTASK_COMBAT_MOVE_H
#define _GTASK_COMBAT_MOVE_H


#include "GTask.h"


class GPathFinder;

// #Design Issue: 모든 길찾기 관련(인공지능 관련)코드는 Job으로 이동시킨다.  이 경우에 CombatMove는 간단히 위치만 고정시키는 일을 한다. (이름도 바꾼다)
//					전략적인 판단은 Job(인공지능)쪽이 담당, 직접적인 행위는 Task에서 다룬다.  원칙은 무조건 지키도록 한다.

/// 점진적 이동 
class GTask_CombatMove : public GTask
{
private:
	float					m_fRotateSpeed;			///< 회전 속도
	vec3					m_vTargetPos;
	vec3					m_vAdvancePos;			///< 점진적으로 이동되는 목표 좌표
	int						m_nOldPathCounter;
	bool					m_bOldMoveStopByContext;

private:
	bool		_FindPath();
	void		_MoveStop();
	void		_SyncPos();
	GTaskResult _AdvancedAlongPath(float fDelta, bool& boutMoveStopByContext, bool& boutNewPathLine);
	void		_RouteStatus(bool bMoveStopByContext, bool bNewPathLine);
	void		_RouteDebugInfo();
protected:
	virtual bool OnStart();
	virtual GTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual bool OnCancel();
public:
	DECLARE_TASK_ID(GTASK_COMBAT_MOVE);

	GTask_CombatMove(GEntityNPC* pEntity, vec3 vTargetPos);
	virtual ~GTask_CombatMove();

	virtual const char* GetTaskName() {return "CombatMove";}

	void Reset(vec3 vTarPos);
	vec3 GetTargetPos()			{ return m_vTargetPos; }
};







#endif