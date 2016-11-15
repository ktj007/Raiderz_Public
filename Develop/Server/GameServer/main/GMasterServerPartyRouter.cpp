#include "stdafx.h"
#include "GMasterServerPartyRouter.h"
#include "GCommand.h"
#include "CCommandTable_Master.h"
#include "CTransData.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"

GMasterServerPartyRouter::GMasterServerPartyRouter()
{
	// do nothing
}

GMasterServerPartyRouter::~GMasterServerPartyRouter()
{
	// do nothing
}

void GMasterServerPartyRouter::InviteReq(MUID uidTargetPlayer, MUID uidRequestPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_INVITE_REQ,
																	2,
																	NEW_UID(uidTargetPlayer),
																	NEW_UID(uidRequestPlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::AcceptRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_ACCEPT_RES,
																	3,
																	NEW_UID(uidRequestPlayer),
																	NEW_UID(uidTargetPlayer),
																	NEW_INT(nResult)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::UpdateParty(MUID uidParty, const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_UPDATE,
																	3,
																	NEW_UID(uidParty),
																	NEW_BLOB(&tdPartyMember, sizeof(tdPartyMember), 1),
																	NEW_BLOB(vecBuff)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::UpdateMember(MUID uidParty, const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_UPDATE_MEMBER,
																	3,
																	NEW_UID(uidParty),
																	NEW_BLOB(&tdPartyMember, sizeof(tdPartyMember), 1),
																	NEW_BLOB(vecBuff)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::AddQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_ADD_QUEST,
																	3,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember),
																	NEW_BLOB(vecQuestID)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::AddField(MUID uidParty, MUID uidMember, MUID uidFieldGroup)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_ADD_FIELD,
																	3,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember),
																	NEW_UID(uidFieldGroup)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::UpdateQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_UPDATE_QUEST_RUNNER,
																	4,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember),
																	NEW_UID(uidField),
																	NEW_BLOB(vecQuestID)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::RemoveQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_REMOVE_QUEST,
																	3,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember),
																	NEW_BLOB(vecQuestID)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);																	
}

void GMasterServerPartyRouter::RemoveField(MUID uidParty, MUID uidMember, int nFieldGroupID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_REMOVE_FIELD,
																	3,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember),
																	NEW_INT(nFieldGroupID)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::RemoveFieldSelf(MUID uidParty, int nFieldGroupID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_REMOVE_FIELD_SELF,
																	2,
																	NEW_UID(uidParty),
																	NEW_INT(nFieldGroupID)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::EraseQuestRunner(MUID uidParty, MUID uidMember)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_ERASE_QUEST_RUNNER,
																	2,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::LeaveReq(MUID uidParty, MUID uidLeavePlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_LEAVE_REQ,
																	2,
																	NEW_UID(uidParty),
																	NEW_UID(uidLeavePlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::KickReq(MUID uidParty, MUID uidRequestPlayer, MUID uidTargetPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_KICK_REQ,
																	3,
																	NEW_UID(uidParty),
																	NEW_UID(uidRequestPlayer),
																	NEW_UID(uidTargetPlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::DoOffline(MUID uidParty, MUID uidMember)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_DO_OFFLINE,
																	2,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::DoOnline(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_DO_ONLINE,
																	3,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember),
																	NEW_UID(uidOffline)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::JoinInviteReq(MUID uidParty, MUID uidRequestPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_JOIN_INVITE_REQ,
																	2,
																	NEW_UID(uidParty),
																	NEW_UID(uidRequestPlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::JoinAcceptRes(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_JOIN_ACCEPT_RES,
																	4,
																	NEW_UID(uidParty),
																	NEW_UID(uidLeader),
																	NEW_UID(uidRequestPlayer),
																	NEW_INT(nResult)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::MoveServer(MUID uidParty, MUID uidMember)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_MOVE_SERVER,
																	2,
																	NEW_UID(uidParty),
																	NEW_UID(uidMember)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::CreateSinglePartyReq(MUID uidRequestPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_CREATE_SINGLE_REQ,
																	1,
																	NEW_UID(uidRequestPlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::PartyInfoAllReq(void)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_INFO_ALL_REQ);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::ChangePartyNameReq(MUID uidParty, MUID uidLeader, wstring strName)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_CHANGE_NAME_REQ,
																3,
																NEW_UID(uidParty),
																NEW_UID(uidLeader),
																NEW_WSTR(strName.c_str())
																);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::ChangePartyLeaderReq(MUID uidParty, MUID uidLeader, MUID uidNewLeader)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_CHANGE_LEADER_REQ,
																3,
																NEW_UID(uidParty),
																NEW_UID(uidLeader),
																NEW_UID(uidNewLeader)
																);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::ChangePartyLootingRuleReq(MUID uidParty, MUID uidLeader, LOOTING_RULE_DATA rule)
{
	int8 nCommon = rule.m_nLRC;
	int8 nRare = rule.m_nLRR;
	int8 nRareFor = rule.m_nLRRF;

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_CHANGE_LOOTING_RULE_REQ,
																5,
																NEW_UID(uidParty),
																NEW_UID(uidLeader),
																NEW_CHAR(nCommon),
																NEW_CHAR(nRare),
																NEW_CHAR(nRareFor)
																);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::ChangeQuestIDReq( MUID uidParty, MUID uidLeader, int nQuestID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_CHANGE_QUESTID_REQ,
		3,
		NEW_UID(uidParty),
		NEW_UID(uidLeader),
		NEW_INT(nQuestID)
		);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::FixedPartyLogOn(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_FIXED_LOG_ON,
		3,
		NEW_UID(uidParty),
		NEW_UID(uidMember),
		NEW_UID(uidOffline)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);	
}

void GMasterServerPartyRouter::CreateAutoPartyReq( QuestID nQuestID, const vector<MUID> vecPromotersUID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_CREATE_AUTO_PARTY_REQ,
		2,
		NEW_BLOB(vecPromotersUID),
		NEW_INT(nQuestID)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPartyRouter::JoinAutoPartyReq(MUID uidParty, MUID uidPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PARTY_JOIN_ATUTO_PARTY_REQ,
																	2,
																	NEW_UID(uidParty),
																	NEW_UID(uidPlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}
