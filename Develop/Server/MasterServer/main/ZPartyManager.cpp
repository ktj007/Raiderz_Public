#include "stdafx.h"
#include "ZPartyManager.h"
#include "ZParty.h"

ZPartyManager::ZPartyManager()
{
	// do nothing
}

ZPartyManager::~ZPartyManager()
{
	Clear();
}

ZParty* ZPartyManager::AddParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderServerID, int nLeaderCID)
{
	ZPartyMap::iterator it = m_mapParty.find(uidParty);
	if (it != m_mapParty.end())
	{
		return it->second;
	}

	ZParty* pParty = new ZParty(uidParty, uidLeader, strLeaderName, nLeaderServerID, nLeaderCID);
	m_mapParty.insert(make_pair(uidParty, pParty));

	return pParty;
}

ZParty* ZPartyManager::AddParty(MUID uidLeader, wstring strLeaderName, int nLeaderServerID, int nLeaderCID)
{
	MUID uidParty = m_UIDGenerator.Generate();

	ZParty* pParty = new ZParty(uidParty, uidLeader, strLeaderName, nLeaderServerID, nLeaderCID);
	m_mapParty.insert(make_pair(uidParty, pParty));

	return pParty;
}

void ZPartyManager::RemoveParty(MUID uidParty)
{
	ZPartyMap::iterator it = m_mapParty.find(uidParty);
	if (it == m_mapParty.end())		return;

	ZParty* pParty = it->second;
	pParty->Drop();
	m_mapParty.erase(uidParty);
}

void ZPartyManager::AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberServerID, int nMemberCID)
{
	ZPartyMap::iterator it = m_mapParty.find(uidParty);
	if (it == m_mapParty.end())		return;

	ZParty* pParty = it->second;
	pParty->AddMember(uidMember, strMemberName, nMemberServerID, nMemberCID);
}

void ZPartyManager::RemoveMember(MUID uidParty, MUID uidMember)
{
	ZPartyMap::iterator it = m_mapParty.find(uidParty);
	if (it == m_mapParty.end())		return;

	ZParty* pParty = it->second;
	pParty->RemoveMember(uidMember);
}

void ZPartyManager::ChangeLeader(MUID uidParty, MUID uidNewLeader)
{
	ZPartyMap::iterator it = m_mapParty.find(uidParty);
	if (it == m_mapParty.end())		return;

	ZParty* pParty = it->second;
	pParty->ChangeLeader(uidNewLeader);
}

int ZPartyManager::GetCount(void) const
{
	return m_mapParty.size();
}

ZParty* ZPartyManager::Find(MUID uidParty)
{
	ZPartyMap::iterator it = m_mapParty.find(uidParty);
	if (it == m_mapParty.end())		return NULL;

	return it->second;
}

void ZPartyManager::Clear(void)
{
	for each(ZPartyMap::value_type pairParty in m_mapParty)
	{
		ZParty* pParty = pairParty.second;
		pParty->Drop();
	}

	m_mapParty.clear();
}

bool ZPartyManager::IsPartyMember(MUID uidParty, MUID uidPlayer)
{
	ZParty* pParty = Find(uidParty);
	if (NULL == pParty) return false;
	return pParty->IsExistMember(uidPlayer);
}

MUID ZPartyManager::GetFieldGroupUID(MUID uidParty, int nFieldGroupID)
{
	ZParty* pParty = Find(uidParty);
	if (NULL == pParty) return false;
	return pParty->GetFieldGroupUID(nFieldGroupID);
}

const ZPartyManager::ZPartyMap& ZPartyManager::GetPartyMap(void) const
{
	return m_mapParty;
}
