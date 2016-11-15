#include "stdafx.h"
#include "ZField.h"
#include "ZFieldInfo.h"

ZField::ZField(MUID uid, ZFieldInfo* pFieldInfo, int nGameServerID)
: m_UID(uid)
, m_pFieldInfo(pFieldInfo)
, m_nGameServerID(nGameServerID)
, m_nPlayerCount(0)
{
	_ASSERT(pFieldInfo);

}

ZField::~ZField()
{

}

ZFieldType ZField::GetType()
{
	if (m_pFieldInfo)
		if (m_pFieldInfo->m_bDynamic) return FT_DYNAMIC;

	return FT_SHARED;
}

void ZField::IncreasePlayerCount()
{
	VALID(GetPlayerLimit() > m_nPlayerCount);

	m_nPlayerCount++;
}
void ZField::DecreasePlayerCount()
{
	if (0 == m_nPlayerCount)
		return;

	m_nPlayerCount--;
}

int ZField::GetPlayerLimit()
{
	if (m_pFieldInfo) return m_pFieldInfo->m_nCapacity;
	return CHANNEL_PLAYER_LIMIT;
}
