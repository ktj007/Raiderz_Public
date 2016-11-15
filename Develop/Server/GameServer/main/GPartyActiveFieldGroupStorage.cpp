#include "stdafx.h"
#include "GPartyActiveFieldGroupStorage.h"
#include "GDef.h"

MUID GPartyActiveFieldGroupStorage::Find(int nFieldGroupID) const
{
	MAP_FIELDGROUP::const_iterator it = m_mapFieldGroups.find(nFieldGroupID);
	if (it == m_mapFieldGroups.end())	return MUID::Invalid();

	return it->second;
}

void GPartyActiveFieldGroupStorage::AddField(int nFieldGroupID, MUID uidFieldGroup)
{
	VALID(nFieldGroupID != -1);
	m_mapFieldGroups.insert(make_pair(nFieldGroupID, uidFieldGroup));
}

void GPartyActiveFieldGroupStorage::RemoveField(int nFieldGroupID)
{
	VALID(nFieldGroupID != -1);
	m_mapFieldGroups.erase(nFieldGroupID);
}

bool GPartyActiveFieldGroupStorage::HasField(int nFieldGroupID) const
{
	MAP_FIELDGROUP::const_iterator it = m_mapFieldGroups.find(nFieldGroupID);
	return it != m_mapFieldGroups.end();
}

void GPartyActiveFieldGroupStorage::AddPolicy(MUID uidFieldGroup)
{
	m_setFieldPolicy.insert(uidFieldGroup);
}

void GPartyActiveFieldGroupStorage::RemovePolicy(MUID uidFieldGroup)
{
	m_setFieldPolicy.erase(uidFieldGroup);
}

const set<MUID>& GPartyActiveFieldGroupStorage::GetFieldPolicy(void) const
{
	return m_setFieldPolicy;
}

void GPartyActiveFieldGroupStorage::ClearField(void)
{
	m_mapFieldGroups.clear();
	m_setFieldPolicy.clear();
}
