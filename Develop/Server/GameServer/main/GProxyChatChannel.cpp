#include "stdafx.h"
#include "GProxyChatChannel.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GClientChatChannelRouter.h"
#include "GConfig.h"


GProxyChatPlayer::GProxyChatPlayer(CID nPlayerCID)
: m_CID(nPlayerCID)
, m_nGameServerID(INVALID_ID)
{
	// do nothing
}

GProxyChatPlayer::~GProxyChatPlayer()
{
	// do nothing
}

void GProxyChatPlayer::AddChannel(GProxyChatChannel* pChannel)
{
	VALID(pChannel != NULL);

	MUID uidChannel = pChannel->GetUID();
	m_mapChannel.insert(make_pair(uidChannel, pChannel));
}

void GProxyChatPlayer::RemoveChannel(MUID uidChannel)
{
	m_mapChannel.erase(uidChannel);	
}

CID GProxyChatPlayer::GetCID(void) const
{
	return m_CID;
}

int GProxyChatPlayer::GetGameServerID(void) const
{
	return m_nGameServerID;
}

void GProxyChatPlayer::SetGameServerID(int nGameServerID)
{
	m_nGameServerID = nGameServerID;
}

MUID GProxyChatPlayer::GetUID(void) const
{
	return m_UID;
}

void GProxyChatPlayer::SetUID(MUID uidPlayer)
{
	m_UID = uidPlayer;
}

bool GProxyChatPlayer::IsEmptyChannel(void) const
{
	return m_mapChannel.empty();
}

void GProxyChatPlayer::RemoveFromAllChannel(void)
{
	for(channel_map::iterator it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
	{
		GProxyChatChannel* pChannel = it->second;
		pChannel->RemovePlayer(m_CID);
	}
	m_mapChannel.clear();
}

GProxyChatChannel::GProxyChatChannel(MUID uidChannel)
: m_UID(uidChannel)
{
	// do nothing
}

GProxyChatChannel::~GProxyChatChannel()
{
	// do nothing
}

MUID GProxyChatChannel::GetUID(void) const
{
	return m_UID;
}

void GProxyChatChannel::SetName(wstring strName)
{
	m_strName = strName;
}

wstring GProxyChatChannel::GetName(void) const
{
	return m_strName;
}

void GProxyChatChannel::AddPlayer(GProxyChatPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	CID nCID = pPlayer->GetCID();
	m_mapPlayer.insert(make_pair(nCID, pPlayer));
}

void GProxyChatChannel::RemovePlayer(CID nCID)
{
	m_mapPlayer.erase(nCID);
}

GProxyChatPlayer* GProxyChatChannel::FindPlayer(CID nCID)
{
	player_map::iterator it = m_mapPlayer.find(nCID);
	if (it == m_mapPlayer.end())	return NULL;

	return it->second;
}

int GProxyChatChannel::GetPlayerCount(void) const
{
	return m_mapPlayer.size();
}

player_map::iterator GProxyChatChannel::BeginPlayer(void)
{
	return m_mapPlayer.begin();
}

player_map::iterator GProxyChatChannel::EndPlayer(void)
{
	return m_mapPlayer.end();
}

GProxyChatChannelManager::GProxyChatChannelManager()
{
	// do nothing
}

GProxyChatChannelManager::~GProxyChatChannelManager()
{
	Clear();
}

bool GProxyChatChannelManager::AddChannel(MUID uidChannel, wstring strChannelName)
{
	channel_map::iterator it = m_mapChannel.find(uidChannel);
	if (it != m_mapChannel.end())	return false;

	GProxyChatChannel* pChannel = new GProxyChatChannel(uidChannel);
	pChannel->SetName(strChannelName);
	m_mapChannel.insert(make_pair(uidChannel, pChannel));

	return true;
}

bool GProxyChatChannelManager::RemoveChannel(MUID uidChannel)
{
	GProxyChatChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL)	return false;

	for (player_map::iterator it = pChannel->BeginPlayer(); it != pChannel->EndPlayer(); it++)
	{
		CID nCID = it->first;
		GProxyChatPlayer* pPlayer = it->second;

		pPlayer->RemoveChannel(uidChannel);
		if (pPlayer->IsEmptyChannel())
		{
			SAFE_DELETE(pPlayer);
			m_mapPlayer.erase(nCID);
		}
	}

	SAFE_DELETE(pChannel);
	m_mapChannel.erase(uidChannel);
	
	return true;
}

bool GProxyChatChannelManager::AddPlayer(MUID uidChannel, CID nCID, int nGameServerID, MUID uidPlayer)
{
	// 채널 확인
	GProxyChatChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL)	return false;

	GProxyChatPlayer* pPlayer = pChannel->FindPlayer(nCID);
	if (pPlayer != NULL)	return false;


	// 플레이어 확인
	player_map::iterator it = m_mapPlayer.find(nCID);
	if (it == m_mapPlayer.end())
	{
		pPlayer = new GProxyChatPlayer(nCID);
		pPlayer->SetGameServerID(nGameServerID);
		pPlayer->SetUID(uidPlayer);
		m_mapPlayer.insert(make_pair(nCID, pPlayer));
	}
	else
	{
		pPlayer = it->second;
	}

	// 채널에 추가	
	pChannel->AddPlayer(pPlayer);
	pPlayer->AddChannel(pChannel);

	return true;
}

bool GProxyChatChannelManager::RemovePlayer(MUID uidChannel, CID nCID)
{
	// 채널 확인
	GProxyChatChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL)	return false;

	GProxyChatPlayer* pPlayer = pChannel->FindPlayer(nCID);
	if (pPlayer == NULL)	return false;

	pChannel->RemovePlayer(nCID);
	pPlayer->RemoveChannel(uidChannel);

	if (pPlayer->IsEmptyChannel())
	{
		SAFE_DELETE(pPlayer);
		m_mapPlayer.erase(nCID);
	}

	return true;
}

bool GProxyChatChannelManager::RemovePlayer(CID nCID)
{
	GProxyChatPlayer* pPlayer = FindPlayer(nCID);
	if (pPlayer == NULL)	return false;

	pPlayer->RemoveFromAllChannel();
	SAFE_DELETE(pPlayer);
	m_mapPlayer.erase(nCID);

	return true;
}

bool GProxyChatChannelManager::UpdatePlayerInLocal(CID nCID, int nGameServerID, MUID uidPlayerInLocal)
{
	GProxyChatPlayer* pPlayer = FindPlayer(nCID);
	if (pPlayer == NULL)	return false;

	pPlayer->SetGameServerID(nGameServerID);
	pPlayer->SetUID(uidPlayerInLocal);
	return true;
}

void GProxyChatChannelManager::Clear(void)
{
	for(channel_map::iterator it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_mapChannel.clear();

	for(player_map::iterator it = m_mapPlayer.begin(); it != m_mapPlayer.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_mapPlayer.clear();
}

GProxyChatChannel* GProxyChatChannelManager::FindChannel(MUID uidChannel)
{
	channel_map::iterator it = m_mapChannel.find(uidChannel);
	if (it == m_mapChannel.end())	return NULL;

	return it->second;
}

int GProxyChatChannelManager::GetChannelCount(void) const
{
	return m_mapChannel.size();
}

GProxyChatPlayer* GProxyChatChannelManager::FindPlayer(CID nCID)
{
	player_map::iterator it = m_mapPlayer.find(nCID);
	if (it == m_mapPlayer.end())	return NULL;

	return it->second;
}

int GProxyChatChannelManager::GetPlayerCount(void) const
{
	return m_mapPlayer.size();
}

int GProxyChatChannelManager::GetPlayerCount(MUID uidChannel)
{
	GProxyChatChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL)	return 0;

	return pChannel->GetPlayerCount();
}

void GProxyChatChannelManager::DisconnectAppServer()
{
	GClientChatChannelRouter router;

	channel_map::iterator itrChannel = m_mapChannel.begin();
	for (; itrChannel != m_mapChannel.end(); ++itrChannel)
	{
		GProxyChatChannel* pChatChannel = (*itrChannel).second;

		router.LeaveChannelAllPlayer(pChatChannel->GetUID());
	}

	Clear();
}
