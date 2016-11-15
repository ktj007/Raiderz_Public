#pragma once

#include <map>
using namespace std;

#include "XBaseTalentEvent.h"

class CSTalentEventInfo;
class TTalentEvent;

class TTalentEvent : public XBaseTalentEvent
{
private:
	TCharacter*				m_pActor;
	CSTalentInfo*			m_pTalent;

	bool					m_bPause;

protected:
	virtual void			DelAllTalentEventEffect();

	virtual void			DeleteEventEffect(MUID & uid, bool bImmediatlyDelete = false);
	virtual void			DeleteEventSound(MUID & uid);
	virtual bool			CheckEventGo();
	virtual bool			CheckLoopEffect(MUID uidEffect);

	virtual void			TalentEvent_None(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_FireEffect(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_Projectile(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_Effect(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_Sound(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_Camera(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_Custom(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_Delayed_Act(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_ExtraActive_1(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_ExtraActive_2(CSTalentEventInfo * pEvent);

public:
	TTalentEvent();
	virtual ~TTalentEvent(void);

	void					SetActor(TCharacter* pActor) { m_pActor = pActor; }
	void					SetTalent(CSTalentInfo* pTalent) { m_pTalent = pTalent; }
	void					SetPause(bool bPause) { m_bPause = bPause; }

	void					SetTalentEventTime(float fActTime, float fTime);

	void					GetActTalentEvent(TALENT_EVENT eventtype, vector<CSTalentEventInfo>& outVecEvents);
	bool					GetPause() { return m_bPause; }

	virtual void			EventsStart(CSTalentEvents* pEvents);
	virtual void			EventUpdate(float fDelta);
};
