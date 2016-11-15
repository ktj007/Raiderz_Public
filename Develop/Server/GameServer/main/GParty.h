#pragma once

#include "GPartyDef.h"

enum LOOT_RULE_NORMALRANK_ITEM
{
	LRNI_NONE = 0,
	LRNI_FREE,
	LRNI_ORDER,
	LRNI_CHARGE,
	LRNI_RANDOM,
	LRNI_MAX
};

enum LOOT_RULE_HIGHRANK_ITEM
{
	LRHI_NONE = 0,
};

class GPartyActiveQuestStorage;
class GPartyActiveFieldGroupStorage;
class GPartyMemberStorage;
class GPartyLootingRuleStorage;

enum LOOTING_RULE_COMMON;
enum LOOTING_RULE_RARE;
enum LOOTING_RULE_RARE_FOR;

struct LOOTING_RULE_DATA;
struct PARTY_SETTING;

class GParty : public MTestMemPool<GParty>
{
public:

public:
	GParty(MUID uidParty);
	virtual ~GParty();

	// AutoParty
	void SetAutoPartyQuestID(QuestID nQuestID);
	bool IsAutoParty();

	// Quest
	void AddQuest(int nQuestID);
	void AddQuest(int nQuestID, int nCount);
	void RemoveQuest(int nQuestID);
	bool HasQuest(int nQuestID) const;
	void ClearQuests(void);

	// Field
	void CreateField(int nFieldGroupID, MUID uidFieldGroup);	
	void RemoveField(int nFieldGroupID);
	MUID FindField(int nFieldGroupID) const;	
	const set<MUID>& GetFieldPolicy(void) const;
	void ClearField(void);

	// Member
	bool AddMember(MUID uidMember, wstring strMembersName, int nMemberCID);
	void RemoveMember(MUID uidMember);
	wstring FindName(MUID uidMember) const;
	bool IsFull(void) const;
	bool IsEmpty(void) const;
	bool IsExistMember(MUID uidMember) const;
	bool IsLeader(MUID uidLeader) const;
	int GetMemberCount(void) const;
	bool SetLeader(MUID uidNewLeader);
	MUID GetLeader(void) const;
	int GetLeaderCID(void) const;
	MUID FindCandidateForLeader(void) const;
	MUID FindOfflineMember(wstring strMembersName) const;

	int GetOnlineMemberCount(void) const;
	MUID GetAloneMember(void) const;

	partymember_iterator GetMemberBegin(void) const;
	partymember_iterator GetMemberEnd(void) const;
	partymember_iterator FindMember(MUID uidMember) const;

	// Looting Rule
	PARTY_SETTING GetPartySetting() const;
	int GetRoundRobinLastOrderCID();
	LOOTING_RULE_DATA GetLootingRuleData() const;

	void SetPartySetting(const PARTY_SETTING& partySetting);	
	void SetRoundRobinLastOrderCID(int nLastOrderCID);
	void SetLootingRuleData(const LOOTING_RULE_DATA& lootingRuleData);
	const wstring& GetName();
	void SetName(wstring strName);

	// Off-line Member
	void AddOfflineMember(MUID uidMember);
	void RemoveOfflineMember(MUID uidMember);
	int GetOfflineMemberCount(void) const;

	void ChangeMemberUID(MUID uidOldMember, MUID uidNewMember);

	MUID FindMemberUID(int nCID);
	vector<int> CollectMemberCID();

public:
	MUID 									GetUID(void) const					{ return m_UID;	}	

private:
	MUID									m_UID;
	GPartyActiveQuestStorage*				m_pActiveQuestStorage;
	GPartyActiveFieldGroupStorage*			m_pActiveFieldGroupStorage;
	GPartyMemberStorage*					m_pMemberStorage;
	GPartyLootingRuleStorage*				m_pLootingRuleStorage;
	wstring									m_strName;
	QuestID									m_nAutoPartyQuestID;
};
