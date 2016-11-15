#include "stdafx.h"
#include "PChatPlayer.h"
#include "PChatChannel.h"
#include "PGlobal.h"
#include "PChatChannelManager.h"
#include "PGameServerObjectManager.h"

PChatPlayer::PChatPlayer(CID cid, int nGameServer)
: m_cid(cid)
, m_nGameServerID(nGameServer)
{
	// do nothing
}

PChatPlayer::~PChatPlayer()
{
	// do nothing
}

void PChatPlayer::AddChannel(MUID uidChannel)
{
	m_setChannel.insert(uidChannel);
}

void PChatPlayer::RemoveChannel(MUID uidChannel)
{
	m_setChannel.erase(uidChannel);
}

void PChatPlayer::ChangedGameServerID(int nNewGameServerID)
{
	for(set<MUID>::iterator it = m_setChannel.begin(); it != m_setChannel.end(); it++)
	{
		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(*it);
		if (pChannel != NULL)
		{
			pChannel->ChangedGameServerID(this, nNewGameServerID);
		}
	}

	m_nGameServerID = nNewGameServerID;
}

CID PChatPlayer::GetCID(void) const
{
	return m_cid;
}

int PChatPlayer::GetGameServerID(void) const
{
	return m_nGameServerID;
}

bool PChatPlayer::IsEmptyChannel(void) const
{
	return m_setChannel.empty();
}

void PChatPlayer::GetChannelList(vector<MUID>& vecChannel)
{
	vecChannel.clear();
	vecChannel.resize(m_setChannel.size());
	copy(m_setChannel.begin(), m_setChannel.end(), vecChannel.begin());
}

int PChatPlayer::GetEnteredPrivateChannelCount()
{
	int nRet = 0;
	for(set<MUID>::iterator it = m_setChannel.begin(); it != m_setChannel.end(); it++)
	{
		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(*it);
		if (pChannel != NULL &&
			pChannel->GetType() == CCT_PRIVATE)
		{
			nRet++;
		}
	}
	return nRet;
}