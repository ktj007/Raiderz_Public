#pragma once

#include "XDummyBot_Roam.h"

enum PARTY_BEHAVIOR
{
	PARTY_BEHAVIOR_IDLE	= 0,
	PARTY_BEHAVIOR_INVITE,
	PARTY_BEHAVIOR_RESPOND,
	PARTY_BEHAVIOR_LEAVE,
	PARTY_BEHAVIOR_KICK,
	PARTY_BEHAVIOR_CHANGE_NAME,
	PARTY_BEHAVIOR_CHANGE_LEADER,
	PARTY_BEHAVIOR_CHANGE_LOOTING_RULE,

	MAX_PARTY_BEHAVIOR
};

struct ENTITY_PLAYER
{
	MUID uid;
	UIID nUIID;
	vec3 vPos;
};

class XDummyBot_Party: public XDummyBot_Roam
{
public:
	XDummyBot_Party(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_Party();

	static const wchar_t* GetName() { return L"Party"; }
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);

protected:
	virtual void OnRun(float fDelta) override;

private:
	void PostInviteReq();
	void PostLeaveReq();
	void PostKickReq();
	void PostInviteQuestionRespond();
	void PostInviteForMeReq();
	void PostInviteForMeQuestionRespond();	
	void PostChangeNameReq();
	void PostChangeLeaderReq();
	void PostChangeLootingRuleReq();

	void GetAllowedBehavior(vector<PARTY_BEHAVIOR>& vecBehavior);
	PARTY_BEHAVIOR SelectBehavior(vector<PARTY_BEHAVIOR>& vecBehavior);
	void DoBehavior(PARTY_BEHAVIOR nBehavior);

	bool BeAdjacencyPlayer();

private:
	// 파티 정보
	MUID m_uidParty;
	MUID m_uidLeader;
	vector<MUID> m_vecMemberUID;

	// 초대 정보
	MUID m_uidInvitor;			// 나를 초대한 사람	

	// 다른 플레이어 정보	
	map<UIID, ENTITY_PLAYER> m_mapPlayers;
	MUID m_uidAdjacencyPlayer;
};
