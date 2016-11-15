#pragma once

class GParty;
struct TD_PARTY_MEMBER;
struct PARTY_SETTING;
struct LOOTING_RULE_DATA;
enum CCommandResultTable;

class GServerPartyRouter
{
public:
	void MakePartyMemberInfo(const GParty* pParty, MUID uidMember, TD_PARTY_MEMBER* pPartyMember, vector<int>* pVecBuff);

public:
	virtual void InviteReq(MUID uidTargetPlayer, MUID uidRequestPlayer) = 0;
	virtual void AcceptRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult) = 0;
	virtual void UpdateParty(MUID uidParty, const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff) = 0;
	virtual void UpdateMember(MUID uidParty, const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff) = 0;
	virtual void AddQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID) = 0;
	virtual void AddField(MUID uidParty, MUID uidMember, MUID uidFieldGroup) = 0;
	virtual void UpdateQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID) = 0;
	virtual void RemoveQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID) = 0;
	virtual void RemoveField(MUID uidParty, MUID uidMember, int nFieldGroupID) = 0;	
	virtual void RemoveFieldSelf(MUID uidParty, int nFieldGroupID) = 0;
	virtual void EraseQuestRunner(MUID uidParty, MUID uidMember) = 0;	
	virtual void LeaveReq(MUID uidParty, MUID uidLeavePlayer) = 0;
	virtual void KickReq(MUID uidParty, MUID uidRequestPlyaer, MUID uidTargetPlayer) = 0;	
	virtual void DoOffline(MUID uidParty, MUID uidMember) = 0;
	virtual void DoOnline(MUID uidParty, MUID uidMember, MUID uidOffline) = 0;
	virtual void JoinInviteReq(MUID uidParty, MUID uidRequestPlayer) = 0;	
	virtual void JoinAcceptRes(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult) = 0;
	virtual void MoveServer(MUID uidParty, MUID uidMember) = 0;
	virtual void CreateSinglePartyReq(MUID uidRequestPlayer) = 0;
	virtual void PartyInfoAllReq(void) = 0;
	virtual void ChangePartyNameReq(MUID uidParty, MUID uidLeader, wstring strName) = 0;
	virtual void ChangePartyLeaderReq(MUID uidParty, MUID uidLeader, MUID uidNewLeader) = 0;
	virtual void ChangePartyLootingRuleReq(MUID uidParty, MUID uidLeader, LOOTING_RULE_DATA rule) = 0;
	virtual void ChangeQuestIDReq(MUID uidParty, MUID uidLeader, int nQuestID) = 0;
	virtual void FixedPartyLogOn(MUID uidParty, MUID uidMember, MUID uidOffline) = 0;
	virtual void CreateAutoPartyReq(QuestID nQuestID, const vector<MUID> vecPromotersUID) = 0;
	virtual void JoinAutoPartyReq(MUID uidParty, MUID uidPlayer) = 0;
};
