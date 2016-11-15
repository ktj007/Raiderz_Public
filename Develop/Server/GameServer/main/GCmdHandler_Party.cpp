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
	SetCmdHandler(MC_PARTY_INVITE_REQ,						OnRequestInviteParty);
	SetCmdHandler(MC_PARTY_INVITE_FOR_ME_REQ,				OnRequestInviteForMe);
	SetCmdHandler(MC_PARTY_INVITE_QUESTION_RESPOND,			OnRequestInviteQuestionRespond);
	SetCmdHandler(MC_PARTY_LEAVE_REQ,						OnRequestLeaveParty);
	SetCmdHandler(MC_PARTY_KICK_REQ,						OnRequestKickParty);	
	SetCmdHandler(MC_PARTY_INVITE_FOR_ME_QUESTION_RESPOND,	OnRequestInviteForMeQuestionRespond);
	
	SetCmdHandler(MC_PARTY_CHANGE_NAME_REQ,					OnRequestChangeName);
	SetCmdHandler(MC_PARTY_CHANGE_LEADER_REQ,				OnRequestChangeLeader);
	SetCmdHandler(MC_PARTY_CHANGE_LOOTING_RULE_REQ,			OnRequestChangeLootingRule);
	SetCmdHandler(MC_PARTY_CHANGE_QUESTID_REQ,				OnRequestQuestID);
	
	SetCmdHandler(MC_AUTOPARTY_ENQUEUE_REQ,					OnRequestAutoPartyEnqueue);
	SetCmdHandler(MC_AUTOPARTY_DEQUEUE_REQ,					OnRequestAutoPartyDequeue);
	SetCmdHandler(MC_AUTOPARTY_CHECK_SENSOR_REQ,			OnRequestAutoPartyCheckSensor);
	SetCmdHandler(MC_AUTOPARTY_CHANGE_STATE_REQ,			OnRequestAutoPartyChangeState);
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

MCommandResult GCmdHandler_Party::OnRequestInviteForMe(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	if (!pCmd->GetParameter(&uidParty, 0, MPT_UID))		return CR_FALSE;

	
	// 가입요청
	MUID uidRequestPlayer = pCmd->GetSenderUID();
	gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


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

MCommandResult GCmdHandler_Party::OnRequestInviteForMeQuestionRespond(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nRespond;
	if (!pCmd->GetParameter(&nRespond, 0, MPT_INT))		return CR_FALSE;

	
	// 가입수락 응답
	MUID uidLeader = pCmd->GetSenderUID();
	gsys.pPartySystem->JoinAcceptRes(uidLeader, PARTY_RESPOND(nRespond));


	return CR_TRUE;
}

MCommandResult GCmdHandler_Party::OnRequestChangeName(MCommand* pCmd, MCommandHandler* pHandler)
{
	wstring strName;
	if (!pCmd->GetParameter(strName, 0, MPT_WSTR))	return CR_FALSE;

	MUID uidRequestor = pCmd->GetSenderUID();	
	gsys.pPartySystem->ChangePartyNameReq(uidRequestor, strName);

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
