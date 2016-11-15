#include "stdafx.h"
#include "GCmdHandler_Master_Party.h"
#include "CCommandTable_Master.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "GPartySystem.h"
#include "GProxyParty.h"
#include "GAppServerFacade.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

GCmdHandler_Master_Party::GCmdHandler_Master_Party(MCommandCommunicator* pCC): MCommandHandler(pCC)
{
	SetCmdHandler(MMC_PARTY_INVITE_RES,					OnPartyInviteRes);
	SetCmdHandler(MMC_PARTY_ACCEPT_REQ,					OnPartyAcceptReq);
	SetCmdHandler(MMC_PARTY_ACCEPT_CANCEL,				OnPartyAcceptCancel);
	SetCmdHandler(MMC_PARTY_MEMBER_SYNC,				OnPartyMemberSync);
	SetCmdHandler(MMC_PARTY_SYNC,						OnPartySync);	

	SetCmdHandler(MMC_PARTY_ADD,						OnPartyAdd);
	SetCmdHandler(MMC_PARTY_REMOVE,						OnPartyRemove);
	SetCmdHandler(MMC_PARTY_ADD_MEMBER,					OnPartyAddMember);	
	SetCmdHandler(MMC_PARTY_REMOVE_MEMBER,				OnPartyRemoveMember);
	SetCmdHandler(MMC_PARTY_QUEST_SYNC,					OnPartyQuestSync);
	SetCmdHandler(MMC_PARTY_FIELD_SYNC,					OnPartyFieldSync);
	SetCmdHandler(MMC_PARTY_QUEST_RUNNER_SYNC,			OnPartyQuestRunnerSync);

	SetCmdHandler(MMC_PARTY_LEAVE_RES,					OnPartyLeaveRes);
	SetCmdHandler(MMC_PARTY_ERASE_QUEST_RUNNER_SYNC,	OnPartyEraseQuestRunnerSync);

	SetCmdHandler(MMC_PARTY_KICK_RES,					OnPartyKickRes);

	SetCmdHandler(MMC_PARTY_ADD_OFFLINE_MEMBER,			OnPartyAddOfflineMember);
	SetCmdHandler(MMC_PARTY_REMOVE_OFFLINE_MEMBER,		OnPartyRemoveOfflineMember);

	SetCmdHandler(MMC_PARTY_JOIN_ACCEPT_REQ,			OnPartyJoinAcceptReq);
	SetCmdHandler(MMC_PARTY_JOIN_INVITE_RES,			OnPartyJoinInviteRes);
	SetCmdHandler(MMC_PARTY_JOIN_ACCEPT_CANCEL, 		OnPartyJoinAcceptCancel);

	SetCmdHandler(MMC_PARTY_MOVE_SERVER_SYNC,			OnPartyMoveServerSync);

	SetCmdHandler(MMC_PARTY_INFO_ALL_RES,				OnPartyInfoAllRes);

	SetCmdHandler(MMC_PARTY_CHANGE_NAME,				OnPartyChangeNameRes);
	SetCmdHandler(MMC_PARTY_CHANGE_LEADER,				OnPartyChangeLeaderRes);
	SetCmdHandler(MMC_PARTY_CHANGE_LOOTING_RULE,		OnPartyChangeLootingRuleRes);
	SetCmdHandler(MMC_PARTY_CHANGE_QUESTID,				OnPartyChangeQuestIDRes);

	SetCmdHandler(MMC_PARTY_FAIL,						OnPartyFail);
}

MCommandResult GCmdHandler_Master_Party::OnPartyAdd(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidLeader;
	wstring strLeaderName;
	int nLeaderCID;

	if (!pCommand->GetParameter(&uidParty,		0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader,		1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strLeaderName,	2, MPT_WSTR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nLeaderCID,	3, MPT_INT))	return CR_ERROR;

	gsys.pMasterServerFacade->AddProxyParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
	gsys.pPartySystem->AddParty(uidParty, uidLeader);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyRemove(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	if (!pCommand->GetParameter(&uidParty,		0, MPT_UID))	return CR_ERROR;

	gsys.pMasterServerFacade->RemoveProxyParty(uidParty);
	gsys.pPartySystem->RemoveParty(uidParty);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyAddMember(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidMember;
	wstring strMemberName;
	int nMemberCID;

	if (!pCommand->GetParameter(&uidParty,		0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember,		1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strMemberName,	2, MPT_WSTR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nMemberCID,	3, MPT_INT))	return CR_ERROR;

	gsys.pMasterServerFacade->AddProxyPartyMember(uidParty, uidMember, strMemberName, nMemberCID);	
	gsys.pPartySystem->AddMember(uidParty, uidMember, strMemberName, nMemberCID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyRemoveMember(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	MUID uidMember;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;


	// 멤버 제거
	gsys.pMasterServerFacade->RemoveProxyPartyMember(uidParty, uidMember);	
	gsys.pPartySystem->RemoveMember(uidParty, uidMember);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyInviteRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;
	MUID uidTargetPlayer;	
	int nResult;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidTargetPlayer, 1, MPT_UID))		return CR_ERROR;	
	if (!pCommand->GetParameter(&nResult, 2, MPT_INT))				return CR_ERROR;


	gsys.pPartySystem->InviteRes(uidRequestPlayer, uidTargetPlayer, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyAcceptReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidTargetPlayer;
	MUID uidRequestPlayer;
	wstring strRequestPlayerName;

	if (!pCommand->GetParameter(&uidTargetPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(strRequestPlayerName, 2, MPT_WSTR))	return CR_ERROR;
	if (PLAYER_NAME_LEN < strRequestPlayerName.length())			return CR_ERROR;


	gsys.pPartySystem->AcceptReq(uidTargetPlayer, uidRequestPlayer, strRequestPlayerName);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyAcceptCancel(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidTargetPlayer;
	int nResult;

	if (!pCommand->GetParameter(&uidTargetPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 1, MPT_INT))				return CR_ERROR;


	gsys.pPartySystem->AcceptCancel(uidTargetPlayer, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyMemberSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	TD_PARTY_MEMBER* pPartyMember;
	int nCount;
	vector<int> vecBuff;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(pPartyMember, nCount, 1))		return CR_ERROR;
	if (!pCommand->GetBlob(vecBuff, 2))						return CR_ERROR;


	// 파티원 정보 갱신 전달
	gsys.pPartySystem->SyncMember(uidParty, pCommand);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartySync(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	TD_PARTY* pParty;
	int nCount;

	if (!pCommand->GetBlob(pParty, nCount, 0))		return CR_ERROR;
	if (NULL == pParty) return CR_ERROR;

	// 파티 확인
	MUID uidParty = pParty->m_uidParty;
	gsys.pPartySystem->SyncParty(*pParty, pCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyQuestSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	vector<TD_PARTY_QUEST> vecPartyQuest;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(vecPartyQuest, 1))				return CR_ERROR;


	// 퀘스트 갱신
	map<int, int> mapQuests;

	for each(const TD_PARTY_QUEST& partyQuest in vecPartyQuest)
	{
		int nID = partyQuest.nQuestID;
		int nCount = partyQuest.nQuestCount;

		mapQuests.insert(make_pair(nID, nCount));
	}

	gsys.pPartySystem->SyncQuest(uidParty, mapQuests);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyFieldSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	vector<TD_PARTY_FIELD> vecPartyField;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(vecPartyField, 1))				return CR_ERROR;


	// 필드 갱신
	map<int, MUID> mapFields;
	for each(const TD_PARTY_FIELD& partyField in vecPartyField)
	{
		int nFieldGroupID = partyField.nFieldGroupID;
		MUID uidFieldGroup = partyField.uidFieldGroup;

		mapFields.insert(make_pair(nFieldGroupID, uidFieldGroup));
	}

	gsys.pPartySystem->SyncField(uidParty, mapFields);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyQuestRunnerSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	MUID uidMember;
	MUID uidField;
	vector<int> vecQuestID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidField, 2, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetBlob(vecQuestID, 3))					return CR_ERROR;


	// 퀘스트 러너 갱신
	gsys.pPartySystem->SyncQuestRunner(uidParty, uidMember, uidField, vecQuestID);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyLeaveRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidLeavePlayer;
	int nResult;

	if (!pCommand->GetParameter(&uidLeavePlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 1, MPT_INT))				return CR_ERROR;


	// 파티탈퇴 응답
	gsys.pPartySystem->LeaveRes(uidLeavePlayer, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyKickRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;
	MUID uidTargetPlayer;
	int nResult;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidTargetPlayer, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 2, MPT_INT))				return CR_ERROR;


	gsys.pPartySystem->KickRes(uidRequestPlayer, uidTargetPlayer, (CCommandResultTable)nResult);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyEraseQuestRunnerSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	MUID uidMember;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))	return CR_ERROR;


	// 퀘스트 러너 제거
	gsys.pPartySystem->SyncEraseQuestRunner(uidParty, uidMember);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyAddOfflineMember(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	MUID uidMember;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;


	// 오프라인 파티원 추가
	gsys.pPartySystem->AddOfflineMember(uidParty, uidMember);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyRemoveOfflineMember(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	MUID uidMember;
	MUID uidOffline;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidOffline, 2, MPT_UID))		return CR_ERROR;


	// 오프라인 파티원 제거
	GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);	
	if (pProxyParty == NULL)	return CR_FALSE;

	pProxyParty->ChangeUID(uidOffline, uidMember);
	gsys.pPartySystem->RemoveOfflineMember(uidParty, uidMember, uidOffline);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyJoinAcceptReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidParty;
	MUID uidLeader;
	MUID uidRequestPlayer;
	wstring strRequestPlayerName;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))					return CR_ERROR;
	if (!pCommand->GetParameter(&uidLeader, 1, MPT_UID))				return CR_ERROR;
	if (!pCommand->GetParameter(&uidRequestPlayer, 2, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(strRequestPlayerName, 3, MPT_WSTR))		return CR_ERROR;


	// 가입 요청
	gsys.pPartySystem->JoinAcceptReq(uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyJoinInviteRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequestPlayer;
	int nResult;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 1, MPT_INT))				return CR_ERROR;


	// 가입 응답
	gsys.pPartySystem->JoinInviteRes(uidRequestPlayer, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyJoinAcceptCancel(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidLeader;
	int nResult;

	if (!pCommand->GetParameter(&uidLeader, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult, 1, MPT_INT))			return CR_ERROR;


	// 가입 응답
	gsys.pPartySystem->JoinAcceptCancel(uidLeader, (CCommandResultTable)nResult);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyMoveServerSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 컨멘드 확인
	MUID uidParty;
	MUID uidMember;
	MUID uidOffline;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))			return CR_ERROR;
	if (!pCommand->GetParameter(&uidMember, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidOffline, 2, MPT_UID))		return CR_ERROR;


	// 오프라인 파티원 제거
	GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);
	if (pProxyParty == NULL)	return CR_FALSE;

	// 서버이동 완료 상태 동기화
	pProxyParty->ChangeUID(uidOffline, uidMember);	
	gsys.pPartySystem->MoveServerSync(uidParty, uidMember, uidOffline);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyInfoAllRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	TD_PARTY_INFO* pPartyInfo;
	int nCount;

	if (!pCommand->GetBlob(pPartyInfo, nCount, 0))		return CR_ERROR;


	// 정보 동기화
	gsys.pMasterServerFacade->RestoreProxyParty(pPartyInfo, nCount);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyChangeNameRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	wstring strName;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(strName, 1, MPT_WSTR))	return CR_ERROR;

	gsys.pPartySystem->ChangePartyNameRes(uidParty, strName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyChangeLeaderRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	MUID uidNewLeader;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidNewLeader, 1, MPT_UID))	return CR_ERROR;

	gsys.pMasterServerFacade->ChangeProxyPartyLeader(uidParty, uidNewLeader);
	gsys.pPartySystem->ChangePartyLeaderRes(uidParty, uidNewLeader);	

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyChangeLootingRuleRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	int8 nCommon;
	int8 nRare;
	int8 nRareFor;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nCommon, 1, MPT_CHAR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nRare, 2, MPT_CHAR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nRareFor, 3, MPT_CHAR))	return CR_ERROR;

	LOOTING_RULE_DATA rule;
	rule.m_nLRC = (LOOTING_RULE_COMMON)nCommon;
	rule.m_nLRR = (LOOTING_RULE_RARE)nRare;
	rule.m_nLRRF = (LOOTING_RULE_RARE_FOR)nRareFor;

	gsys.pPartySystem->ChangePartyLootingRuleRes(uidParty, rule);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyChangeQuestIDRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidParty;
	int nQuestID;

	if (!pCommand->GetParameter(&uidParty, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nQuestID, 1, MPT_INT))	return CR_ERROR;

	gsys.pPartySystem->ChangeQuestIDRes(uidParty, nQuestID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Party::OnPartyFail(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidPlayer;
	CCommandResultTable nResult;

	if (!pCommand->GetParameter(&uidPlayer,		0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,		1, MPT_INT))		return CR_ERROR;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (pPlayer == NULL)		return CR_ERROR;

	pPlayer->FailAndRouteSystemMsg(nResult);

	return CR_TRUE;
}
