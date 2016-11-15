#ifndef _GJOB_MISC_H
#define _GJOB_MISC_H

#include "MMemPool.h"
#include "GJob.h"

class GEntityActor;

class GJob_Speak : public GJob, public MMemPool<GJob_Speak>
{
private:
	wstring			m_strType;
	wstring			m_strText;
	float			m_fMaintainTime;
private:
	void SpeakImpl();
protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
public:
	DECLARE_JOB_ID(GJOB_SPEAK);
	virtual string GetParamString() const override;

	GJob_Speak(GModuleAI* pmAI, wstring strType, wstring strText, float fMaintainTime);
	virtual ~GJob_Speak();
};







#endif