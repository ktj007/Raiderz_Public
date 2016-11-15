#ifndef _GJOB_MOVE_TO_POS_H
#define _GJOB_MOVE_TO_POS_H

#include "MMemPool.h"
#include "GJob.h"

/// 목적 위치로 이동하는 행동 모듈
class GJob_MoveToPos : public GJob, public MMemPool<GJob_MoveToPos>
{
private:
	vec3			m_vTarPos;					///< 최종 목적지
	vec3			m_vTarDir;					///< 최종 목적지
	float			m_fLimitSumOfPathLength;	///< 최종 목적지까지 가는데 사용될수 있는 길이 총합의 제한값
	unsigned long	m_nMarkerID;				///< 최종 목적지가 마커일때 사용

private:
	void CallScript();

protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
	virtual void OnComplete();	

public:
	DECLARE_JOB_ID(GJOB_MOVE_TO_POS);
	virtual string GetParamString() const override;

	GJob_MoveToPos(GModuleAI* pmAI, vec3 vTarPos, vec3 vTarDir, float fLimitSumOfPathLength = FLT_MAX, unsigned long nMarkerID=0);
	virtual ~GJob_MoveToPos();

	// for test
	vec3	GetTarPos()		{ return m_vTarPos; }
};


#endif // _GJOB_MOVE_TO_POS_H