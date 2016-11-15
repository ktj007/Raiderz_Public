#pragma once

#include "GServerPartyRouter.h"

class GParty;

class GStandAloneModePartyRouter: public GServerPartyRouter
{
public:
	GStandAloneModePartyRouter();
	virtual ~GStandAloneModePartyRouter();

	virtual void InviteReq(MUID uidTargetPlayer, MUID uidRequestPlayer) override;
	virtual void AcceptRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult) override;
	virtual void UpdateParty(MUID uidParty, const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff) override;
	virtual void UpdateMember(MUID uidParty, const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff) override;
	virtual void AddQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID) override;
	virtual void AddField(MUID uidParty, MUID uidMember, MUID uidFieldGroup) override;
	virtual void UpdateQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID) override;
	virtual void RemoveQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID) override;
	virtual void RemoveField(MUID uidParty, MUID uidMember, int nFieldGroupID) override;	
	virtual void RemoveFieldSelf(MUID uidParty, int nFieldGroupID) override;
	virtual void EraseQuestRunner(MUID uidParty, MUID uidMember) override;
	virtual void LeaveReq(MUID uidParty, MUID uidLeavePlayer) override;
	virtual void KickReq(MUID uidParty, MUID uidRequestPlyaer, MUID uidTargetPlayer) override;	
	virtual void DoOffline(MUID uidParty, MUID uidMember) override;
	virtual void DoOnline(MUID uidParty, MUID uidMember, MUID uidOffline) override;
	virtual void JoinInviteReq(MUID uidParty, MUID uidRequestPlayer) override;
	virtual void JoinAcceptRes(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult) override;
	virtual void MoveServer(MUID uidParty, MUID uidMember) override;
	virtual void CreateSinglePartyReq(MUID uidRequestPlayer) override;
	virtual void PartyInfoAllReq(void) override;
	virtual void ChangePartyNameReq(MUID uidParty, MUID uidLeader, wstring strName) override;
	virtual void ChangePartyLeaderReq(MUID uidParty, MUID uidLeader, MUID uidNewLeader) override;
	virtual void ChangePartyLootingRuleReq(MUID uidParty, MUID uidLeader, LOOTING_RULE_DATA rule) override;
	virtual void ChangeQuestIDReq(MUID uidParty, MUID uidLeader, int nQuestID) override;
	virtual void FixedPartyLogOn(MUID uidParty, MUID uidMember, MUID uidOffline) override;
	virtual void CreateAutoPartyReq(QuestID nQuestID, const vector<MUID> vecPromotersUID) override;
	virtual void JoinAutoPartyReq(MUID uidParty, MUID uidPlayer) override;

private:
	void SyncParty(const GParty* pParty);
	
};
