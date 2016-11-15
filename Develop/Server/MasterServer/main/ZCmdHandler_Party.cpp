#include "StdAfx.h"
#include "ZCmdHandler_Party.h"
#include "MCommand.h"
#include "ZCommandTable.h"
#include "ZCommandCenter.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZServer.h"
#include "MCommandCommunicator.h"
#include "ZNetServer.h"
#include "CTransData.h"
#include "ZPartyLogic.h"
#include "ZPartyMember.h"


ZCmdHandler_Party::ZCmdHandler_Party(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_PARTY_INVITE_REQ,					OnPartyInviteReq);
	SetCmdHandler(MMC_PARTY_ACCEPT_RES,					OnPartyAcceptRes);	
	SetCmdHandler(MMC_PARTY_UPDATE,						OnPartyUpdate);
	SetCmdHandler(MMC_PARTY_UPDATE_MEMBER,				OnPartyUpdateMember);
	SetCmdHandler(MMC_PARTY_ADD_QUEST,					OnPartyAddQuest);
	SetCmdHandler(MMC_PARTY_ADD_FIELD,					OnPartyAddField);
	SetCmdHandler(MMC_PARTY_UPDATE_QUEST_RUNNER,		OnPartyUpdateQuestRunner);
	SetCmdHandler(MMC_PARTY_LEAVE_REQ,					OnPartyLeaveReq);

	SetCmdHandler(MMC_PARTY_REMOVE_QUEST,				OnPartyRemoveQuest);
	SetCmdHandler(MMC_PARTY_REMOVE_FIELD,				OnPartyRemoveField);
	SetCmdHandler(MMC_PARTY_ERASE_QUEST_RUNNER,			OnPartyEraseQuestRunner);

	SetCmdHandler(MMC_PARTY_KICK_REQ,					OnPartyKickReq);	

	SetCmdHandler(MMC_PARTY_DO_OFFLINE,					OnPartyDoOffline);
	SetCmdHandler(MMC_PARTY_DO_ONLINE,					OnPartyDoOnline);

	SetCmdHandler(MMC_PARTY_JOIN_INVITE_REQ,			OnPartyJoinInviteReq);
	SetCmdHandler(MMC_PARTY_JOIN_ACCEPT_RES,			OnPartyJoinAcceptRes);

	SetCmdHandler(MMC_PARTY_MOVE_SERVER,				OnPartyMoveServer);

	SetCmdHandler(MMC_PARTY_CREATE_SINGLE_REQ,			OnPartyCreateSingleReq);
	SetCmdHandler(MMC_PARTY_INFO_ALL_REQ,				OnPartyInfoAllReq);

	SetCmdHandler(MMC_PARTY_REMOVE_FIELD_SELF,			OnPartyRemoveFieldSelf);	

	SetCmdHandler(MMC_PARTY_CHANGE_NAME_REQ,			OnPartyChangeNameReq);
	SetCmdHandler(MMC_PARTY_CHANGE_LEADER_REQ,			OnPartyChangeLeaderReq);
	SetCmdHandler(MMC_PARTY_CHANGE_LOOTING_RULE_REQ,	OnPartyChangeLootingRuleReq);
	SetCmdHandler(MMC_PARTY_CHANGE_QUESTID_REQ,			OnPartyChangeQuestIDReq);

	SetCmdHandler(MMC_PARTY_FIXED_LOG_ON,				OnPartyFixedLogOn);
	SetCmdHandler(MMC_PARTY_CREATE_AUTO_PARTY_REQ,		OnPartyCreateAutoPartyReq);
	SetCmdHandler(MMC_PARTY_JOIN_ATUTO_PARTY_REQ,		OnPartyJoinAutoPartyReq);
}

MCommandResult ZCmdHandler_Party::OnPartyInviteReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidTargetPlayer;
	MUID uidRequestPlayer;

	if (!pCommand->GetParameter(&uidTargetPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer, 1, MPT_UID))		return CR_ERROR;


	// 파티 초대 요청
	MUID uidSender = pCommand->GetSenderUID();

	gsys.pPartyLogic->InviteReq(uidSender, uidTargetPlayer, uidRequestPlayer);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyAcceptRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidRequestPlayer;
	MUID uidTargetPlayer;
	int nResult;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidTargetPlayer, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 2, MPT_INT))				return CR_ERROR;


	// 초대 수락 응답
	MUID uidSender = pCommand->GetSenderUID();

	gsys.pPartyLogic->AcceptRes(uidSender, uidRequestPlayer, uidTargetPlayer, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyUpdate(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	TD_PARTY_MEMBER* pPartyMember;
	int nCount;
	vector<int> vecBuff;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(pPartyMember, nCount, 1))		return CR_ERROR;
	if (!pCommand->GetBlob(vecBuff, 2))						return CR_ERROR;


	// 파티원 정보 갱신
	ZPartyMember partyMember;
	partyMember.Assign(*pPartyMember, vecBuff);

	gsys.pPartyLogic->UpdateParty(uidParty, &partyMember);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyUpdateMember(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	TD_PARTY_MEMBER* pPartyMember;
	int nCount;
	vector<int> vecBuff;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(pPartyMember, nCount, 1))		return CR_ERROR;
	if (!pCommand->GetBlob(vecBuff, 2))						return CR_ERROR;


	// 파티원 정보 갱신
	ZPartyMember partyMember;
	partyMember.Assign(*pPartyMember, vecBuff);

	gsys.pPartyLogic->UpdateMember(uidParty, &partyMember);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyAddQuest(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;
	vector<int> vecQuestID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetBlob(vecQuestID, 2))					return CR_ERROR;


	// 퀘스트 추가	
	gsys.pPartyLogic->AddQuest(uidParty, uidMember, vecQuestID);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyAddField(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;	
	MUID uidMember;
	MUID uidFieldGroup;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidFieldGroup, 2, MPT_UID))	return CR_ERROR;


	// 필드 추가
	gsys.pPartyLogic->AddField(uidParty, uidMember, uidFieldGroup);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyUpdateQuestRunner(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;
	MUID uidField;
	vector<int> vecQuestID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidField, 2, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(vecQuestID, 3))					return CR_ERROR;


	// 퀘스트 러너 동기화
	gsys.pPartyLogic->UpdateQuestRunner(uidParty, uidMember, uidField, vecQuestID);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyLeaveReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidLeavePlayer;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeavePlayer, 1, MPT_UID))		return CR_ERROR;


	// 탈퇴 처리
	MUID uidSender = pCommand->GetSenderUID();

	gsys.pPartyLogic->LeaveReq(uidSender, uidParty, uidLeavePlayer);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyRemoveQuest(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;
	vector<int> vecQuestID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetBlob(vecQuestID, 2))					return CR_ERROR;


	// 퀘스트 제거
	gsys.pPartyLogic->RemoveQuest(uidParty, uidMember, vecQuestID);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyRemoveField(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커메드 확인
	MUID uidParty;
	MUID uidMember;
	int nFieldGroupID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldGroupID, 2, MPT_INT))		return CR_ERROR;


	// 필드 제거
	gsys.pPartyLogic->RemoveField(uidParty, uidMember, nFieldGroupID);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyEraseQuestRunner(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;


	// 퀘스트 러너 제거
	gsys.pPartyLogic->EraseQuestRunner(uidParty, uidMember);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyKickReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidRequestPlayer;
	MUID uidTargetPlayer;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidTargetPlayer, 2, MPT_UID))		return CR_ERROR;


	// 강퇴 처리
	MUID uidSender = pCommand->GetSenderUID();

	gsys.pPartyLogic->KickReq(uidSender, uidParty, uidRequestPlayer, uidTargetPlayer);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyDoOffline(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;


	// 파티원 오프라인
	gsys.pPartyLogic->DoOffline(uidParty, uidMember);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyDoOnline(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;
	MUID uidOffline;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidOffline, 2, MPT_UID))		return CR_ERROR;


	// 파티원 온라인
	gsys.pPartyLogic->DoOnline(uidParty, uidMember, uidOffline);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyJoinInviteReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidRequestPlayer;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer, 1, MPT_UID))		return CR_ERROR;


	// 가입 요청
	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->JoinInviteReq(uidSender, uidParty, uidRequestPlayer);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyJoinAcceptRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidLeader;
	MUID uidRequestPlayer;
	int nResult;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader, 1, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer, 2, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 3, MPT_INT))				return CR_ERROR;


	// 수락 응답
	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->JoinAcceptRes(uidSender, uidParty, uidLeader, uidRequestPlayer, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyMoveServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidParty;
	MUID uidMember;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;


	// 서버이동 상태 통보
	gsys.pPartyLogic->MoveServer(uidParty, uidMember);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyCreateSingleReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	MUID uidRequestPlayer;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))		return CR_ERROR;


	// 서버이동 상태 통보
	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->CreateSingleReq(uidSender, uidRequestPlayer);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyInfoAllReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// no parameter command

	// 파티 동기화
	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->PartyInfoAllReq(uidSender);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyRemoveFieldSelf(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커메드 확인
	MUID uidParty;
	int nFieldGroupID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldGroupID, 1, MPT_INT))		return CR_ERROR;


	// 필드 제거
	gsys.pPartyLogic->RemoveFieldSelf(uidParty, nFieldGroupID);


	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyChangeNameReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidLeader;
	wstring strName;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strName, 2, MPT_WSTR))	return CR_ERROR;

	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->ChangeNameReq(uidSender, uidParty, uidLeader, strName);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyChangeLeaderReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidLeader;
	MUID uidNewLeader;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidNewLeader, 2, MPT_UID))	return CR_ERROR;

	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->ChangeLeaderReq(uidSender, uidParty, uidLeader, uidNewLeader);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyChangeLootingRuleReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidLeader;
	int8 nCommon;
	int8 nRare;
	int8 nRareFor;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nCommon, 2, MPT_CHAR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nRare, 3, MPT_CHAR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nRareFor, 4, MPT_CHAR))	return CR_ERROR;

	LOOTING_RULE_DATA rule;
	rule.m_nLRC = (LOOTING_RULE_COMMON)nCommon;
	rule.m_nLRR = (LOOTING_RULE_RARE)nRare;
	rule.m_nLRRF = (LOOTING_RULE_RARE_FOR)nRareFor;

	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->ChangeLootingRuleReq(uidSender, uidParty, uidLeader, rule);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyChangeQuestIDReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidLeader;
	int nQuestID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nQuestID, 2, MPT_INT))	return CR_ERROR;

	MUID uidSender = pCommand->GetSenderUID();
	gsys.pPartyLogic->ChangeQuestIDReq(uidSender, uidParty, uidLeader, nQuestID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyFixedLogOn(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidMember;
	MUID uidOffline;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidOffline, 2, MPT_UID))	return CR_ERROR;

	gsys.pPartyLogic->FixedPartyLogOn(uidParty, uidMember, uidOffline);

	return CR_TRUE;
}


MCommandResult ZCmdHandler_Party::OnPartyCreateAutoPartyReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<MUID> vecMemberUID;
	int nQuestID = 0;

	if (!pCommand->GetBlob(vecMemberUID, 0))			return CR_ERROR;
	if (!pCommand->GetParameter(&nQuestID, 1, MPT_INT))	return CR_ERROR;


	gsys.pPartyLogic->CreateAutoPartyReq(nQuestID, vecMemberUID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Party::OnPartyJoinAutoPartyReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidPlayer;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidPlayer, 1, MPT_UID))	return CR_ERROR;


	gsys.pPartyLogic->JoinAotuPartyReq(uidParty, uidPlayer);

	return CR_TRUE;
}
