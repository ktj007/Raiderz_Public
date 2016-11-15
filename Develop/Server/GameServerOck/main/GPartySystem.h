#pragma once

#include "CTransData.h"
#include "GParty.h"

enum PARTY_RESPOND
{
	PRS_NONE = -1,
	PRS_REFUSE =0,
	PRS_OK = 1,
};


using namespace minet;

class GPartyManager;
class GPartyMatcher;
class GEntityPlayer;
class GClientPartyRouter;
class GServerPartyRouter;

enum LOOTING_RULE_COMMON;
enum LOOTING_RULE_RARE;
enum LOOTING_RULE_RARE_FOR;
enum CCommandResultTable;

struct PARTY_SETTING;

class GPartySystem : public MTestMemPool<GPartySystem>
{
public:
	GPartySystem();	
	virtual ~GPartySystem();

	virtual GPartyManager*		NewPartyManager(void);
	virtual GClientPartyRouter* NewClientRouter(void);
	GPartyManager*				GetPartyManager(void) const;
	
	GParty*						FindParty(MUID uidParty) const;
	bool						IsPartyMember(GEntityPlayer* pPlayer) const;	
	bool						IsSamePartyMember(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2) const;
	bool						IsPartyLeader(GEntityPlayer* pPlayer) const;
	int							CalculateOnlineMemberCount(MUID uidParty) const;
	vector<GEntityPlayer*>		CollectNeighborMember(GEntityPlayer* pPlayer, float fDistance) const;
	vector<MUID>				CollectMemberUID(GEntityPlayer* pPlayer) const;
	vector<int>					CollectNeighborMemberCID(GEntityPlayer* pPlayer, float fDistance=FLT_MAX) const;

public:
	virtual void 				RouteToAllMembers(MUID uidParty, MCommand* pCommand, const MUID& uidExceptMember=MUID::ZERO);	
	void 						UpdateMember(GEntityPlayer* pPlayer);
	void						UpdateQuestRunner(MUID uidParty, GEntityPlayer* pMember) const;
	void						AddField(MUID uidParty, GEntityPlayer* pMember) const;
	void						AddQuest(MUID uidParty, MUID uidMember, int nQuestID) const;
	void						RemoveQuest(MUID uidParty, MUID uidMember, int nQuestID) const;
	MUID						FindField(MUID uidParty, int nFieldGroupID) const;
	void						RemoveFieldSelf(MUID uidParty, int nFieldGroupID) const;

public:		// C-G
	virtual void				CreateAutoPartyReq(QuestID nQuestID, const vector<MUID>& uidPromoters);
	virtual void				JoinAutoPartyReq(MUID uidParty, GEntityPlayer* pPlayer);

	virtual void 				InviteReq(MUID uidTargetPlayer, MUID uidRequestPlayer);	
	virtual void 				InviteRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult);
	virtual void 				AcceptReq(MUID uidTargetPlayer, MUID uidRequestPlayer, wstring strRequestPlayerName);
	virtual void 				AcceptRes(MUID uidTargetPlayer, PARTY_RESPOND nRespond);
	virtual void				AcceptCancel(MUID uidTargetPlayer, CCommandResultTable nResult);
	virtual void				LeaveReq(MUID uidLeavePlayer);
	virtual void				LeaveRes(MUID uidLeavePlayer, CCommandResultTable nResult);
	virtual void				KickReq(MUID uidRequestPlayer, MUID uidTargetPlayer);
	virtual void				KickRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult);
	virtual void				DoOnline(MUID uidMember);
	virtual void				DoOffline(GEntityPlayer* pMember);	
	virtual void				JoinInviteReq(MUID uidParty, MUID uidRequestPlayer);
	virtual void				JoinInviteRes(MUID uidRequestPlayer, CCommandResultTable nResult);
	virtual void				JoinAcceptReq(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, wstring strRequestPlayerName);
	virtual void				JoinAcceptRes(MUID uidLeader, PARTY_RESPOND nRespond);	
	virtual void				JoinAcceptCancel(MUID uidLeader, CCommandResultTable nResult);
	virtual void				CreateSinglePartyReq(MUID uidRequestPlayer);

	virtual void 				ChangePartyNameReq(MUID uidLeader, wstring strName);
	virtual void 				ChangePartyLeaderReq(MUID uidLeader, MUID uidNewLeader);
	virtual void 				ChangePartyLootingRuleReq(MUID uidLeader, LOOTING_RULE_DATA rule);
	virtual void				ChangeQuestIDReq(MUID uidLeader, QuestID nQuestID);

	void						FixedPartyLogOn(MUID uidMember, wstring strName) const;
		
public:		// M-G
	virtual void				AddParty(MUID uidParty, MUID uidLeader, QuestID nQuestID=INVALID_ID);
	virtual void				RemoveParty(MUID uidParty);
	virtual void				AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID);
	virtual void				RemoveMember(MUID uidParty, MUID uidMember);
	virtual void				AddOfflineMember(MUID uidParty, MUID uidMember);
	virtual void				RemoveOfflineMember(MUID uidParty, MUID uidMember, MUID uidOffline);
	virtual void    			SyncParty(const TD_PARTY& tdParty, MCommand* pPartySyncCommand);
	virtual void				SyncMember(MUID uidParty, MCommand* pPartyMemberSyncCommand);	
	virtual void				SyncQuest(MUID uidParty, const map<int, int>& mapQuests);
	virtual void				SyncField(MUID uidParty, const map<int, MUID>& mapFields);
	virtual void				SyncQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID);
	virtual void				SyncEraseQuestRunner(MUID uidParty, MUID uidMember);	
	virtual void				MoveServerSync(MUID uidParty, MUID uidMember, MUID uidOffline);
	virtual void				ChangePartyNameRes(MUID uidParty, wstring strName);
	virtual void				ChangePartyLeaderRes(MUID uidParty, MUID uidNewLeader);
	virtual void				ChangePartyLootingRuleRes(MUID uidParty, LOOTING_RULE_DATA rule);
	virtual void				ChangeQuestIDRes(MUID uidParty, int nQuestID);
	

private:
	bool						IsPartyFull(MUID uidParty) const;	
	GParty*						RestoreParty(MUID uidParty);
	void						AddQuest(MUID uidParty, GEntityPlayer* pMember) const;
	void						RemoveQuest(MUID uidParty, GEntityPlayer* pMember) const;
	void						RemoveField(MUID uidParty, GEntityPlayer* pMember) const;
	void						EraseQuestRunner(MUID uidParty, MUID uidMember) const;
	void						UpdateParty(const GParty* pParty, GEntityPlayer* pMember) const;
	bool						IsEmptyMemberInServer(const GParty* pParty) const;	

protected:
	GPartyManager*				m_pPartyManager;
	GClientPartyRouter*			m_pClientRouter;
};
