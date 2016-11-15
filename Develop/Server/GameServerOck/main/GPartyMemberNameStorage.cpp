#include "stdafx.h"
#include "GPartyMemberNameStorage.h"

void GPartyMemberNameStorage::AddName(MUID uidMember, string strMemberName)
{
	m_mapName.insert(NAME_MAP::value_type(uidMember, strMemberName));
}

void GPartyMemberNameStorage::DeleteName(MUID uidMember)
{
	m_mapName.erase(uidMember);
}

string GPartyMemberNameStorage::FindName(MUID uidMember) const
{
	NAME_MAP::const_iterator it = m_mapName.find(uidMember);	
	if (it == m_mapName.end())		return string();

	return it->second;
}
