#pragma once

class ZPartyMember;
class ZPartyCommandRouter;
class ZParty;
class ZFixedPartyLogic;
struct TD_PARTY_FIELD;
struct TD_PARTY_QUEST;
struct PARTY_SETTING;
struct LOOTING_RULE_DATA;

class ZPartyLogic
{
public:
	ZPartyLogic();
	virtual ~ZPartyLogic();

	virtual ZPartyCommandRouter* NewRouter(void);
	
	virtual void InviteReq(MUID uidSender, MUID uidTargetPlayer, MUID uidRequestPlayer);
	virtual void AcceptRes(MUID uidSender, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult);
	virtual void LeaveReq(MUID uidSender, MUID uidParty, MUID uidLeaveMember);
	virtual void KickReq(MUID uidSender, MUID uidParty, MUID uidRequestPlayer, MUID uidTargetPlayer);
	virtual void JoinInviteReq(MUID uidSender, MUID uidParty, MUID uidRequestPlayer);
	virtual void JoinAcceptRes(MUID uidSender, MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult);
	virtual void CreateSingleReq(MUID uidSender, MUID uidRequestPlayer);
	virtual void PartyInfoAllReq(MUID uidSender);
	virtual void AllMemberOfflineInGameServer(int nGameServerID);
		 	
	virtual void UpdateParty(MUID uidParty, const ZPartyMember* pMember);
	virtual void UpdateMember(MUID uidParty, const ZPartyMember* pMember);	
	virtual void AddQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID);
	virtual void AddField(MUID uidParty, MUID uidMember, MUID uidFieldGroup);
	virtual void UpdateQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID);
	virtual void RemoveQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID);
	virtual void RemoveField(MUID uidParty, MUID uidMember, int nFieldGroupID);
	virtual void RemoveFieldSelf(MUID uidParty, int nFieldGroupID);
	virtual void EraseQuestRunner(MUID uidParty, MUID uidMember);
	virtual void DoOffline(MUID uidParty, MUID uidMember);
	virtual bool DoOnline(MUID uidParty, MUID uidMember, MUID uidOffline);
	virtual void MoveServer(MUID uidParty, MUID uidMember);	

	virtual void ChangeNameReq(MUID uidSender, MUID uidParty, MUID uidLeader, wstring strName);
	virtual void ChangeLeaderReq(MUID uidSender, MUID uidParty, MUID uidLeader, MUID uidNewLeader);
	virtual void ChangeLootingRuleReq(MUID uidSender, MUID uidParty, MUID uidLeader, LOOTING_RULE_DATA rule);
	virtual void ChangeQuestIDReq( MUID uidSender, MUID uidParty, MUID uidLeader, int nQuestID );

	bool FixedPartyLogOn(MUID uidParty, MUID uidMember, MUID uidOffline);
	void CreateAutoPartyReq(int nQuestID, const vector<MUID>& vecMemberUID);
	void JoinAotuPartyReq(MUID uidParty, MUID uidPlayer);

private:	
	void MakePartyField(const ZParty* pParty, vector<TD_PARTY_FIELD>& vecPartyField);
	void MakePartyQuest(const ZParty* pParty, vector<TD_PARTY_QUEST>& vecPartyQuest);
	void MemberQuestCounting(const ZPartyMember* pPartyMember, map<int, int>& mapQuestID);
	void RouteRemoveMemberSync(const ZParty* pParty);
private:
	ZPartyCommandRouter* m_pRouter;
	ZFixedPartyLogic* m_pFixedPartyLogic;
};
