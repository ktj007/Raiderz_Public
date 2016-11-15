#pragma once

class GEntityNPC;

//////////////////////////////////////////////////////////////////////////
//
//		GMoveStrategy
//
class GMoveStrategy
{
protected:
	// 이동 관련
	GEntityNPC*		m_pOwner;
	vec3			m_vGoalPos;
	vec3			m_vGoalDir;
	float			m_fLeastGap;
	vec3			m_vOldPos;

protected:
	GEntityNPC* GetOwnerNPC();
	vec3	GetGoalPos();
	vec3	GetGoalDir();
	bool	IsReachPos(const vec3& vDestPos, float fDelta);	
	bool	IsReachGoalPos(float fDelta);
	
	bool	SetValidateZPos(vec3& vResultPos);
	
	void	RouteMoveStop();
	void	RouteMove(vec3 vDestPos);
public:
	GMoveStrategy(GEntityNPC* pOwner, float fLeastGap);
	virtual ~GMoveStrategy();
	
	/// 전략시작시 호출되는 함수.  반환값은 전략을 시작할지 여부 (false면 포기)
	virtual bool		Start(vec3 vGoalPos, vec3 vGoalDir, float fLeastGap);

	/// 상태 업데이트 함수.  반환값은 모든 처리가 종료됐는지 여부
	virtual void		Update(float fDelta)=0;

	bool	IsPathfindFailed();	
	bool	IsReachGoalPos();	

	virtual GTaskMoveStrategy GetType() = 0;
};

//////////////////////////////////////////////////////////////////////////
//
//		GDefiniteMoveStrategy (결정적 이동)
//

class GDefiniteMoveStrategy : public GMoveStrategy, public MTestMemPool<GDefiniteMoveStrategy>
{
protected:
	int				m_nMoveIndex;
	vec3			m_vWaypoint;
	vec3			m_vNextDir;
	vector<vec3>	m_vecWaypoints;
protected:
	bool	FindPath(  vec3 vStartPos, vec3& vDestPos);
	bool	IsReachWaypoint(float fDelta);
	bool	IsEndOfWaypoint();
	void	SetNextWaypoint();

	void SetNextWaypointDir();
	void	RouteDebugPathInfo();
	void	RouteMoveNextWayPoint();
public:
	GDefiniteMoveStrategy(GEntityNPC* pOwner, float fLeastGap);
	virtual ~GDefiniteMoveStrategy();

	/// 전략시작시 호출되는 함수.  반환값은 전략을 시작할지 여부 (false면 포기)
	virtual bool		Start(vec3 vGoalPos, vec3 vGoalDir, float fLeastGap);

	/// 상태 업데이트 함수.  반환값은 모든 처리가 종료됐는지 여부
	virtual void		Update(float fDelta);		

	virtual GTaskMoveStrategy GetType() { return MS_DEFINITE; }
	
};

//////////////////////////////////////////////////////////////////////////
//
//		GStraightMoveStrategy (길찾기 없이 직선 이동)
//

class GStraightMoveStrategy : public GMoveStrategy, public MTestMemPool<GStraightMoveStrategy>
{
public:
	GStraightMoveStrategy(GEntityNPC* pOwner, float fLeastGap);
	virtual ~GStraightMoveStrategy();

	/// 전략시작시 호출되는 함수.  반환값은 전략을 시작할지 여부 (false면 포기)
	virtual bool		Start(vec3 vGoalPos, vec3 vGoalDir, float fLeastGap);

	/// 상태 업데이트 함수.  반환값은 모든 처리가 종료됐는지 여부
	virtual void		Update(float fDelta);		

	virtual GTaskMoveStrategy GetType() { return MS_STRAIGHT; }
	
};




