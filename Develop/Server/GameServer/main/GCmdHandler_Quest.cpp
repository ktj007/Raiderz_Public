#include "StdAfx.h"
#include "GCmdHandler_Quest.h"
#include "CCommandTable.h"
#include "MiNetCommon.h"
#include "GQuestSystem.h"
#include "GInteractionSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerInteraction.h"
#include "GMessageID.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GNPCInteractor.h"
#include "GQuestInfoMgr.h"
#include "GQuestGiver.h"
#include "GQuestRewarder.h"
#include "GQuestSharer.h"
#include "GQuestCanceler.h"
#include "GQObjInteractor.h"
#include "CCommandResultTable.h"


GCmdHandler_Quest::GCmdHandler_Quest(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_QUEST_GIVE_REQ,		OnRequestQuestGive);
	SetCmdHandler(MC_QUEST_REWARD_REQ,		OnRequestQuestReward);
	SetCmdHandler(MC_QUEST_CANCEL_REQ,		OnRequestQuestCancel);
	SetCmdHandler(MC_QUEST_SHARE_REQ,		OnRequestQuestShare);
	SetCmdHandler(MC_QUEST_SHARE_GIVE_REQ,	OnRequestQuestShareGive);

	SetCmdHandler(MC_QUEST_INTERACT_QOBJECTIVE_CANCEL_REQ,OnInteractQObjectiveCancel);
}

MCommandResult GCmdHandler_Quest::OnRequestQuestGive(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nQuestID;	
	if (pCmd->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_FALSE;

	bool bChallengeQuest = gmgr.pQuestInfoMgr->IsChallengerQuestID(nQuestID);
	INTERACTION_TYPE nIEType = (false == bChallengeQuest) ? IT_QUEST_BEGIN : IT_CHALLENGERQUEST;;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, nIEType))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	if (false == gsys.pQuestSystem->GetGiver().Give(pPlayer, nQuestID))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Quest::OnRequestQuestReward(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidNPC;
	int nQuestID;
	int nSelectedReward;
	if (pCmd->GetParameter(&uidNPC,		0, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nQuestID,	1, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSelectedReward,	2, MPT_INT)==false) return CR_FALSE;

	GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
	if (NULL == pNPC) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_QUEST_END))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	if (false == gsys.pQuestSystem->GetRewarder().Reward(pPlayer, nQuestID, nSelectedReward))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Quest::OnRequestQuestCancel(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_FALSE;

	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer, nQuestID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Quest::OnRequestQuestShare(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_FALSE;

	gsys.pQuestSystem->GetSharer().Share(pPlayer, nQuestID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Quest::OnRequestQuestShareGive(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nQuestID;	
	if (pCmd->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_FALSE;
	
	if (false == gsys.pQuestSystem->GetSharer().IsSharableQuest(pPlayer, nQuestID))
	{
		pPlayer->FailAndRouteAdviceMsg(CR_FAIL_QUEST_NOT_TAKABLE_SHARE_QUEST);
		return CR_FALSE;
	}

	if (false == gsys.pQuestSystem->GetGiver().Give(pPlayer, nQuestID))
	{
		pPlayer->FailAndRouteAdviceMsg(CR_FAIL_QUEST_NOT_TAKABLE_SHARE_QUEST);
		return CR_FALSE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Quest::OnInteractQObjectiveCancel(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	INTERACTION_TYPE nIEType = pPlayer->GetInteraction().GetSelectedIElementInfo().m_nType;
	if (IT_QUEST_INTERACT_IMMORTAL != nIEType && IT_QUEST_INTERACT_MORTAL != nIEType) return CR_FALSE;
	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, nIEType)) return CR_FALSE;

	gsys.pQuestSystem->GetQObjInteractor().Cancel(pPlayer, pPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct());
	gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);

	return CR_TRUE;
}