#include "stdafx.h"
#include "XLoadingMgr.h"
#include "XLoadingStarter.h"
#include "XLoadingCompleter.h"

XLoadingMgr::XLoadingMgr(XLoadingStarter* pStarter, XLoadingCompleter* pCompleter)
: m_pStarter(pStarter)
, m_pCompleter(pCompleter)
, m_nState(STATE_READY)
{

}

XLoadingMgr::~XLoadingMgr()
{
	ClearCheckers();
	SAFE_DELETE(m_pStarter);
	SAFE_DELETE(m_pCompleter);
}

void XLoadingMgr::Start()
{
	m_nState = STATE_STARTED;

	if (m_pStarter) m_pStarter->Start();
}

void XLoadingMgr::WorldLoadingFinish()
{
	if (m_pCompleter) m_pCompleter->WorldLoadingFinish();
	m_nState = STATE_WORLD_LOADED;
}

void XLoadingMgr::Complete()
{
	if (m_pCompleter) m_pCompleter->Complete();
	m_nState = STATE_COMPLETED;
}

bool XLoadingMgr::CheckCompleted()
{
	switch ( m_nState)
	{
	case STATE_READY :
		break;

	case STATE_STARTED :
		for ( vector< XLoadingChecker*>::iterator itr = m_vecCheckers.begin();  itr != m_vecCheckers.end();  itr++)
		{
			if ( (*itr)->IsWorldLoaded() == false)
				return false;
		}
		m_nState = STATE_WORLD_LOADED;
		break;


	case STATE_WORLD_LOADED :
		for ( vector< XLoadingChecker*>::iterator itr = m_vecCheckers.begin();  itr != m_vecCheckers.end();  itr++)
		{
			if ( (*itr)->IsCompleted() == false)
				return false;
		}
		m_nState = STATE_COMPLETED;
		return true;

	case STATE_COMPLETED :
		return false;
	}

	return false;


/*
	if (IsLoading() == false) return true;

	if (m_nState == STATE_STARTED)
	{
		for (size_t i = 0; i < m_vecCheckers.size(); ++i)
		{
			XLoadingChecker* pChecker = m_vecCheckers[i];
			if (pChecker->IsWorldLoaded() == false) return false;
		}

		WorldLoadingFinish();
	}

	for (size_t i = 0; i < m_vecCheckers.size(); ++i)
	{
		XLoadingChecker* pChecker = m_vecCheckers[i];
		if (pChecker->IsCompleted() == false) return false;
	}

	if (m_nState == STATE_WORLD_LOADED)
	{
		Complete();
	}

	return true;
*/
}

void XLoadingMgr::ClearCheckers()
{
	for (size_t i = 0; i < m_vecCheckers.size(); ++i)
	{
		XLoadingChecker* pChecker = m_vecCheckers[i];
		delete pChecker;
	}

	m_vecCheckers.clear();
}

void XLoadingMgr::AddChecker( XLoadingChecker* pChecker )
{
	m_vecCheckers.push_back(pChecker);
}

bool XLoadingMgr::IsLoading()
{
	if (m_nState >= STATE_STARTED && m_nState < STATE_COMPLETED)
	{
		return true;
	}
	return false;
}

void XLoadingMgr::Event( XEvent& evt )
{
	for (size_t i = 0; i < m_vecCheckers.size(); ++i)
	{
		XLoadingChecker* pChecker = m_vecCheckers[i];
		pChecker->Event(evt);
	}
}