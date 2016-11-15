#include "stdafx.h"
#include "ZPartyCommandRouter.h"
#include "STransData_M2G.h"
#include "CTransData.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "ZParty.h"
#include "ZPartyMember.h"

ZPartyCommandRouter::ZPartyCommandRouter()
{
	// do nothing
}

ZPartyCommandRouter::~ZPartyCommandRouter()
{
	// do nothing
}

void ZPartyCommandRouter::InviteRes(MUID uidTarget, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_INVITE_RES,
							uidTarget,
							3,
							NEW_UID(uidRequestPlayer),
							NEW_UID(uidTargetPlayer),
							NEW_INT(nResult)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::AcceptReq(MUID uidTarget, MUID uidTargetPlayer, MUID uidRequestPlayer, wstring strRequestPlayerName)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_ACCEPT_REQ,
							uidTarget,
							3,
							NEW_UID(uidTargetPlayer),
							NEW_UID(uidRequestPlayer),
							NEW_WSTR(strRequestPlayerName.c_str())
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::AcceptCancel(MUID uidTarget, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_ACCEPT_CANCEL,
							uidTarget,
							2,
							NEW_UID(uidTargetPlayer),
							NEW_INT(nResult)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::LeaveRes(MUID uidTarget, MUID uidLeavePlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_LEAVE_RES,
							uidTarget,
							2,
							NEW_UID(uidLeavePlayer),
							NEW_INT(nResult)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::KickRes(MUID uidTarget, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_KICK_RES,
							uidTarget,
							3,
							NEW_UID(uidRequestPlayer),
							NEW_UID(uidTargetPlayer),
							NEW_INT(nResult)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::JoinInviteRes(MUID uidTarget, MUID uidRequestPlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_JOIN_INVITE_RES,
							uidTarget,
							2,
							NEW_UID(uidRequestPlayer),
							NEW_INT(nResult)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::JoinAcceptReq(MUID uidTarget, MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, wstring strRequestPlayerName)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_JOIN_ACCEPT_REQ,
							uidTarget,
							4,
							NEW_UID(uidParty),
							NEW_UID(uidLeader),
							NEW_UID(uidRequestPlayer),
							NEW_WSTR(strRequestPlayerName.c_str())
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::JoinAcceptCancel(MUID uidTarget, MUID uidLeader, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_JOIN_ACCEPT_CANCEL,
							uidTarget,
							2,
							NEW_UID(uidLeader),
							NEW_INT(nResult)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::PartyInfoAllRes(MUID uidTarget, vector<TD_PARTY_INFO>& vecPartyInfo)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_INFO_ALL_RES,
							uidTarget,
							1,
							NEW_BLOB(vecPartyInfo)
							);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::Fail(MUID uidTarget, MUID uidPlayer, CCommandResultTable nResult )
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_FAIL,
		uidTarget,
		2,
		NEW_UID(uidPlayer),
		NEW_INT(nResult)
		);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void ZPartyCommandRouter::PartyAdd(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_ADD,
							4,
							NEW_UID(uidParty),
							NEW_UID(uidLeader),
							NEW_WSTR(strLeaderName.c_str()),
							NEW_INT(nLeaderCID)
							);
	
	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::PartyRemove(MUID uidParty)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_REMOVE,		
							1,
							NEW_UID(uidParty)
							);
	
	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_ADD_MEMBER,
							4,
							NEW_UID(uidParty),
							NEW_UID(uidMember),
							NEW_WSTR(strMemberName.c_str()),
							NEW_INT(nMemberCID)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::RemoveMember(MUID uidParty, MUID uidMember)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_REMOVE_MEMBER,
							2,
							NEW_UID(uidParty),
							NEW_UID(uidMember)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::PartySync(const ZParty* pParty)
{
	_ASSERT(pParty != NULL);

	TD_PARTY_SETTING tdPartySetting;
	tdPartySetting.Import(pParty->GetPartySetting());

	TD_PARTY tdParty(pParty->GetUID(), tdPartySetting);

	vector<TD_PARTY_MEMBER> vecMember;
	vector<vector<int> > vecBuff(MAX_PARTY_MEMBER_COUNT);

	int i = 0;
	ZParty::member_iterator it = pParty->BeginMember();
	while(it != pParty->EndMember())
	{
		ZPartyMember* pPartyMember = it->second;

		TD_PARTY_MEMBER tdMember;		
		pPartyMember->Export(&tdMember, &vecBuff[i]);
		vecMember.push_back(tdMember);

		i++;
		if (MAX_PARTY_MEMBER_COUNT <= i)	break;
		
		it++;		
	}


	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_SYNC,
							7,
							NEW_BLOB(&tdParty, sizeof(tdParty), 1),
							NEW_BLOB(vecMember),
							NEW_BLOB(vecBuff[0]),
							NEW_BLOB(vecBuff[1]),
							NEW_BLOB(vecBuff[2]),
							NEW_BLOB(vecBuff[3]),
							NEW_BLOB(vecBuff[4])
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::MemberSync(MUID uidParty, const ZPartyMember* pPartyMember)
{
	_ASSERT(pPartyMember != NULL);

	TD_PARTY_MEMBER tdPartyMember;
	vector<int> vecBuff;
	pPartyMember->Export(&tdPartyMember, &vecBuff);

	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_MEMBER_SYNC,
							3,
							NEW_UID(uidParty),
							NEW_BLOB(&tdPartyMember, sizeof(tdPartyMember), 1),
							NEW_BLOB(vecBuff)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::QuestSync(MUID uidParty, vector<TD_PARTY_QUEST>& vecPartyQuest)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_QUEST_SYNC,
							2,
							NEW_UID(uidParty),
							NEW_BLOB(vecPartyQuest)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::FieldSync(MUID uidParty, vector<TD_PARTY_FIELD>& vecPartyField)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_FIELD_SYNC,
							2,
							NEW_UID(uidParty),
							NEW_BLOB(vecPartyField)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::QuestRunnerSync(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_QUEST_RUNNER_SYNC,
							4,
							NEW_UID(uidParty),
							NEW_UID(uidMember),
							NEW_UID(uidField),
							NEW_BLOB(vecQuestID)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::EraseQuestRunnerSync(MUID uidParty, MUID uidMember)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_ERASE_QUEST_RUNNER_SYNC,
							2,
							NEW_UID(uidParty),
							NEW_UID(uidMember)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::ChangeLeaderRes(MUID uidParty, MUID uidNewLeader)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_CHANGE_LEADER,
							2,
							NEW_UID(uidParty),
							NEW_UID(uidNewLeader)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::AddOfflineMember(MUID uidParty, MUID uidMember)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_ADD_OFFLINE_MEMBER,
							2,
							NEW_UID(uidParty),
							NEW_UID(uidMember)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::RemoveOfflineMember(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_REMOVE_OFFLINE_MEMBER,
							3,
							NEW_UID(uidParty),
							NEW_UID(uidMember),
							NEW_UID(uidOffline)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::MoveServerSync(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_MOVE_SERVER_SYNC,
							3,
							NEW_UID(uidParty),
							NEW_UID(uidMember),
							NEW_UID(uidOffline)
							);

	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::ChangeNameRes( MUID uidParty, wstring strName )
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_CHANGE_NAME,
															2,
															NEW_UID(uidParty),
															NEW_WSTR(strName.c_str())
															);
	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::ChangeLootingRuleRes( MUID uidParty, LOOTING_RULE_DATA rule )
{
	int8 nCommon = rule.m_nLRC;
	int8 nRare = rule.m_nLRR;
	int8 nRareFor = rule.m_nLRRF;

	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_CHANGE_LOOTING_RULE,
															4,
															NEW_UID(uidParty),
															NEW_CHAR(nCommon),
															NEW_CHAR(nRare),
															NEW_CHAR(nRareFor)
															);
	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

void ZPartyCommandRouter::ChangeQuestIDRes( MUID uidParty, int nQuestID)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_PARTY_CHANGE_QUESTID,
		2,
		NEW_UID(uidParty),
		NEW_INT(nQuestID)
		);
	gsys.pCommandCenter->RouteToGameServer(pNewCmd);
}

