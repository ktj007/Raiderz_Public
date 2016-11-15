#include "stdafx.h"
#include "SProxyPlayer.h"
#include "STransData.h"

SProxyPlayer::SProxyPlayer(CID cid)
: m_CID(cid)
, m_nGameServerID(INVALID_ID)
, m_nGuildID(INVALID_ID)
, m_nFieldID(INVALID_ID)
, m_nChannelID(INVALID_CHANNELID)
{
	// do nothing
}

SProxyPlayer::~SProxyPlayer()
{
	// do nothing
}

bool SProxyPlayer::Update(const TD_PROXY_PLAYER_INFO& info)
{
	if (m_CID != info.nCID)	return false;

	m_nAID = info.nAID;
	m_UID = info.UID;
	m_strName = info.szName;
	m_nGameServerID = info.nGameServerID;
	m_uidField = info.uidField;
	m_uidParty = info.uidParty;	
	m_nGuildID = info.nGuildID;
	m_nFieldID = info.nFieldID;
	m_nChannelID = info.nChannelID;
	m_uidFieldGroup = info.uidFieldGroup;

	return true;
}

AID SProxyPlayer::GetAID( void ) const
{
	return m_nAID;
}

CID SProxyPlayer::GetCID(void) const
{
	return m_CID;
}

MUID SProxyPlayer::GetUID(void) const
{
	return m_UID;
}

wstring SProxyPlayer::GetName(void) const
{
	return m_strName;
}

int SProxyPlayer::GetGameServerID(void) const
{
	return m_nGameServerID;
}

MUID SProxyPlayer::GetFieldUID(void) const
{
	return m_uidField;
}

MUID SProxyPlayer::GetPartyUID(void) const
{
	return m_uidParty;
}

int SProxyPlayer::GetFieldID(void) const
{
	return m_nFieldID;
}

int SProxyPlayer::GetChannelID(void) const
{
	return m_nChannelID;
}

MUID SProxyPlayer::GetFieldGroupUID(void) const
{
	return m_uidFieldGroup;
}

void SProxyPlayer::SetUID(MUID uid)
{
	m_UID = uid;
}

void SProxyPlayer::SetName(wstring strName)
{
	m_strName = strName;
}

void SProxyPlayer::SetGameServerID(int nGameServerID)
{
	m_nGameServerID = nGameServerID;
}

void SProxyPlayer::SetFieldUID(MUID uidField)
{
	m_uidField = uidField;
}

void SProxyPlayer::SetPartyUID(MUID uidParty)
{
	m_uidParty = uidParty;
}

void SProxyPlayer::SetFieldID(int nFieldID)
{
	m_nFieldID = nFieldID;
}

void SProxyPlayer::SetChannelID(int nChannelID)
{
	m_nChannelID = nChannelID;
}

void SProxyPlayer::SetFieldGroupUID(MUID uidFieldGroup)
{
	m_uidFieldGroup = uidFieldGroup;
}