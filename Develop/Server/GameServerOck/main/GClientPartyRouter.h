#pragma once

class GEntityPlayer;
class GParty;

struct LOOTING_RULE_DATA;
enum CCommandResultTable;

struct PARTY_SETTING;

class GClientPartyRouter : public MTestMemPool<GClientPartyRouter>
{
public:
	GClientPartyRouter();
	virtual ~GClientPartyRouter();

	void AcceptCancel(const GEntityPlayer* pTarget, CCommandResultTable nResult);
	void JoinAcceptCancel(const GEntityPlayer* pTarget, CCommandResultTable nResult);
	void InviteQuestion(const GEntityPlayer* pTarget, MUID uidInviter, const wchar_t* szInviterName, bool isCombatInvitee);	
	void InviteRespond(const GEntityPlayer* pTarget, CCommandResultTable nResult);		
	void InviteForMeQuestion(const GEntityPlayer* pTarget, MUID uidInvitee, const wchar_t* szInviteeName, bool isCombatInviter);
	void NotifyJoin(const GParty* pTarget, MUID uidMember, const wchar_t* szMembersName);
	void PartySync(GParty* pTarget, minet::MCommand* pPartySyncCommand);
	void MemberSync(GParty* pTarget, minet::MCommand* pPartyMemberSyncCommand);
	void NotifyLeave(GParty* pTarget, MUID uidMember);	
	void NotifyLeave(const GEntityPlayer* pTarget, MUID uidMember);	
	void ChangePartyNameRes(const GParty* pParty, wstring strName);
	void ChangePartyLeaderRes(const GParty* pParty, MUID uidOldLeader, MUID uidNewLeader);
	void ChangePartyLootingRuleRes(const GParty* pParty, LOOTING_RULE_DATA rule);
	void ChangePartyQuestIDRes(const GParty* pParty, int nQuestID);
public:
	void RouteToAllMembers(const GParty* pTarget, minet::MCommand* pCommand, const MUID& uidExceptMember=MUID::ZERO);
	
};
