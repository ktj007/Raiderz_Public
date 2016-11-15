#pragma once

#include "MMemPool.h"
#include "GModule.h"
#include "GMoveStrategy.h"

#define SERVER_MOVABLE	L"MOVABLE"

TEST_FORWARD_FT(Rotate, FRotate, fnCalcDir);

class GTask;

class GModuleMovable : public GModule, public MMemPool<GModuleMovable>
{
	TEST_FRIEND_FT(Rotate, FRotate, fnCalcDir);
	DECLARE_ID(GMID_MOVABLE);
public:
	GModuleMovable(GEntity* pOwner);
	virtual ~GModuleMovable(void);

	virtual const wchar_t*	GetClassName(void)			{ return SERVER_MOVABLE;			}	/// 클래스 이름 얻기

	vec3				GetGoalPos()			{ return m_vGoalPos; }
	GTaskMoveStrategy	GetMoveStrategy()		{ return m_nStrategyType; }

	void Move( vec3 vTargetPos, float fLeastGap, GTaskMoveStrategy nStrategyType );

	// 마지막에 복구될 각도 설정
	void SetFinishDir(vec3 vFinishDir);

	const wchar_t* GetTaskName();

	bool IsMoveFinish();

public: // Task에 의해 조정되는 함수들
	bool Start();
	void Complete();
	void Cancel();
	void Suspend();
	void Resume();
	void Reset( GTask* pNewTask );

private:
	virtual void OnUpdate(float fDelta) override;

	GMoveStrategy*	NewStrategy(GTaskMoveStrategy nStrategy, float fLeastGap);
//	vec3 CalcFlexibleDestPos( GEntityNPC* pAgentNPC, vec3& vStartPos, vec3& vDestPos, float fLeastGap );
	bool IsNeedRotation();
	bool IsFinishRotate(float fTolerAngle);
//	void ChangeWay(int nAgentNum, float fDistanceFromEnemy);
	void CalcGoalDir();
	void CalcRotateGoalDir();
	bool CheckValidation(vec3 vTestGoalPos);
	void MoveFinish();

	GEntityNPC* GetOwnerNPC() const;

private:
	bool			m_bMoving;
	BOOL			m_bSuspended;
	GMoveStrategy*	m_pStrategy;
	MovingWay		m_nWay;
	int				m_nChangeWayCounter;
//	int				m_nNextChangeWay;

	vec3			m_vGoalPos;
	vec3			m_vGoalDir;
	vec3			m_vFinishDir;	// 마지막에 복구될 각도
	vec3			m_vRotateGoalDir;
	float			m_fLeastGap;
	GTaskMoveStrategy		m_nStrategyType;	

	vec3			m_vStartDir;
	vec3			m_vStartGoalPos;
};
