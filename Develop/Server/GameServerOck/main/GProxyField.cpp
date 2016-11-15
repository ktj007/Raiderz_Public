#include "stdafx.h"
#include "GProxyField.h"
#include "GFieldInfo.h"
#include "GConst.h"

GProxyField::GProxyField(MUID uidField, GFieldInfo* pFieldInfo, int nGameServerID)
: MReferenceCounted()
, m_UID(uidField)
, m_pFieldInfo(pFieldInfo)
, m_nGameServerID(nGameServerID)
, m_nPlayerCount(0)
, m_bDynamic(false)
, m_nChannelID(INVALID_CHANNELID)
, m_uidFieldGroup(MUID::ZERO)
{
}

GProxyField::~GProxyField()
{
}

int GProxyField::GetCapacity() const
{	
	if (m_pFieldInfo) return m_pFieldInfo->m_nCapacity;
	return CHANNEL_PLAYER_LIMIT;
}

void GProxyField::SetPlayerCount(int nCount)
{
	RVALID(GetCapacity() >= nCount);

	m_nPlayerCount = nCount;
}

bool GProxyField::IsEnterable()
{
	if (1 > m_nChannelID)
		return false;
	if (GetPlayerCount() >= GetCapacity())
		return false;

	return true;
}