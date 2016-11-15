#ifndef _GJOB_MOVE_TO_POS_AIR_H_
#define _GJOB_MOVE_TO_POS_AIR_H_

#include "GJob_Movement.h"

/// 목적 위치로 날아서 이동하는 행동 모듈
class GJob_MoveToPos_Air : public GJob_Movement
{
private:
	vec3			m_vFinalTarPos;				///< 최종 목적지
	float			m_fLimitSumOfPathLength;	///< 최종 목적지까지 가는데 사용될수 있는 길이 총합의 제한값
	bool			m_bRun;
	float			m_fSpeed;
	bool			m_bPathFindFailed;
	bool			m_bNowRetreat;

protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
	virtual bool OnComplete();
	virtual bool OnCancel();
	virtual void OnSuspend();
	virtual void OnResume();

public:
	GJob_MoveToPos_Air(GModuleAI* pmAI, vec3 vTarPos, float fSpeed, bool bRun, bool bNowRetreat, float fLimitSumOfPathLength);
	virtual ~GJob_MoveToPos_Air();
	DECLARE_JOB_ID(GJOB_MOVE_TO_POS_AIR);
	virtual const char* GetJobName() { return "MoveToPosAir"; }
};


#endif //_GJOB_MOVE_TO_POS_AIR_H_