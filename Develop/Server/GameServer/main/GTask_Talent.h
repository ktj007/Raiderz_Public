#ifndef _GTASK_TALENT_H
#define _GTASK_TALENT_H

#include "GTask.h"
#include "MTime.h"

class GTalentInfo;

class GTask_Talent : public GTask
{
public:
	GTask_Talent(GEntityNPC* pOwner);

	int		GetTalentID()	{ return m_nTalentID; }
protected:
	int		m_nTalentID;
};

/// 스킬 사용하는 태스크
class GTask_UseTalent : public GTask_Talent, public MTestMemPool<GTask_UseTalent>
{
private:
	GTalentInfo* m_pTalentInfo;
	MUID		m_uidTarget;
	bool		m_bGainStress;

	bool		m_bUsedTalent;
	bool		m_bSetDirection;

	void		UseTalent();
protected:
	virtual bool OnStart();
	virtual GTaskResult OnRun(float fDelta);

	bool IsTalentCompleted();
	bool IsTalentCanceled();
	virtual void OnComplete();
	virtual void OnCancel();
public:
	DECLARE_TASK_ID(GTASK_TALENT);

	GTask_UseTalent(GEntityNPC* pEntity, GTalentInfo* pTalentInfo, MUID uidTarget=0, bool bGainStress=true, bool bSetDirection=true);
	virtual ~GTask_UseTalent();
	virtual const wchar_t* GetTaskName() { return L"Talent"; }
	const GTalentInfo*	GetTalentInfo()	  { return m_pTalentInfo; }
	
};



class GTask_Guard : public GTask_Talent, public MTestMemPool<GTask_Guard>
{
private:
	float	m_fElapsed;
	float	m_fGuardingTime;
	float	m_fPostDelay;
	bool	m_bReleased;
	
protected:
	virtual bool OnStart();
	virtual GTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual void OnCancel();
	void GuardRelease();
public:
	DECLARE_TASK_ID(GTASK_GUARD);

	GTask_Guard(GEntityNPC* pEntity, int nGuardTalentID, float fGuardingTime);
	virtual ~GTask_Guard();
	virtual const wchar_t* GetTaskName() { return L"Guard"; }
};

#endif