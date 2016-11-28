#include "StdAfx.h"
#include "GCmdHandler_Party.h"
#include "CCommandTable.h"
#include "MiNetCommon.h"
#include "GPartySystem.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GPlayerObjectManager.h"
#include "GClientPartyRouter.h"
#include "GSharedField.h"
#include "GAutoPartyMgr.h"

GCmdHandler_Party::GCmdHandler_Party(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_PARTY_INVITE_REQ,							OnRequestInviteParty);
	SetCmdHandler(MC_PARTY_INVITE_BY_NAME_REQ,					OnRequestInvitePartyByName);
	SetCmdHandler(MC_PARTY_JOIN_REQ,							OnRequestJoinParty);
	SetCmdHandler(MC_PARTY_INVITE_QUESTION_RESPOND,				OnRequestInviteQuestionRespond);
	SetCmdHandler(MC_PARTY_LEAVE_REQ,							OnRequestLeaveParty);
	SetCmdHandler(MC_PARTY_KICK_REQ,							OnRequestKickParty);	
	SetCmdHandler(MC_PARTY_JOIN_QUESTION_RESPOND,				OnRequestJoinQuestionRespond);
	
	SetCmdHandler(MC_PARTY_CHANGE_PUBLIC_PARTY_SETTING_REQ,		OnRequestChangePublicPartySetting);
	SetCmdHandler(MC_PARTY_CHANGE_LEADER_REQ,					OnRequestChangeLeader);
	SetCmdHandler(MC_PARTY_CHANGE_LOOTING_RULE_REQ,				OnRequestChangeLootingRule);
	SetCmdHandler(MC_PARTY_CHANGE_QUESTID_REQ,					OnRequestQuestID);

	SetCmdHandler(MC_PARTY_SHOW_INFO_REQ,						OnRequestShowInfo);
	SetCmdHandler(MC_PARTY_CREATE_SINGLE_PARTY_REQ,				OnRequestCreateSingleParty);
	SetCmdHandler(MC_PARTY_MATCHING_SHOW_PUBLIC_PARTY_LIST_REQ,	OnRequestMatchingShowPublicPartyList);

	SetCmdHandler(MC_AUTOPARTY_ENQUEUE_REQ,						OnRequestAutoPartyEnqueue);
	SetCmdHandler(MC_AUTOPARTY_DEQUEUE_REQ,						OnRequestAutoPartyDequeue);
	SetCmdHandler(MC_AUTOPARTY_CHECK_SENSOR_REQ,				OnRequestAutoPartyCheckSensor);
	SetCmdHandler(MC_AUTOPARTY_CHANGE_STATE_REQ,				OnRequestAutoPartyChangeState);
}

MCommandResult GCmdHandler_Party::OnRequestInviteParty(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidTargetPlayer;
	if (!pCmd->GetParameter(&uidTargetPlayer, 0, MPT_UID))		return CR_FALSE;

	
	// 파티 가입 요청
	MUID uidRequestPlayer = pCmd->GetSenderUID();
	gsys.pPartySystem->InviteReq(uidTargetPlayer, uidRequestPlayer);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestInvitePartyByName(MCommand* pCmd, MCommandHandler* pHandler)
{
	wstring strTargetPlayer;
	if (!pCmd->GetParameter(strTargetPlayer, 0, MPT_WSTR))		return CR_FALSE;

	MUID uidRequestPlayer = pCmd->GetSenderUID();
	gsys.pPartySystem->InviteByNameReq(strTargetPlayer, uidRequestPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestJoinParty(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	if (!pCmd->GetParameter(&uidParty, 0, MPT_UID))		return CR_FALSE;

	
	// 가입요청
	MUID uidRequestPlayer = pCmd->GetSenderUID();
	gsys.pPartySystem->JoinReq(uidParty, uidRequestPlayer);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestInviteQuestionRespond(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nRespond;
	if (!pCmd->GetParameter(&nRespond, 0, MPT_INT))		return CR_FALSE;

	
	// 수락 응답
	MUID uidTargetPlayer = pCmd->GetSenderUID();	
	gsys.pPartySystem->AcceptRes(uidTargetPlayer, (PARTY_RESPOND)nRespond);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestLeaveParty(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 탈퇴 요청
	MUID uidLeavePlayer = pCmd->GetSenderUID();
	gsys.pPartySystem->LeaveReq(uidLeavePlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestKickParty(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidTargetPlayer;
	if (!pCmd->GetParameter(&uidTargetPlayer, 0, MPT_UID))		return CR_FALSE;


	// 강퇴 처리
	MUID uidRequestPlayer = pCmd->GetSenderUID();
	gsys.pPartySystem->KickReq(uidRequestPlayer, uidTargetPlayer);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestJoinQuestionRespond(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidJoinRequestPlayer;
	int nRespond;
	if (!pCmd->GetParameter(&uidJoinRequestPlayer, 0, MPT_UID))	 return CR_FALSE;
	if (!pCmd->GetParameter(&nRespond, 1, MPT_INT))		return CR_FALSE;

	
	// 가입수락 응답
	MUID uidLeader = pCmd->GetSenderUID();
	gsys.pPartySystem->JoinAcceptRes(uidLeader, uidJoinRequestPlayer, PARTY_RESPOND(nRespond));


	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestChangePublicPartySetting(MCommand* pCmd, MCommandHandler* pHandler)
{
	bool bPublicParty;
	wstring strPartyName;
	if (!pCmd->GetParameter(&bPublicParty, 0, MPT_BOOL))	return CR_FALSE;
	if (!pCmd->GetParameter(strPartyName, 1, MPT_WSTR))	return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();	
	gsys.pPartySystem->ChangePublicPartySettingReq(uidRequestor, bPublicParty, strPartyName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestChangeLeader(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidNewLeader;
	if (!pCmd->GetParameter(&uidNewLeader, 0, MPT_UID))	return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();
	gsys.pPartySystem->ChangePartyLeaderReq(uidRequestor, uidNewLeader);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestChangeLootingRule(MCommand* pCmd, MCommandHandler* pHandler)
{
	int8 nCommon;
	int8 nRare;
	int8 nRareFor;

	if (!pCmd->GetParameter(&nCommon, 0, MPT_CHAR))		return CR_FALSE;
	if (!pCmd->GetParameter(&nRare,	1, MPT_CHAR))		return CR_FALSE;
	if (!pCmd->GetParameter(&nRareFor, 2, MPT_CHAR))	return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();

	LOOTING_RULE_DATA rule;
	rule.m_nLRC = (LOOTING_RULE_COMMON)nCommon;
	rule.m_nLRR = (LOOTING_RULE_RARE)nRare;
	rule.m_nLRRF = (LOOTING_RULE_RARE_FOR)nRareFor;

	gsys.pPartySystem->ChangePartyLootingRuleReq(uidRequestor, rule);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestQuestID(MCommand* pCmd, MCommandHandler* pHandler)
{
	int nQuestID;

	if (!pCmd->GetParameter(&nQuestID, 0, MPT_INT))		return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();

	gsys.pPartySystem->ChangeQuestIDReq(uidRequestor, nQuestID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestShowInfo(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidParty;

	if (!pCmd->GetParameter(&uidParty, 0, MPT_UID))		return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();

	gsys.pPartySystem->ShowInfoReq(uidRequestor, uidParty);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestCreateSingleParty(MCommand* pCmd, MCommandHandler* pHandler)
{
	bool bPublicParty;
	wstring strPartyName;

	if (!pCmd->GetParameter(&bPublicParty, 0, MPT_BOOL))		return CR_FALSE;
	if (!pCmd->GetParameter(strPartyName, 1, MPT_WSTR))			return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();

	gsys.pPartySystem->CreateSinglePartyReq(uidRequestor, bPublicParty, strPartyName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestMatchingShowPublicPartyList(MCommand* pCmd, MCommandHandler* pHandler)
{
	char nPage;
	char nLevelMin;
	char nLevelMax;
	wstring strSearchText;

	if (!pCmd->GetParameter(&nPage, 0, MPT_CHAR))				return CR_FALSE;
	if (!pCmd->GetParameter(&nLevelMin, 1, MPT_CHAR))			return CR_FALSE;
	if (!pCmd->GetParameter(&nLevelMax, 2, MPT_CHAR))			return CR_FALSE;
	if (!pCmd->GetParameter(strSearchText, 3, MPT_WSTR))		return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();
	gsys.pPartySystem->ShowMatchingPublicPartyListReq(uidRequestor, nPage, nLevelMin, nLevelMax, strSearchText);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestAutoPartyEnqueue(MCommand* pCmd, MCommandHandler* pHandler)
{
	QuestID nQuestID;

	if (!pCmd->GetParameter(&nQuestID, 0, MPT_INT))		return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	GSharedField* pField = ToSharedField(pPlayer->GetField());
	if (!pField)
		return CR_TRUE;

	pField->GetAutoPartyMgr()->Enqueue(nQuestID, pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestAutoPartyDequeue(MCommand* pCmd, MCommandHandler* pHandler)
{
	QuestID nQuestID;

	if (!pCmd->GetParameter(&nQuestID, 0, MPT_INT))		return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	GSharedField* pField = ToSharedField(pPlayer->GetField());
	if (!pField)
		return CR_TRUE;

	pField->GetAutoPartyMgr()->Dequeue(nQuestID, pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestAutoPartyCheckSensor(MCommand* pCmd, MCommandHandler* pHandler)
{
	QuestID nQuestID;
	if (!pCmd->GetParameter(&nQuestID, 0, MPT_INT))		return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	GSharedField* pField = ToSharedField(pPlayer->GetField());
	if (!pField)
		return CR_TRUE;

	AUTOPARTY_STATE nState = pField->GetAutoPartyMgr()->GetAutopartyState(nQuestID, pPlayer);
	pField->GetAutoPartyMgr()->RouteCheckSensor(pPlayer, nQuestID, nState);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestAutoPartyChangeState(MCommand* pCmd, MCommandHandler* pHandler)
{
	QuestID nQuestID;
	AUTOPARTY_STATE nState;
	if (!pCmd->GetParameter(&nQuestID, 0, MPT_INT))		return CR_FALSE;
	if (!pCmd->GetParameter(&nState, 1, MPT_INT))		return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	GSharedField* pField = ToSharedField(pPlayer->GetField());
	if (!pField)
		return CR_TRUE;

	pField->GetAutoPartyMgr()->ChangeAutopartyState(pPlayer, nQuestID, nState);

	return CR_TRUE;
}
