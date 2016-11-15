#pragma once

#include "GJob.h"
#include "MTime.h"
#include "MMemPool.h"

class GEntityActor;

/// 목적 엔티티 위치로 이동하는 행동 모듈
class GJob_MoveToEntity : public GJob, public MMemPool<GJob_MoveToEntity>
{
private:
	float			m_fLeastGap;				///< findpath에서 사용하는 최소 거리
	MUID			m_uidTargetEntity;
	bool			m_bInifity;					// 계속 쫓아다니기
	MRegulator		m_FindPathRegulator;
	vec3			m_vLastTarEntityPos;		///< 마지막으로 findpath 했을때의 타겟 엔티티 위치
	vec3			m_vLastTarEntityDir;		///< 마지막으로 findpath 했을때의 타겟 엔티티 방향
private:
	virtual bool		OnStart();
	virtual	GJobResult	OnRun(float fDelta);
	virtual void		OnTaskComplete(GTaskID nTaskID);
	virtual void		OnCancel();
	virtual void		OnResume();
	virtual void		OnComplete();
	virtual void		OnTimeout();						///< Job 수행시간 만료

	bool			IsTargetMoved( vec3 vCurTarEntityPos );
	bool			MoveToEntity();
	bool			IsArriveTarEntity(const vec3& vTarEntityPos, float fDelta=0.0f);			
	bool			CheckFindPathTick(vec3 vCurTarEntityPos, float fDelta);
	GEntityActor*	GetTargetEntity();
public:
	DECLARE_JOB_ID(GJOB_MOVE_TO_ENTITY);
	virtual string GetParamString() const override;

	GJob_MoveToEntity(GModuleAI* pmAI, MUID uidTargetEntity, float fLeastDistance=0.0f, bool bSetInifity=false);
	virtual ~GJob_MoveToEntity();

	bool			GetTargetEntityPos(vec3& voutTarPos);
	bool			GetTargetEntityDir(vec3& voutTarDir);
	float			GetLeastDistance()						{ return m_fLeastGap; }			

};

