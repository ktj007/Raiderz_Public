#include "stdafx.h"
#include "GProxyParty.h"
#include "SDef.h"
#include "CSDef_Party.h"

GProxyParty::GProxyParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID)
: m_uidParty(uidParty)
, m_uidLeader(uidLeader)
{
	m_mapMembers.insert(make_pair(uidLeader, PARTY_MEMBER_INFO(strLeaderName, nLeaderCID)));
}

GProxyParty::~GProxyParty()
{
	// do nothing
}

void GProxyParty::AddMember(MUID uidMember, wstring strMemberName, int nMemberCID)
{
	m_mapMembers.insert(make_pair(uidMember, PARTY_MEMBER_INFO(strMemberName, nMemberCID)));
}

void GProxyParty::RemoveMember(MUID uidMember)
{
	m_mapMembers.erase(uidMember);
}

void GProxyParty::ChangeLeader(MUID uidLeader)
{	
	if (!IsExistMember(uidLeader))		return;
	
	m_uidLeader = uidLeader;
}

void GProxyParty::ChangeUID(MUID uidOld, MUID uidNew)
{	
	if (uidOld == uidNew)			return;
	if (IsExistMember(uidNew))		return;
	
	partymember_iterator it = m_mapMembers.find(uidOld);
	if (it == m_mapMembers.end())	return;

	m_mapMembers.insert(make_pair(uidNew, it->second));
	m_mapMembers.erase(it);

	if (m_uidLeader == uidOld)
	{
		m_uidLeader == uidNew;
	}
}

MUID GProxyParty::GetUID(void) const
{
	return m_uidParty;
}

MUID GProxyParty::GetLeader(void) const
{
	return m_uidLeader;
}

wstring GProxyParty::GetName(MUID uidMember) const
{
	partymember_iterator it = FindMember(uidMember);
	if (it == EndMember())		return wstring();

	return it->second.m_strName;
}

int GProxyParty::GetCID( MUID uidMember ) const
{
	partymember_iterator it = FindMember(uidMember);
	if (it == EndMember())		return 0;

	return it->second.m_nCID;
}

int GProxyParty::GetCount(void) const
{
	return m_mapMembers.size();
}

bool GProxyParty::IsFull(void) const
{
	return MAX_PARTY_MEMBER_COUNT <= m_mapMembers.size();
}

MUID GProxyParty::Find(wstring strName) const
{
	for each(map_partymember::value_type pairMember in m_mapMembers)
	{
		if (pairMember.second.m_strName == strName)
		{
			return pairMember.first;
		}
	}

	return MUID::Invalid();
}

MUID GProxyParty::Find( int nCID ) const
{
	for each(map_partymember::value_type pairMember in m_mapMembers)
	{
		if (pairMember.second.m_nCID == nCID)
		{
			return pairMember.first;
		}
	}

	return MUID::Invalid();
}

partymember_iterator GProxyParty::BeginMember(void) const
{
	return m_mapMembers.begin();
}

partymember_iterator GProxyParty::EndMember(void) const
{
	return m_mapMembers.end();
}

partymember_iterator GProxyParty::FindMember(MUID uidMember) const
{
	return m_mapMembers.find(uidMember);
}

bool GProxyParty::IsExistMember(MUID uidMember) const
{
	partymember_iterator it = FindMember(uidMember);
	return it != EndMember();
}

GProxyPartyManager::GProxyPartyManager()
{
	// do nothing
}

GProxyPartyManager::~GProxyPartyManager()
{
	Clear();
}

GProxyParty* GProxyPartyManager::AddParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID)
{
	GProxyPartyMap::iterator it = m_PartyMap.find(uidParty);
	if (it != m_PartyMap.end())
	{
		return it->second;
	}

	GProxyParty* pProxyParty = new GProxyParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
	m_PartyMap.insert(make_pair(uidParty, pProxyParty));

	return pProxyParty;
}

void GProxyPartyManager::RemoveParty(MUID uidParty)
{
	GProxyPartyMap::iterator it = m_PartyMap.find(uidParty);
	if (it == m_PartyMap.end())		return;

	GProxyParty* pProxyParty = it->second;
	pProxyParty->Drop();
	m_PartyMap.erase(it);
}

void GProxyPartyManager::AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID)
{
	GProxyPartyMap::iterator it = m_PartyMap.find(uidParty);
	if (it == m_PartyMap.end())		return;

	GProxyParty* pProxyParty = it->second;
	pProxyParty->AddMember(uidMember, strMemberName, nMemberCID);
}

void GProxyPartyManager::RemoveMember(MUID uidParty, MUID uidMember)
{
	GProxyPartyMap::iterator it = m_PartyMap.find(uidParty);
	if (it == m_PartyMap.end())					return;

	GProxyParty* pProxyParty = it->second;
	if (uidMember == pProxyParty->GetLeader())	return;	

	pProxyParty->RemoveMember(uidMember);
}

void GProxyPartyManager::ChangeLeader(MUID uidParty, MUID uidNewLeader)
{
	GProxyPartyMap::iterator it = m_PartyMap.find(uidParty);
	if (it == m_PartyMap.end())		return;

	GProxyParty* pProxyParty = it->second;
	pProxyParty->ChangeLeader(uidNewLeader);
}

void GProxyPartyManager::Clear(void)
{
	for each(GProxyPartyMap::value_type pairParty in m_PartyMap)
	{
		GProxyParty* pProxyParty = pairParty.second;
		pProxyParty->Drop();
	}

	m_PartyMap.clear();
}

GProxyParty* GProxyPartyManager::Find(MUID uidParty) const
{
	GProxyPartyMap::const_iterator it = m_PartyMap.find(uidParty);
	if (it == m_PartyMap.end())		return NULL;

	return it->second;
}

size_t GProxyPartyManager::GetCount(void) const
{
	return m_PartyMap.size();		
}
