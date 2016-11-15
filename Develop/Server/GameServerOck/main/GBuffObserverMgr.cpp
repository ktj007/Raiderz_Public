#include "StdAfx.h"
#include "GBuffObserverMgr.h"
#include "GBuffObserver.h"

#define NOTIFY_OBSERVERS_REVERSE(obs_type, set, event)					\
	vector<obs_type*>::reverse_iterator it = set.rbegin(); \
	for (it; it != set.rend(); ++it) \
	(*it)->event;									\

GBuffObserverMgr::GBuffObserverMgr(void)
{
}

GBuffObserverMgr::~GBuffObserverMgr(void)
{
}

void GBuffObserverMgr::AttachObserver( GBuffObserver* pOvr )
{
	VALID(pOvr);
	vector<GBuffObserver*>::iterator it = find(m_vecObservers.begin(), m_vecObservers.end(), pOvr);
	VALID(it == m_vecObservers.end());
	m_vecObservers.push_back(pOvr);
}

void GBuffObserverMgr::DetachObserver( GBuffObserver* pOvr )
{
	VALID(pOvr);
	vector<GBuffObserver*>::iterator it = find(m_vecObservers.begin(), m_vecObservers.end(), pOvr);
	VALID(it != m_vecObservers.end());
	m_vecObservers.erase(it);
}

void GBuffObserverMgr::OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	// GAIN 패킷을 먼저 쏘기위해 반대 순서로 알림
	NOTIFY_OBSERVERS_REVERSE(GBuffObserver, m_vecObservers, OnGain(uidUser, pTarget, pBuffInfo));
}

void GBuffObserverMgr::OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute)
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnLost(uidUser, pTarget, pBuffInfo, bRoute));
}

void GBuffObserverMgr::OnCancelled()
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnCancelled());
}

void GBuffObserverMgr::OnPeriod()
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnPeriod());
}

void GBuffObserverMgr::OnExpired(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnExpired(uidUser, pTarget, pBuffInfo));
}

void GBuffObserverMgr::OnDispelled()
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnDispelled());
}

void GBuffObserverMgr::OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnDuplicated(uidUser, pTarget, pBuffInfo));
}

void GBuffObserverMgr::OnMaxStacked()
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnMaxStacked());
}

void GBuffObserverMgr::OnTriggeredBuffAffected()
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnAffected());
}

void GBuffObserverMgr::OnStacked( float fDurationTime, float fPeriodTime )
{
	NOTIFY_OBSERVERS(GBuffObserver, m_vecObservers, OnStacked(fDurationTime, fPeriodTime));
}



