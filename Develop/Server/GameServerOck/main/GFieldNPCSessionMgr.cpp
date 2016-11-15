#include "StdAfx.h"
#include "GFieldNPCSessionMgr.h"
#include "GFieldNPCSession.h"
#include "AStlUtil.h"

GFieldNPCSessionMgr::GFieldNPCSessionMgr(void)
{
}

GFieldNPCSessionMgr::~GFieldNPCSessionMgr(void)
{
	Clear();
}

bool GFieldNPCSessionMgr::Add( GFieldNPCSession* pSession )
{
	VALID_RET(pSession, false);
	
	if (Find(pSession->GetName().c_str()))
	{
		mlog3("session_error: same session name already exist. (name: %s)", pSession->GetName().c_str());
		return false; // 같은 세션 이름이 이미 존재
	}

	m_mapSessions.insert(make_pair(pSession->GetUID(), pSession));
	return true;
}

void GFieldNPCSessionMgr::Remove( MUID uid )
{
	m_mapSessions.erase(uid);
}

size_t GFieldNPCSessionMgr::GetQty() const
{
	return m_mapSessions.size();
}

void GFieldNPCSessionMgr::Update( float fDelta )
{
	for (SessionMap::iterator it = m_mapSessions.begin();
		 it != m_mapSessions.end();)
	{
		GFieldNPCSession* pSession = it->second;
		pSession->Update(fDelta);

		if (pSession->HasExpired())
		{
			delete pSession;
			it = m_mapSessions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void GFieldNPCSessionMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapSessions);
}

GFieldNPCSession* GFieldNPCSessionMgr::Find( const char* pszName )
{
	// TODO: 검색 속도 향상 가능
	for each (SessionMap::value_type each in m_mapSessions)
	{
		if (string(each.second->GetName()) == pszName)
			return each.second;
	}

	return NULL;
}

GFieldNPCSession* GFieldNPCSessionMgr::Find( MUID uid )
{
	SessionMap::iterator it = m_mapSessions.find(uid);
	if (it == m_mapSessions.end())
		return NULL;

	return it->second; 
}
