#include "stdafx.h"
#include "GServerCloser.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GAsyncTaskFactory.h"
#include "GServer.h"


GServerCloser::GServerCloser(): m_state(SLEEP)
{
	// do nothing
}

void GServerCloser::CloseServer(void)
{
	if (m_state == SLEEP)
	{
		m_state = BEGIN;
	}
}

bool GServerCloser::Update(void)
{
	switch(m_state)
	{
	case SLEEP:	
		// do nothing
		break;

	case BEGIN:
		LogoutAllPlayer();
		m_state = WAIT;
		break;

	case WAIT:
		if (IsComplateLogout())
		{
			m_state = END;
		}
		break;

	case END:
		return false;		// 종료 조건

	default:
		break;
	}

	return true;
}

void GServerCloser::LogoutAllPlayer(void) const
{
	mlog("Begin logout all players.\n");

	for each (const GPlayerObjectManager::GPlayerObjectMap::value_type& data in gmgr.pPlayerObjectManager->GetObjects())	
	{		
		gsys.pServer->Disconnect(data.first);
	}
}

bool GServerCloser::IsComplateLogout(void) const
{
	if (gmgr.pPlayerObjectManager->GetPlayersCount() <= 0)
	{
		mlog("End logout all players.\n");

		return true;
	}

	return false;
}

bool GServerCloser::IsRunServerCloser(void)
{
	return m_state != SLEEP;
}
