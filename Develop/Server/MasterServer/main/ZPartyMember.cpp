#include "stdafx.h"
#include "ZPartyMember.h"
#include "CTransData.h"

ZPartyMember::ZPartyMember()
: CSPartyMember()
, m_nGameServerID(0)
, m_pairFieldGroup(-1, MUID::Invalid())
, m_isMoveServer(false)
, m_isOffline(false)
, m_nCID(0)
{
	// do nothing
}

ZPartyMember::ZPartyMember(MUID uidMember, wstring strMemberName, int nMemberServerID, int MemberCID)
: CSPartyMember(uidMember, strMemberName)
, m_nGameServerID(nMemberServerID)
, m_pairFieldGroup(-1, MUID::Invalid())
, m_isMoveServer(false)
, m_isOffline(false)
, m_nCID(MemberCID)
{
	// do noting
}

ZPartyMember::~ZPartyMember()
{
	// do nothing
}

int ZPartyMember::GetGameServerID(void) const
{
	return m_nGameServerID;
}

void ZPartyMember::SetGameServerID(int id)
{
	m_nGameServerID = id;
}

bool ZPartyMember::IsMoveServer(void) const
{
	return m_isMoveServer;
}

void ZPartyMember::SetMoveServer(bool isMoveServer)
{
	m_isMoveServer = isMoveServer;
}

bool ZPartyMember::IsOffline(void) const
{
	return m_isOffline;
}

void ZPartyMember::SetOffline(bool isOffline)
{
	m_isOffline = isOffline;

	if (m_isOffline)
	{
		DoOffline();
	}
}

int ZPartyMember::GetFieldGroupID(void) const
{
	return m_pairFieldGroup.first;
}

MUID ZPartyMember::GetFieldGroupUID(void) const
{
	return m_pairFieldGroup.second;
}

void ZPartyMember::SetFieldGroup(int nFieldGroupID, MUID uidFieldGroup)
{
	m_pairFieldGroup.first = nFieldGroupID;
	m_pairFieldGroup.second = uidFieldGroup;
}

void ZPartyMember::ClearFieldGroup(void)
{
	m_pairFieldGroup.first = -1;
	m_pairFieldGroup.second = MUID::Invalid();
}

bool ZPartyMember::IsEmptyFieldGroup(void) const
{
	return m_pairFieldGroup.first == -1;
}

void ZPartyMember::AddQuest(int nQuestID)
{
	m_setQuestID.insert(nQuestID);	
}

void ZPartyMember::RemoveQuest(int nQuestID)
{
	m_setQuestID.erase(nQuestID);
}

void ZPartyMember::ClearQuest(void)
{
	m_setQuestID.clear();
}

bool ZPartyMember::IsEmptyQuest(void) const
{
	return m_setQuestID.empty();
}

ZPartyMember::quest_iterator ZPartyMember::BeginQuest(void) const
{
	return m_setQuestID.begin();
}

ZPartyMember::quest_iterator ZPartyMember::EndQuest(void) const
{
	return m_setQuestID.end();
}

ZPartyMember::quest_iterator ZPartyMember::FindQuest(int nQuestID) const
{
	return m_setQuestID.find(nQuestID);
}

bool ZPartyMember::IsExistQuest(int nQuestID) const
{
	quest_iterator it = m_setQuestID.find(nQuestID);
	return it != m_setQuestID.end();
}

int ZPartyMember::GetCID( void ) const
{
	return m_nCID;
}
