#pragma once

struct PARTY_MEMBER_INFO
{	
	PARTY_MEMBER_INFO(const wstring& strName, int nCID)
	: m_strName(strName), m_nCID(nCID) {}

	int		m_nCID;
	wstring	m_strName;
};

typedef map<MUID, PARTY_MEMBER_INFO>	map_partymember;
typedef set<MUID>						set_partymember_offline;
typedef map_partymember::const_iterator	partymember_iterator;
