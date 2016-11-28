#pragma once

class GEntityPlayer;
class GParty;

struct LOOTING_RULE_DATA;
enum CCommandResultTable;

struct PARTY_SETTING;

struct TD_PARTY;
struct TD_PARTY_MEMBER;
struct TD_PARTY_MATCHING_PUBLIC_PARTY_LIST_ITEM;

class GClientPartyRouter : public MTestMemPool<GClientPartyRouter>
{
public:
	GClientPartyRouter();
	virtual ~GClientPartyRouter();

	void AcceptCancel(const GEntityPlayer* pTarget, CCommandResultTable nResult);
	void JoinAcceptCancel(const GEntityPlayer* pTarget, CCommandResultTable nResult);
	void InviteQuestion(const GEntityPlayer* pTarget, MUID uidInviter, const wchar_t* szInviterName, bool isCombatInvitee);	
	void InviteRespond(const GEntityPlayer* pTarget, CCommandResultTable nResult);		
	void JoinRes(const GEntityPlayer* pTarget, CCommandResultTable nResult);
	void JoinQuestion(const GEntityPlayer* pTarget, MUID uidInvitee, const wchar_t* szInviteeName, int nInviteeLevel, int nInviteeTalentStyle, int nInviteeFieldID);
	void NotifyJoin(const GParty* pTarget, MUID uidMember, const wchar_t* szMembersName);
	void PartySync(GParty* pTarget, minet::MCommand* pPartySyncCommand);
	void MemberSync(GParty* pTarget, minet::MCommand* pPartyMemberSyncCommand);
	void NotifyLeave(GParty* pTarget, MUID uidMember);	
	void NotifyLeave(const GEntityPlayer* pTarget, MUID uidMember);	
	void ChangePublicPartySettingRes(const GParty* pParty, bool bPublicParty, wstring strPartyName);
	void ChangePartyLeaderRes(const GParty* pParty, MUID uidOldLeader, MUID uidNewLeader);
	void ChangePartyLootingRuleRes(const GParty* pParty, LOOTING_RULE_DATA rule);
	void ChangePartyQuestIDRes(const GParty* pParty, int nQuestID);
	void ShowInfoRes(GEntityPlayer* pTarget, const TD_PARTY& tdParty, const vector<TD_PARTY_MEMBER>& vecMembers);
	void CreateSinglePartyRes(GEntityPlayer* pTarget, CCommandResultTable nResult);
	void ShowMatchedPublicPartyListRes(GEntityPlayer* pTarget, CCommandResultTable nResult, const vector<TD_PARTY_MATCHING_PUBLIC_PARTY_LIST_ITEM>& vecMatchedItem, int nFilteredCount);
public:
	void RouteToAllMembers(const GParty* pTarget, minet::MCommand* pCommand, const MUID& uidExceptMember=MUID::ZERO);
	
};
