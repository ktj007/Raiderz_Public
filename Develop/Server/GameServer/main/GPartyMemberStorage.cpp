#include "stdafx.h"
#include "GPartyMemberStorage.h"
#include "GEntityPlayer.h"

GPartyMemberStorage::GPartyMemberStorage(int nLimitMemberCount)
: m_nLimitMemberCount(nLimitMemberCount)
, m_uidLeader(MUID::Invalid())
{
	_ASSERT(0 <= m_nLimitMemberCount);
}

bool GPartyMemberStorage::AddMember(MUID uidMember, wstring strMembersName, int nMemberCID)
{
	if (IsFull())					return false;
	if (IsExistMember(uidMember))	return false;
	
	m_mapMember.insert(make_pair(uidMember, PARTY_MEMBER_INFO(strMembersName, nMemberCID)));
	return true;
}

void GPartyMemberStorage::RemoveMember(MUID uidMember)
{
	m_mapMember.erase(uidMember);
	m_setOfflineMember.erase(uidMember);
}

bool GPartyMemberStorage::IsEmpty(void) const
{
	return m_setOfflineMember.size() == m_mapMember.size();
}

bool GPartyMemberStorage::IsFull(void) const
{
	return m_nLimitMemberCount <= (int)m_mapMember.size();
}

bool GPartyMemberStorage::IsExistMember(MUID uidMember) const
{
	partymember_iterator it = m_mapMember.find(uidMember);
	if (it == m_mapMember.end())	return false;

	return true;
}

bool GPartyMemberStorage::IsLeader( MUID uidLeader ) const
{
	return m_uidLeader == uidLeader;
}

int GPartyMemberStorage::GetMemberCount(void) const
{
	return m_mapMember.size();
}

int GPartyMemberStorage::GetOnlineMemberCount(void) const
{
	_ASSERT(m_setOfflineMember.size() <= m_mapMember.size());
	return m_mapMember.size() - m_setOfflineMember.size();
}

MUID GPartyMemberStorage::GetAloneMember(void) const
{
	if (1 != GetOnlineMemberCount())	return MUID::Invalid();
	
	for each(const map_partymember::value_type& member in m_mapMember)
	{
		MUID uidMember = member.first;

		set<MUID>::const_iterator it = m_setOfflineMember.find(uidMember);
		if (it != m_setOfflineMember.end())	continue;

		return uidMember;
	}

	return MUID::Invalid();
}

partymember_iterator GPartyMemberStorage::GetMemberBegin(void) const
{
	return m_mapMember.begin();
}

partymember_iterator GPartyMemberStorage::GetMemberEnd(void) const
{
	return m_mapMember.end();
}

partymember_iterator GPartyMemberStorage::FindMember(MUID uidMember) const
{
	return m_mapMember.find(uidMember);
}

bool GPartyMemberStorage::SetLeader(MUID uidNewLeader)
{
	// 멤버가 아니면 리더가 될 수 없다
	if (!IsExistMember(uidNewLeader))	return false;


	// 온라인 상태여야한다
	set<MUID>::iterator itOff = m_setOfflineMember.find(uidNewLeader);
	if (itOff != m_setOfflineMember.end())	return false;

	m_uidLeader = uidNewLeader;

	return true;
}

MUID GPartyMemberStorage::GetLeader(void) const
{	
	return m_uidLeader;
}

int GPartyMemberStorage::GetLeaderCID() const
{
	partymember_iterator itor = m_mapMember.find(m_uidLeader);
	if (m_mapMember.end() == itor) return 0;

	return itor->second.m_nCID;
}

MUID GPartyMemberStorage::FindCandidateForLeader(void) const
{
	if (m_uidLeader.IsInvalid())	return MUID::Invalid();		// 리더없는 파티는 작업하지 않는다.
	if (m_mapMember.size() <= 1)	return MUID::Invalid();		// 리더만 있는 파티는 후보가 없다.

	for each(const map_partymember::value_type& member in m_mapMember)
	{
		MUID uidMember = member.first;

		if (uidMember == m_uidLeader)			continue;
		
		set<MUID>::const_iterator it = m_setOfflineMember.find(uidMember);
		if (it != m_setOfflineMember.end())		continue;

		return uidMember;
	}

	return MUID::Invalid();
}

wstring GPartyMemberStorage::FindName(MUID uidMember) const
{
	partymember_iterator it = m_mapMember.find(uidMember);
	if (it == m_mapMember.end())	return wstring();

	return it->second.m_strName;
}

void GPartyMemberStorage::AddOfflineMember(MUID uidMember)
{
	if (!IsExistMember(uidMember))		return;

	m_setOfflineMember.insert(uidMember);
}

void GPartyMemberStorage::RemoveOfflineMember(MUID uidMember)
{
	m_setOfflineMember.erase(uidMember);
}

MUID GPartyMemberStorage::FindOfflineMember(wstring strName) const
{	
	for each (const MUID& uidOfflineMember in m_setOfflineMember)
	{
		if (strName == FindName(uidOfflineMember))
		{
			return uidOfflineMember;
		}
	}

	return MUID::Invalid();
}

int GPartyMemberStorage::GetOfflineMemberCount(void) const
{
	return m_setOfflineMember.size();
}

bool GPartyMemberStorage::ChangeMemberUID(MUID uidOldMember, MUID uidNewMember)
{
	if (uidOldMember == uidNewMember) return true;
	if (IsExistMember(uidNewMember)) return false;

	partymember_iterator it = m_mapMember.find(uidOldMember);
	if (it == m_mapMember.end())	return false;

	// 정보 교체
	m_mapMember.insert(make_pair(uidNewMember, it->second));
	m_mapMember.erase(it);

	// 오프라인 정보 교체
	set<MUID>::iterator itOffline = m_setOfflineMember.find(uidOldMember);
	if (itOffline != m_setOfflineMember.end())
	{
		m_setOfflineMember.insert(uidNewMember);
		m_setOfflineMember.erase(itOffline);		
	}


	// 파티장 변경
	if (m_uidLeader == uidOldMember)
	{
		m_uidLeader = uidNewMember;
	}
	
	return true;
}

MUID GPartyMemberStorage::FindMemberUID(int nCID)
{
	for (map_partymember::iterator itor=m_mapMember.begin(); itor!=m_mapMember.end(); itor++)
	{
		if (nCID == itor->second.m_nCID)
		{
			return itor->first;
		}
	}

	return MUID::ZERO;
}

vector<int> GPartyMemberStorage::CollectMemberCID()
{
	vector<int> vecMemberCID;

	for (map_partymember::iterator itor=m_mapMember.begin(); itor!=m_mapMember.end(); itor++)
	{
		vecMemberCID.push_back(itor->second.m_nCID);
	}

	return vecMemberCID;
}