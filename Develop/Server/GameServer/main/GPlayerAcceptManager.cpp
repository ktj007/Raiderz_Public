#include "stdafx.h"
#include "GPlayerAcceptManager.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GFakeLoginPlayerAcceptor_FromGameServerToLoginServer.h"
#include "GSystem.h"
#include "GGlobal.h"
#include "AStlUtil.h"

void GPlayerAcceptManager::GPlayerAcceptorMap::Update(uint32 nNowTime)
{
	for (iterator itor = begin(); itor != end(); )
	{
		GPlayerAcceptor* pAcceptor = (*itor).second;

		if (nNowTime - pAcceptor->GetReserveTime() >= TIMEOUT_TICK_MOVE_SERVER)
		{
			pAcceptor->OnTimeout();
			delete pAcceptor;
			itor = erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void GPlayerAcceptManager::GPlayerAcceptorMap::Clear()
{
	SAFE_DELETE_MAP(*this);	
}

bool GPlayerAcceptManager::GPlayerAcceptorMap::Add( GPlayerAcceptor* pAcceptor )
{
	if (FindPlayerAcceptor(pAcceptor->GetConnectionKey()) != NULL)
	{
		return false;
	}
	insert(value_type(pAcceptor->GetConnectionKey(), pAcceptor));

	return true;
}

void GPlayerAcceptManager::GPlayerAcceptorMap::Del( MUID uidConnectionKey )
{
	iterator itor = find(uidConnectionKey);
	if (itor != end())
	{
		delete (*itor).second;
		erase(itor);
	}
}

GPlayerAcceptor* GPlayerAcceptManager::GPlayerAcceptorMap::FindPlayerAcceptor( MUID uidConnectionKey )
{
	iterator itor = find(uidConnectionKey);
	if (itor != end()) return (*itor).second;

	return NULL;
}

void GPlayerAcceptManager::Update()
{
	uint32 nNowTime = gsys.pSystem->GetNowTime();

	m_LoginServerAcceptorMap.Update(nNowTime);
	m_GameServerAcceptorMap.Update(nNowTime);
}

GPlayerAcceptManager::GPlayerAcceptManager()
{

}

GPlayerAcceptManager::~GPlayerAcceptManager()
{
	m_LoginServerAcceptorMap.Clear();
	m_GameServerAcceptorMap.Clear();
}

bool GPlayerAcceptManager::Add( GPlayerAcceptor* pAcceptor, GPlayerAcceptor::AcceptorType nAcceptorType )
{
	if (nAcceptorType == GPlayerAcceptor::FROM_LOGIN_SERVER) return m_LoginServerAcceptorMap.Add(pAcceptor);
	return m_GameServerAcceptorMap.Add(pAcceptor);
}

void GPlayerAcceptManager::Del( MUID uidConnectionKey, GPlayerAcceptor::AcceptorType nAcceptorType )
{
	if (nAcceptorType == GPlayerAcceptor::FROM_LOGIN_SERVER) 
	{
		m_LoginServerAcceptorMap.Del(uidConnectionKey);
	}
	else
	{
		m_GameServerAcceptorMap.Del(uidConnectionKey);
	}
}

GPlayerAcceptor_FromLoginServer* GPlayerAcceptManager::FindPlayerAcceptor_FromLoginServer( MUID uidConnectionKey )
{
	GPlayerAcceptor* pAcceptor = m_LoginServerAcceptorMap.FindPlayerAcceptor(uidConnectionKey);
	if (pAcceptor && pAcceptor->GetType() == GPlayerAcceptor::FROM_LOGIN_SERVER)
	{
		return static_cast<GPlayerAcceptor_FromLoginServer*>(pAcceptor);
	}
	return NULL;
}

GPlayerAcceptor_FromGameServer* GPlayerAcceptManager::FindPlayerAcceptor_FromGameServer( MUID uidConnectionKey )
{
	GPlayerAcceptor* pAcceptor = m_GameServerAcceptorMap.FindPlayerAcceptor(uidConnectionKey);
	if (pAcceptor && pAcceptor->GetType() == GPlayerAcceptor::FROM_GAME_SERVER)
	{
		return static_cast<GPlayerAcceptor_FromGameServer*>(pAcceptor);
	}
	return NULL;
}

GFakeLoginPlayerAcceptor_FromGameServerToLoginServer* GPlayerAcceptManager::FindFakeLoginPlayerAcceptor_FromGameServerToLoginServer( MUID uidConnectionKey )
{
	GPlayerAcceptor* pAcceptor = m_GameServerAcceptorMap.FindPlayerAcceptor(uidConnectionKey);
	if (pAcceptor && pAcceptor->GetType() == GPlayerAcceptor::FAKE_LOGIN_FROM_GAME_SERVER_TO_LOGIN_SERVER)
	{
		return static_cast<GFakeLoginPlayerAcceptor_FromGameServerToLoginServer*>(pAcceptor);
	}
	return NULL;
}

size_t GPlayerAcceptManager::GetSize( GPlayerAcceptor::AcceptorType nAcceptorType )
{
	if (nAcceptorType == GPlayerAcceptor::FROM_LOGIN_SERVER) return m_LoginServerAcceptorMap.size(); 
	return m_GameServerAcceptorMap.size();
}

