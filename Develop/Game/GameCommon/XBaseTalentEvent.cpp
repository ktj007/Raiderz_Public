#include "stdafx.h"
#include "XBaseTalentEvent.h"

XBaseTalentEvent::XBaseTalentEvent()
{
	EventInit();
}

XBaseTalentEvent::~XBaseTalentEvent()
{
	m_vecEffectList.clear();
	m_pCurrentTalentEvent			= NULL;

	DelAllTalentEventEffect();
}

void XBaseTalentEvent::EventInit()
{
	m_pCurrentTalentEvent			= NULL;
	m_bActStart						= false;

	m_fPreNormalEventElapsedTime	= 0.0f;
	m_fNormalEventElapsedTime		= 0.0f;
	m_fPreActEventElapsedTime		= 0.0f;
	m_fActEventElapsedTime			= 0.0f;

	DelAllTalentEventEffect();
}

void XBaseTalentEvent::EventsStart( CSTalentEvents* pEvents )
{
	EventInit();

	m_pCurrentTalentEvent = pEvents;
}

void XBaseTalentEvent::EventsEnd()
{
	EventInit();
}

void XBaseTalentEvent::EventUpdate( float fDelta )
{
	if(m_pCurrentTalentEvent)
	{
		// 일반
		EventNormalCheck();

		m_fPreNormalEventElapsedTime = m_fNormalEventElapsedTime;
		m_fNormalEventElapsedTime += fDelta;

		// Act
		if(m_bActStart)
		{
			EventActCheck();

			m_fPreActEventElapsedTime = m_fActEventElapsedTime;
			m_fActEventElapsedTime +=  fDelta;
		}

		// Loop Effect
		UpdateTalentEventEffect(fDelta);
	}
}

void XBaseTalentEvent::EventNormalCheck()
{
	//------------------------------------------------------------------------
	// 일반
	for(vector<CSTalentEventInfo>::iterator itNor = m_pCurrentTalentEvent->m_vecEvents.begin(); itNor != m_pCurrentTalentEvent->m_vecEvents.end(); ++itNor)
	{
		CSTalentEventInfo * pEvent = &(*itNor);

		if(m_fPreNormalEventElapsedTime <= pEvent->m_fTime1 &&
			m_fNormalEventElapsedTime > pEvent->m_fTime1)
		{
			// 이벤트 발생
			EventGo(pEvent);
		}
	}
}

void XBaseTalentEvent::EventActCheck()
{
	//------------------------------------------------------------------------
	// Act
	for(vector<CSTalentEventInfo>::iterator itAct = m_pCurrentTalentEvent->m_vecActEvents.begin(); itAct != m_pCurrentTalentEvent->m_vecActEvents.end(); ++itAct)
	{
		CSTalentEventInfo * pEvent = &(*itAct);

		if(m_fPreActEventElapsedTime <= pEvent->m_fTime1 &&
			m_fActEventElapsedTime > pEvent->m_fTime1)
		{
			// 이벤트 발생
			EventGo(pEvent);
		}
	}
}

void XBaseTalentEvent::EventGo( CSTalentEventInfo * pEvent )
{
	if(CheckEventGo() == false)
		return ;

	map<TALENT_EVENT, TALENTEVENTHANDLER>::iterator itFind = m_TalentEventHandlers.find(pEvent->m_nEvent);
	if(itFind != m_TalentEventHandlers.end())
	{
		itFind->second(*this, pEvent);
	}
}

map<TALENT_EVENT, TALENTEVENTHANDLER> XBaseTalentEvent::m_TalentEventHandlers;

void XBaseTalentEvent::InitTalentEventHandler()
{
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_NONE,				OnTalentEvent_None) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_FIRE_EFFECT,		OnTalentEvent_FireEffect) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_LAUNCH_PROJECTILE,	OnTalentEvent_Projectile) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_EFFECT,				OnTalentEvent_Effect) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_SOUND,				OnTalentEvent_Sound) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_CAMERA,				OnTalentEvent_Camera) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_CUSTOM,				OnTalentEvent_Custom) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_DELAYED_ACT,		OnTalentEvent_Delayed_Act) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_EXTRAACTIVE_1,		OnTalentEvent_ExtraActive_1) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_EXTRAACTIVE_2,		OnTalentEvent_ExtraActive_2) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_CAMEAR_LOCK,		OnTalentEvent_Camera_Lock) );
	m_TalentEventHandlers.insert( map<TALENT_EVENT, TALENTEVENTHANDLER>::value_type( TE_GROUND_FIRE_EFFECT,	OnTalentEvent_GroundFireEffect) );
}

void XBaseTalentEvent::OnTalentEvent_None( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_None(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_FireEffect( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_FireEffect(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Projectile( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_Projectile(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Effect( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_Effect(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Sound( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_Sound(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Camera( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_Camera(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Custom( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_Custom(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_ExtraActive_1( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_ExtraActive_1(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_ExtraActive_2( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_ExtraActive_2(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Delayed_Act(XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent)
{
	TalentEvent.TalentEvent_Delayed_Act(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_Camera_Lock( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_Camear_Lock(pEvent);
}

void XBaseTalentEvent::OnTalentEvent_GroundFireEffect( XBaseTalentEvent& TalentEvent, CSTalentEventInfo * pEvent )
{
	TalentEvent.TalentEvent_GroundFireEffect(pEvent);
}

void XBaseTalentEvent::UpdateTalentEventEffect( float fDelta )
{
	vector<TIMEEVENT>::iterator it = m_vecEffectList.begin();
	while(it != m_vecEffectList.end())
	{
		(*it).fElapsedTime -= fDelta;

		if((*it).bInfiniteTime == false &&						// 시간이 무한대가 아니다.
			(*it).fElapsedTime <= 0.0f)
		{
			DeleteEventEffect((*it).EffectDel.uidEffect, (*it).EffectDel.bImmediatlyDelete);
			DeleteEventSound((*it).uidSound);
			it = m_vecEffectList.erase(it);
			continue;
		}

		it++;
	}
}

void XBaseTalentEvent::SetTalentEventEffectList( MUID uidEffect, CSTalentEventInfo * pEvent )
{
	bool bImme = false;
	if(pEvent->m_nParam2 == 1)
		bImme = true;

	TIMEEVENT timeEvent;
	timeEvent.EffectDel.uidEffect			= uidEffect;
	timeEvent.EffectDel.bImmediatlyDelete	= bImme;

	if(pEvent->m_fTime2 <= 0)
	{
		// 탤런트가 끝날때까지 종료를 하지 않기 위해서 값을 많이 준다. 무한처럼 보이게 한다.
		timeEvent.bInfiniteTime = true;
	}
	else
	{
		timeEvent.fElapsedTime					= pEvent->m_fTime2 - pEvent->m_fTime1;
	}

	m_vecEffectList.push_back(timeEvent);
}

void XBaseTalentEvent::DelTalentEventEffect( MUID uidEffect )
{
	vector<TIMEEVENT>::iterator it = m_vecEffectList.begin();
	while(it != m_vecEffectList.end())
	{
		if((*it).EffectDel.uidEffect == uidEffect)
		{
			DeleteEventEffect(uidEffect, (*it).EffectDel.bImmediatlyDelete);
			m_vecEffectList.erase(it);
			return;
		}

		if((*it).uidSound == uidEffect)
		{
			DeleteEventSound(uidEffect);
			m_vecEffectList.erase(it);
			return;
		}

		it++;
	}
}

void XBaseTalentEvent::DelAllTalentEventEffect()
{
	vector<TIMEEVENT>::iterator it = m_vecEffectList.begin();
	while(it != m_vecEffectList.end())
	{
		DeleteEventEffect((*it).EffectDel.uidEffect, (*it).EffectDel.bImmediatlyDelete);
		DeleteEventSound((*it).uidSound);
		it++;
	}
}

void XBaseTalentEvent::SetTalentEventSoundList( MUID uidSound, CSTalentEventInfo * pEvent )
{
	TIMEEVENT timeEvent;
	timeEvent.uidSound = uidSound;

	if(pEvent->m_fTime2 <= 0)
	{
		// 탤런트가 끝날때까지 종료를 하지 않기 위해서 값을 많이 준다. 무한처럼 보이게 한다.
		timeEvent.bInfiniteTime = true;
	}
	else
	{
		timeEvent.fElapsedTime					= pEvent->m_fTime2 - pEvent->m_fTime1;
	}

	m_vecEffectList.push_back(timeEvent);
}
