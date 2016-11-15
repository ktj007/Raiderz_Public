#ifndef _XTALENT_EVENT_H
#define _XTALENT_EVENT_H

#include "XTalentInfo.h"
#include "XEffectManager.h"
#include "XBaseTalentEvent.h"

class XTalentEvent;
class XBaseTalent;

//////////////////////////////////////////////////////////////////////////
// 탤런트 이벤트
class XTalentEvent : public XBaseTalentEvent
{
private:
	XObject *				m_pOwner;
	XBaseTalent *			m_pTalent;

private:
	void					CalMatrixTalentHitCapsule(int nTalentID, int nSegmentIndex, int nCapsuleIndex, MMatrix& matWorld);

protected:
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
	virtual void			TalentEvent_Camear_Lock(CSTalentEventInfo * pEvent);
	virtual void			TalentEvent_GroundFireEffect(CSTalentEventInfo * pEvent);

public:
	XTalentEvent(XBaseTalent* pTalent, XObject* pOwner = NULL);
	virtual ~XTalentEvent(void);

	XObject *				GetOwner() { return m_pOwner; }
	void					GetActTalentEvent(TALENT_EVENT eventtype, vector<CSTalentEventInfo>& outVecEvents);

	bool					IsCurrentTalent();

	virtual void			EventUpdate(float fDelta);
};






#endif // _XTALENT_EVENT_H