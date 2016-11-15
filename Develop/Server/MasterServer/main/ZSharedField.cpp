#include "StdAfx.h"
#include "ZSharedField.h"


ZSharedField::ZSharedField(MUID uid, ZFieldInfo* pFieldInfo, int nChannelID, bool bIsDefaultChannel, int m_nGameServerID)
: ZField(uid, pFieldInfo, m_nGameServerID)
, m_nChannelID(nChannelID)
, m_bIsDefaultChannel(bIsDefaultChannel)
{
}

ZSharedField::~ZSharedField()
{
}

int ZSharedField::GetPlayerLimit()
{
	if (m_pFieldInfo) return m_pFieldInfo->m_nCapacity;
	return CHANNEL_PLAYER_LIMIT;

}

bool ZSharedField::IsQuiet()
{
	float fPlayerRate = (float)GetPlayerCount() / (float)GetPlayerLimit() * 100.f;
	return (fPlayerRate < LIMIT_PLAYER_RATE_FOR_QUIET_CHANNEL);
}

bool ZSharedField::IsEnterable()
{
	return (GetPlayerCount() < GetPlayerLimit());
}
