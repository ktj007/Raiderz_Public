#pragma once

class ZParty;
class ZPartyMember;
struct TD_PARTY;
struct TD_PARTY_MEMBER;
struct TD_PARTY_INFO;
struct TD_PARTY_FIELD;
struct TD_PARTY_QUEST;
struct TD_PARTY_MATCHING_PUBLIC_PARTY_LIST_ITEM;
struct PARTY_SETTING;
struct LOOTING_RULE_DATA;

class ZPartyCommandRouter
{
public:
	ZPartyCommandRouter();
	virtual ~ZPartyCommandRouter();

	// Unicast Command
	void InviteRes(MUID uidTarget, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult);
	void AcceptReq(MUID uidTarget, MUID uidTargetPlayer, MUID uidRequestPlayer, wstring strRequestPlayerName);
	void AcceptCancel(MUID uidTarget, MUID uidTargetPlayer, CCommandResultTable nResult);
	void LeaveRes(MUID uidTarget, MUID uidLeavePlayer, CCommandResultTable nResult);
	void KickRes(MUID uidTarget, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult);
	void JoinRes(MUID uidTarget, MUID uidRequestPlayer, CCommandResultTable nResult);
	void JoinAcceptReq(MUID uidTarget, MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, wstring strRequestPlayerName, int nReqPlayerLevel, int nReqPlayerTalentStyle, int nReqPlayerFieldID);
	void JoinAcceptCancel(MUID uidTaget, MUID uidLeader, CCommandResultTable nResult);
	void PartyInfoAllRes(MUID uidTarget, vector<TD_PARTY_INFO>& vecPartyInfo);	
	void ShowInfoRes(MUID uidTarget, MUID uidRequestor, const ZParty* pParty);
	void CreateSinglePartyRes(MUID uidTarget, MUID uidRequestPlayer, CCommandResultTable nResult);
	void ShowMatchingPublicPartyListRes(MUID uidTarget, MUID uidRequestor, CCommandResultTable nResult, const vector<TD_PARTY_MATCHING_PUBLIC_PARTY_LIST_ITEM>& vecMatchedItem, int nFilteredCount);
	void Fail(MUID uidTarget, MUID uidPlayer, CCommandResultTable nResult);

	// Broadcast Command
	void PartyAdd(MUID uidParty, MUID uidLeader, wstring strLeaderName, CID nLeaderCID);
	void PartyRemove(MUID uidParty);
	void AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, CID nMemberCID);		
	void RemoveMember(MUID uidParty, MUID uidMember);
	void PartySync(const ZParty* pParty);
	void MemberSync(MUID uidParty, const ZPartyMember* pPartyMember);
	void QuestSync(MUID uidParty, vector<TD_PARTY_QUEST>& vecPartyQuest);	
	void FieldSync(MUID uidParty, vector<TD_PARTY_FIELD>& vecPartyField);
	void QuestRunnerSync(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID);
	void EraseQuestRunnerSync(MUID uidParty, MUID uidMember);	
	void AddOfflineMember(MUID uidParty, MUID uidMember);
	void RemoveOfflineMember(MUID uidParty, MUID uidMember, MUID uidOffline);
	void MoveServerSync(MUID uidParty, MUID uidMember, MUID uidOffline);
	void ChangePublicPartySettingRes(MUID uidParty, bool bPublicParty, wstring strPartyName);
	void ChangeLeaderRes(MUID uidParty, MUID uidNewLeader);
	void ChangeLootingRuleRes(MUID uidParty, LOOTING_RULE_DATA rule);
	void ChangeQuestIDRes( MUID uidParty, int nQuestID);

private:
	void MakeTD_PartyInfo(const ZParty* pParty, TD_PARTY& outtdParty, vector<TD_PARTY_MEMBER>& outvecMember, vector<vector<int>>* outvecBuff);
};
