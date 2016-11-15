#include "stdafx.h"
#include "GParty.h"
#include "GPartyActiveFieldGroupStorage.h"
#include "GPartyActiveQuestStorage.h"
#include "GPartyMemberStorage.h"
#include "GPartyLootingRuleStorage.h"
#include "GConst.h"
#include "SDef.h"
#include "CSDef_Loot.h"
#include "CSDef_Party.h"

GParty::GParty(MUID uidParty)
: m_UID(uidParty)
, m_pActiveQuestStorage(NULL)
, m_pActiveFieldGroupStorage(NULL)
, m_nAutoPartyQuestID(INVALID_ID)
{
	m_pActiveFieldGroupStorage = new GPartyActiveFieldGroupStorage();
	m_pActiveQuestStorage = new GPartyActiveQuestStorage();
	m_pMemberStorage = new GPartyMemberStorage(MAX_PARTY_MEMBER_COUNT);
	m_pLootingRuleStorage = new GPartyLootingRuleStorage();	
}

GParty::~GParty()
{
	SAFE_DELETE(m_pActiveQuestStorage);
	SAFE_DELETE(m_pActiveFieldGroupStorage);
	SAFE_DELETE(m_pMemberStorage);
	SAFE_DELETE(m_pLootingRuleStorage);
}



bool GParty::IsAutoParty()
{
	return m_nAutoPartyQuestID != INVALID_ID;
}

void GParty::AddQuest(int nQuestID)
{
	m_pActiveQuestStorage->AddQuest(nQuestID);
}

void GParty::AddQuest(int nQuestID, int nCount)
{
	m_pActiveQuestStorage->AddQuest(nQuestID, nCount);
}

void GParty::RemoveQuest(int nQuestID)
{
	m_pActiveQuestStorage->RemoveQuest(nQuestID);
}

bool GParty::HasQuest(int nQuestID) const
{
	return m_pActiveQuestStorage->HasQuest(nQuestID);
}

void GParty::ClearQuests(void)
{
	m_pActiveQuestStorage->ClearQuest();
}

void GParty::CreateField(int nFieldGroupID, MUID uidFieldGroup)
{
	if (m_pActiveFieldGroupStorage->HasField(nFieldGroupID))	return;	
	
	m_pActiveFieldGroupStorage->AddField(nFieldGroupID, uidFieldGroup);
	m_pActiveFieldGroupStorage->AddPolicy(uidFieldGroup);
}

void GParty::RemoveField(int nFieldGroupID)
{
	MUID uidFieldGroup = m_pActiveFieldGroupStorage->Find(nFieldGroupID);
	if (uidFieldGroup.IsInvalid())		return;

	m_pActiveFieldGroupStorage->RemoveField(nFieldGroupID);
	m_pActiveFieldGroupStorage->RemovePolicy(uidFieldGroup);
}

MUID GParty::FindField(int nFieldGroupID) const
{
	return m_pActiveFieldGroupStorage->Find(nFieldGroupID);
}

const set<MUID>& GParty::GetFieldPolicy(void) const
{
	return m_pActiveFieldGroupStorage->GetFieldPolicy();
}

void GParty::ClearField(void)
{
	m_pActiveFieldGroupStorage->ClearField();
}

bool GParty::AddMember(MUID uidMember, wstring strMembersName, int nMemberCID)
{
	return m_pMemberStorage->AddMember(uidMember, strMembersName, nMemberCID);
}

void GParty::RemoveMember(MUID uidMember)
{
	m_pMemberStorage->RemoveMember(uidMember);
}

wstring GParty::FindName(MUID uidMember) const
{
	return m_pMemberStorage->FindName(uidMember);
}

bool GParty::IsFull(void) const
{
	return m_pMemberStorage->IsFull();
}

bool GParty::IsEmpty(void) const
{
	return m_pMemberStorage->IsEmpty();
}

bool GParty::IsExistMember(MUID uidMember) const
{
	return m_pMemberStorage->IsExistMember(uidMember);
}

bool GParty::IsLeader( MUID uidLeader ) const
{
	return m_pMemberStorage->IsLeader(uidLeader);
}

int GParty::GetMemberCount(void) const
{
	return m_pMemberStorage->GetMemberCount();
}

bool GParty::SetLeader(MUID uidNewLeader)
{
	return m_pMemberStorage->SetLeader(uidNewLeader);
}

MUID GParty::GetLeader(void) const
{
	return m_pMemberStorage->GetLeader();
}

int GParty::GetLeaderCID( void ) const
{
	return m_pMemberStorage->GetLeaderCID();
}

MUID GParty::FindCandidateForLeader(void) const
{
	return m_pMemberStorage->FindCandidateForLeader();
}

MUID GParty::FindOfflineMember(wstring strMembersName) const
{
	return m_pMemberStorage->FindOfflineMember(strMembersName);
}

int GParty::GetOnlineMemberCount(void) const
{
	return m_pMemberStorage->GetOnlineMemberCount();
}

MUID GParty::GetAloneMember(void) const
{
	return m_pMemberStorage->GetAloneMember();
}

partymember_iterator GParty::GetMemberBegin(void) const
{
	return m_pMemberStorage->GetMemberBegin();
}

partymember_iterator GParty::GetMemberEnd(void) const
{
	return m_pMemberStorage->GetMemberEnd();
}

partymember_iterator GParty::FindMember(MUID uidMember) const
{
	return m_pMemberStorage->FindMember(uidMember);
}

void GParty::AddOfflineMember(MUID uidMember)
{
	m_pMemberStorage->AddOfflineMember(uidMember);
}

void GParty::RemoveOfflineMember(MUID uidMember)
{
	m_pMemberStorage->RemoveOfflineMember(uidMember);
}

int GParty::GetOfflineMemberCount(void) const
{
	return m_pMemberStorage->GetOfflineMemberCount();
}

void GParty::ChangeMemberUID(MUID uidOldMember, MUID uidNewMember)
{
	m_pMemberStorage->ChangeMemberUID(uidOldMember, uidNewMember);
}

PARTY_SETTING GParty::GetPartySetting() const
{
	LOOTING_RULE_DATA lootingRuleData = m_pLootingRuleStorage->GetLootingRuleData();
	PARTY_SETTING partySetting(m_pMemberStorage->GetLeader(), lootingRuleData, m_strName);
	partySetting.m_nAutoPartyQuestID = m_nAutoPartyQuestID;

	return partySetting;
}

int GParty::GetRoundRobinLastOrderCID()
{
	return m_pLootingRuleStorage->GetRoundRobinLastOrderCID();
}

LOOTING_RULE_DATA GParty::GetLootingRuleData() const
{
	return m_pLootingRuleStorage->GetLootingRuleData();
}

void GParty::SetPartySetting( const PARTY_SETTING& partySetting )
{
	m_pMemberStorage->SetLeader(partySetting.m_uidLeader);	
	m_pLootingRuleStorage->SetLootingRuleData(partySetting.m_lootingRuleData);	
	m_strName = partySetting.m_szName;	
	m_nAutoPartyQuestID = partySetting.m_nAutoPartyQuestID;
}

void GParty::SetRoundRobinLastOrderCID(int nLastOrderCID)
{
	m_pLootingRuleStorage->SetRoundRobinLastOrderCID(nLastOrderCID);
}

void GParty::SetLootingRuleData(const LOOTING_RULE_DATA& lootingRuleData)
{
	m_pLootingRuleStorage->SetLootingRuleData(lootingRuleData);
}

const wstring& GParty::GetName()
{
	return m_strName;
}

void GParty::SetName(wstring strName)
{
	m_strName = strName;
}

MUID GParty::FindMemberUID( int nCID )
{
	return m_pMemberStorage->FindMemberUID(nCID);
}

vector<int> GParty::CollectMemberCID()
{
	return m_pMemberStorage->CollectMemberCID();
}

void GParty::SetAutoPartyQuestID( QuestID nQuestID )
{
	m_nAutoPartyQuestID = nQuestID;
}


