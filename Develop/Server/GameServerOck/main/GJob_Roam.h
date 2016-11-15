#ifndef _GJOB_ROAM_H
#define _GJOB_ROAM_H

#include "MMemPool.h"
#include "GJob.h"

class GField;

/// 목적 위치로 이동하는 행동 모듈 (나중에 여기서 길찾기도 수행하도록 한다.)
class GJob_Roam : public GJob, public MMemPool<GJob_Roam>
{
private:
	enum PHASE
	{
		PHASE_ROAM = 0,
		PHASE_IDLE_COLT = 1,
		PHASE_IDLE_TASK = 2,
	};

	vec3					m_vCenter;
	float					m_fRadius;
	float					m_fPickDepth;
	PHASE					m_nPhase;
protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual void OnCancel();
	virtual void OnTaskComplete(GTaskID nTaskID);
			void OnIdleColt();
			void OnIdle();

	virtual void OnRoaming();
public:
	DECLARE_JOB_ID(GJOB_ROAM);
	virtual string GetParamString() const override;

	GJob_Roam(GModuleAI* pmAI, float fRadius, float fDurationTime=0);
	virtual ~GJob_Roam();

	float GetRadius()		{ return m_fRadius; }
};


#endif // _GJOB_MOVE_TO_POS_H