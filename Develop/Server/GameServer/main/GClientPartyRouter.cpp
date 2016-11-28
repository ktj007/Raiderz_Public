#include "stdafx.h"
#include "GClientPartyRouter.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "CCommandTable_Master.h"
#include "GGlobal.h"
#include "GCommandCenter.h"
#include "GParty.h"
#include "GPlayerObjectManager.h"

GClientPartyRouter::GClientPartyRouter()
{
	// do nothing
}

GClientPartyRouter::~GClientPartyRouter()
{
	// do nothing
}

void GClientPartyRouter::AcceptCancel(const GEntityPlayer* pTarget, CCommandResultTable nResult)
{
	_ASSERT(pTarget != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_ACCEPT_CANCEL,
										1,
										NEW_INT(nResult)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::JoinAcceptCancel(const GEntityPlayer* pTarget, CCommandResultTable nResult)
{
	_ASSERT(pTarget != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_ACCEPT_CANCEL,
										1,
										NEW_INT(nResult)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::InviteQuestion(const GEntityPlayer* pTarget, MUID uidInviter, const wchar_t* szInviterName, bool isCombatInvitee)
{
	_ASSERT(pTarget != NULL);
	_ASSERT(szInviterName != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_INVITE_QUESTION,
										3,
										NEW_UID(uidInviter),
										NEW_WSTR(szInviterName),
										NEW_BOOL(isCombatInvitee)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::InviteRespond(const GEntityPlayer* pTarget, CCommandResultTable nResult)
{
	_ASSERT(pTarget != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_INVITE,
										1,
										NEW_INT(nResult)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::JoinRes(const GEntityPlayer* pTarget, CCommandResultTable nResult)
{
	_ASSERT(pTarget != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_JOIN_RES,
										1,
										NEW_INT(nResult)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::JoinQuestion(const GEntityPlayer* pTarget, MUID uidInvitee, const wchar_t* szInviteeName, int nInviteeLevel, int nInviteeTalentStyle, int nInviteeFieldID)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_JOIN_QUESTION,
										5,
										NEW_UID(uidInvitee),
										NEW_WSTR(szInviteeName),
										NEW_INT(nInviteeLevel),
										NEW_INT(nInviteeTalentStyle),
										NEW_INT(nInviteeFieldID)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::NotifyJoin(const GParty* pTarget, MUID uidMember, const wchar_t* szMembersName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_NOTIFY_JOIN,
										2,
										NEW_UID(uidMember),
										NEW_WSTR(szMembersName)
										);

	RouteToAllMembers(pTarget, pNewCmd);
}

void GClientPartyRouter::PartySync(GParty* pTarget, MCommand* pPartySyncCommand)
{
	_ASSERT(pPartySyncCommand != NULL);
	_ASSERT(MMC_PARTY_SYNC == pPartySyncCommand->GetID());
	
	MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MC_PARTY_REFRESH_INFO_ALL);

	for(int i = 0; i < pPartySyncCommand->GetParameterCount(); i++)
	{
		MCommandParameter* pParameter = pPartySyncCommand->GetParameter(i);
		_ASSERT(pParameter != NULL);
		pNewCmd->AddParameter(pParameter->Clone());
	}

	RouteToAllMembers(pTarget, pNewCmd);
}

void GClientPartyRouter::MemberSync(GParty* pTarget, MCommand* pPartyMemberSyncCommand)
{
	_ASSERT(pPartyMemberSyncCommand != NULL);
	_ASSERT(MMC_PARTY_MEMBER_SYNC == pPartyMemberSyncCommand->GetID());

	MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MC_PARTY_REFRESH_INFO);

	for(int i = 0; i < pPartyMemberSyncCommand->GetParameterCount(); i++)
	{		
		// 0번 패킷 uidParty는 복사에서 제외.
		if (i == 0)		continue;

		MCommandParameter* pParameter = pPartyMemberSyncCommand->GetParameter(i);
		_ASSERT(pParameter != NULL);
		pNewCmd->AddParameter(pParameter->Clone());
	}

	RouteToAllMembers(pTarget, pNewCmd);
}

void GClientPartyRouter::NotifyLeave(GParty* pTarget, MUID uidMember)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_NOTIFY_LEAVE,
										1,
										NEW_UID(uidMember)
										);

	RouteToAllMembers(pTarget, pNewCmd);
}

void GClientPartyRouter::NotifyLeave(const GEntityPlayer* pTarget, MUID uidMember)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_NOTIFY_LEAVE,
										1,
										NEW_UID(uidMember)
										);

	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::RouteToAllMembers(const GParty* pTarget, MCommand* pCommand, const MUID& uidExceptMember)
{
	_ASSERT(pTarget != NULL);
	_ASSERT(pCommand != NULL);

	bool isNoReceiver = pCommand->IsNoReceiver();

	for(partymember_iterator it = pTarget->GetMemberBegin(); it != pTarget->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;
		if (uidExceptMember == uidMember) continue;

		if (isNoReceiver)
		{
			pCommand->SetReceiverUID(uidMember);
			isNoReceiver = false;
		}
		else
		{
			pCommand->AddAdditiveReceiver(uidMember);
		}
	}

	gsys.pCommandCenter->PostCommand(pCommand);
}

void GClientPartyRouter::ChangePublicPartySettingRes(const GParty* pParty, bool bPublicParty, wstring strPartyName)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_PUBLIC_PARTY_SETTING,
										2,
										NEW_BOOL(bPublicParty),
										NEW_WSTR(strPartyName.c_str())
										);
	RouteToAllMembers(pParty, pNewCmd);
}

void GClientPartyRouter::ChangePartyLeaderRes(const GParty* pParty, MUID uidOldLeader, MUID uidNewLeader)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LEADER,
										1,
										NEW_UID(uidNewLeader)
										);
	RouteToAllMembers(pParty, pNewCmd);
}

void GClientPartyRouter::ChangePartyLootingRuleRes(const GParty* pParty, LOOTING_RULE_DATA rule)
{
	int nCommon = rule.m_nLRC;
	int nRare = rule.m_nLRR;
	int nRareFor = rule.m_nLRRF;

	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LOOTING_RULE,
										3,
										NEW_CHAR(nCommon),
										NEW_CHAR(nRare),
										NEW_CHAR(nRareFor)
										);
	RouteToAllMembers(pParty, pNewCmd);
}

void GClientPartyRouter::ChangePartyQuestIDRes(const GParty* pParty, int nQuestID)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_QUESTID,
		1,
		NEW_INT(nQuestID)
		);
	RouteToAllMembers(pParty, pNewCmd);
}

void GClientPartyRouter::ShowInfoRes(GEntityPlayer* pTarget, const TD_PARTY& tdParty, const vector<TD_PARTY_MEMBER>& vecMembers)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_SHOW_INFO,
										2,
										NEW_BLOB(&tdParty, sizeof(tdParty), 1),
										NEW_BLOB(vecMembers)
										);
	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::CreateSinglePartyRes(GEntityPlayer* pTarget, CCommandResultTable nResult)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CREATE_SINGLE_PARTY_RES,
										1,
										NEW_INT(nResult)
										);
	pTarget->RouteToMe(pNewCmd);
}

void GClientPartyRouter::ShowMatchedPublicPartyListRes(GEntityPlayer* pTarget, CCommandResultTable nResult, const vector<TD_PARTY_MATCHING_PUBLIC_PARTY_LIST_ITEM>& vecMatchedItem, int nFilteredCount)
{
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_MATCHING_SHOW_PUBLIC_PARTY_LIST,
										3,
										NEW_INT(nResult),
										NEW_BLOB(vecMatchedItem),
										NEW_INT(nFilteredCount)
										);
	pTarget->RouteToMe(pNewCmd);
}
