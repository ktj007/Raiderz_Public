#ifndef _XBASE_TALENT_EVENT_H
#define _XBASE_TALENT_EVENT_H

#pragma once

#include "XTalentEffectInfo.h"
#include "CSTalentInfo.h"

class XBaseTalentEvent;
class XBaseTalent;

//////////////////////////////////////////////////////////////////////////
// 탤런트 이벤트
typedef void(*TALENTEVENTHANDLER) (XBaseTalentEvent&, CSTalentEventInfo * );

struct TIMEEVENT
{
	bool					bInfiniteTime;
	EFFECT_DEL				EffectDel;
	float					fElapsedTime;

	MUID					uidSound;

	TIMEEVENT()
	{
		bInfiniteTime		= false;
		fElapsedTime		= 0.0f;
	}
};

class XBaseTalentEvent
{
protected:
	CSTalentEvents*			m_pCurrentTalentEvent;

	vector<TIMEEVENT>		m_vecEffectList;

	bool					m_bActStart;

	float					m_fNormalEventElapsedTime;
	float					m_fPreNormalEventElapsedTime;	
	float					m_fPreActEventElapsedTime;
	float					m_fActEventElapsedTime;

protected:
	void					UpdateTalentEventEffect(float fDelta);
	void					SetTalentEventEffectList(MUID uidEffect, CSTalentEventInfo * pEvent);
	void					SetTalentEventSoundList(MUID uidSound, CSTalentEventInfo * pEvent);
	void					DelTalentEventEffect(MUID uidEffect);
	virtual void			DelAllTalentEventEffect();

	virtual void			DeleteEventEffect(MUID & uid, bool bImmediatlyDelete = false) = 0;
	virtual void			DeleteEventSound(MUID & uid) = 0;
	virtual bool			CheckEventGo() { return true; }
	virtual bool			CheckLoopEffect(MUID uidEffect) { return false; }

	virtual void			TalentEvent_None(CSTalentEventInfo * pEvent) 			{}
	virtual void			TalentEvent_FireEffect(CSTalentEventInfo * pEvent) 		{}
	virtual void			TalentEvent_Projectile(CSTalentEventInfo * pEvent) 		{}
	virtual void			TalentEvent_Effect(CSTalentEventInfo * pEvent) 			{}
	virtual void			TalentEvent_Sound(CSTalentEventInfo * pEvent) 			{}
	virtual void			TalentEvent_Camera(CSTalentEventInfo * pEvent)			{}
	virtual void			TalentEvent_Custom(CSTalentEventInfo * pEvent)			{}
	virtual void			TalentEvent_Delayed_Act(CSTalentEventInfo * pEvent)		{}
	virtual void			TalentEvent_ExtraActive_1(CSTalentEventInfo * pEvent)	{}		// <-- 툴에서 쓰이고 있다.
	virtual void			TalentEvent_ExtraActive_2(CSTalentEventInfo * pEvent)	{}		// <-- 툴에서 쓰이고 있다.
	virtual void			TalentEvent_Camear_Lock(CSTalentEventInfo * pEvent)		{}
	virtual void			TalentEvent_GroundFireEffect(CSTalentEventInfo * pEvent) {}

public:
	XBaseTalentEvent();
	virtual ~XBaseTalentEvent();

	virtual void			EventInit();
	virtual void			EventsStart(CSTalentEvents* pEvents);
	virtual void			EventsEnd();
	virtual void			EventUpdate(float fDelta);
	void					EventNormalCheck();
	void					EventActCheck();
	void					EventGo(CSTalentEventInfo * pEvent);

	void					SetActStart(bool bActStart) { m_bActStart = bActStart; }

	static map<TALENT_EVENT, TALENTEVENTHANDLER> m_TalentEventHandlers;

	static void				InitTalentEventHandler();

	static void				OnTalentEvent_None(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_FireEffect(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Projectile(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Effect(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Sound(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Camera(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Custom(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Delayed_Act(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_ExtraActive_1(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_ExtraActive_2(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_Camera_Lock(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
	static void				OnTalentEvent_GroundFireEffect(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent);
};

#endif // _XBASE_TALENT_EVENT_H