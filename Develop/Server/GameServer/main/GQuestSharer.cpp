#include "StdAfx.h"
#include "GQuestSharer.h"
#include "GEntityPlayer.h"
#include "CSDef_Quest.h"
#include "GQuestInfo.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GPlayerQuests.h"
#include "GParty.h"
#include "GPartySystem.h"
#include "GPlayerObjectManager.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GConditionsSystem.h"
#include "CCommandResultTable.h"


bool GQuestSharer::Share(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID_RET(pPlayer, false);
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo, false);
	GParty* pParty = gsys.pPartySystem->FindParty(pPlayer->GetPartyUID());
	if (NULL == pParty) return false;

	if (false == Check(pPlayer, nQuestID)) return false;	

	Route(pPlayer, pParty, pQuestInfo);

	return true;
}

bool GQuestSharer::Check( GEntityPlayer* pPlayer, int nQuestID )
{
	if (false == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return false;

	return true;
}

void GQuestSharer::Route( GEntityPlayer* pPlayer, GParty* pParty, GQuestInfo* pQuestInfo )
{
	VALID(pPlayer);
	VALID(pParty);
	VALID(pQuestInfo);

	vector<TD_QUEST_SHARE_RESULT> vecResult;
	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;
		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
		if (NULL == pMember)	continue;
		if (pPlayer == pMember) continue;		

		QUEST_SHARE_RESULT nResult = SelectShareResult(pPlayer, pMember, pQuestInfo);
		MCommand* pNewCmd = MakeNewCommand(MC_QUEST_SHARE_REQ_RELAY,
			1,
			NEW_SINGLE_BLOB(TD_QUEST_SHARE_RESULT(pPlayer->GetUID(), pQuestInfo->nID, nResult)));
		pMember->RouteToMe(pNewCmd);

		vecResult.push_back(TD_QUEST_SHARE_RESULT(pMember->GetUID(), pQuestInfo->nID, nResult));
	}

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_SHARE,
		1,
		NEW_BLOB(vecResult));

	pPlayer->RouteToMe(pNewCmd);
}

QUEST_SHARE_RESULT GQuestSharer::SelectShareResult( GEntityPlayer* pPlayer, GEntityPlayer* pMember, GQuestInfo* pQuestInfo )
{
	QUEST_SHARE_RESULT nResult = QSR_SUCCESS;	
	if (GConst::QUEST_SHARE_DISTANCE < pPlayer->DistanceTo(pMember))
	{
		nResult = QSR_FAIL_DISTANCE;
	}
	else if (false == gsys.pConditionsSystem->Check(pMember, pQuestInfo->nShareConditionsID))
	{
		nResult = QSR_FAIL_CONDITION;
	}
	else if (true == pMember->GetQuests().IsDoingQuest(pQuestInfo->nID) || 
			 true == pMember->GetQuests().IsCompleteQuest(pQuestInfo->nID))
	{
		nResult = QSR_FAIL_EXIST;
	}
	else if (false == pQuestInfo->bRepeatable && pMember->GetQuests().IsDoneQuest(pQuestInfo->nID))
	{
		nResult = QSR_FAIL_DONE;
	}

	return nResult;
}


bool GQuestSharer::IsSharableQuest(GEntityPlayer* pPlayer, int nQuestID)
{
	if (NULL == pPlayer) return false;

	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return false;

	if (QSHAT_NONE != pQuestInfo->nShareType && 
		true == HaveQuestSharableMember(pPlayer, nQuestID))
	{
		if (true == gsys.pConditionsSystem->Check(pPlayer, pQuestInfo->nShareConditionsID))
		{
			return true;
		}
	}

	return false;
}

bool GQuestSharer::HaveQuestSharableMember(GEntityPlayer* pPlayer, int nQuestID)
{
	if (NULL == pPlayer) return false;

	if (!gsys.pPartySystem->IsPartyMember(pPlayer)) return false;

	MUID uidParty = pPlayer->GetPartyUID();
	GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
	_ASSERT(pParty != NULL);

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;

		GEntityPlayer* pMember = pPlayer->FindPlayer(uidMember);
		if (NULL == pMember) continue;
		if (false == pMember->GetQuests().IsDoingQuest(nQuestID)) continue;
		if (GConst::QUEST_SHARE_DISTANCE < pMember->DistanceTo(pPlayer)) continue;

		return true;		
	}

	return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_TAKABLE_QUEST);
}