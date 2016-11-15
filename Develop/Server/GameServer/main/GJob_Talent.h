#ifndef _GJOB_TALENT_H
#define _GJOB_TALENT_H

#include "MMemPool.h"
#include "GJob.h"

class GTalentInfo;

class GJob_Talent : public GJob
{
public:
	GJob_Talent(GModuleAI* pmAI, MUID uidTarget=0);

	MUID	GetTargetActor() const				{ return m_uidTargetActor; }
	void	SetTargetActor(MUID val)				{ m_uidTargetActor = val; }
	int		GetTalentID()					{ return m_nTalentID; }
	void	SetGainStress(bool bGained)		{ m_bGainStress = bGained; }

	virtual string GetParamString() const override;
protected:
	int		m_nTalentID;
	bool	m_bGainStress;
	MUID	m_uidTargetActor;
	
};

/// 스킬 사용하는 태스크
class GJob_UseTalent : public GJob_Talent, public MMemPool<GJob_UseTalent>
{
private:
	float		m_fElapsed;
	GTalentInfo* m_pTalentInfo;
	bool		m_bRunMainTalent;
	bool		m_bRotatable;

private:
	void UseTalent();

	void UseMainTalent();
protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual void OnCancel();
	virtual void OnTaskComplete(GTaskID nTaskID);

public:
	DECLARE_JOB_ID(GJOB_TALENT);

	GJob_UseTalent(GModuleAI* pmAI, GTalentInfo* pTalentInfo, MUID uidTarget=0);
	virtual ~GJob_UseTalent();
	const GTalentInfo*	GetTalentInfo()		{ return m_pTalentInfo; }
	bool IsRunMainTalent()					{ return m_bRunMainTalent; }
	
	void SetRotatable(bool bEnable)			{ m_bRotatable = bEnable; }
};



class GJob_Guard : public GJob_Talent, public MMemPool<GJob_Guard>
{
private:
	float	m_fGuardingTime;
	
protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
	virtual void OnComplete() {}
	virtual void OnCancel() {}
public:
	DECLARE_JOB_ID(GJOB_GUARD);

	GJob_Guard(GModuleAI* pmAI, int nGuardTalentID, float fGuardingTime);
	virtual ~GJob_Guard();
};

#endif