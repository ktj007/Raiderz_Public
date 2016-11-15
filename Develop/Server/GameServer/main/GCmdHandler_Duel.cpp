#include "StdAfx.h"
#include "GCmdHandler_Duel.h"
#include "CCommandTable.h"
#include "GDuelMgr.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GSharedField.h"
#include "GParty.h"
#include "GPartySystem.h"

GCmdHandler_Duel::GCmdHandler_Duel(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_DUEL_REQUEST,				OnDuelRequest);
	SetCmdHandler(MC_DUEL_PARTY_REQUEST,		OnDuelPartyRequest);
	SetCmdHandler(MC_DUEL_QUESTION_RESPOND,		OnDuelQuestionRespond);
}

MCommandResult GCmdHandler_Duel::OnDuelRequest(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	GField* pField = pPlayer->GetField();
	if (!pField)	
		return CR_FALSE;

	MUID uidTargetPlayer;
	if (pCmd->GetParameter(&uidTargetPlayer,	0, MPT_UID)==false) return CR_FALSE;

	pPlayer->GetField()->GetDuelMgr()->DuelRequest(pPlayer, uidTargetPlayer);
	pPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Duel::OnDuelPartyRequest(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (!pPlayer) 
		return CR_FALSE;
	GField* pField = pPlayer->GetField();
	if (!pField)	
		return CR_FALSE;

	MUID uidTargetPlayer;
	if (pCmd->GetParameter(&uidTargetPlayer,	0, MPT_UID)==false) return CR_FALSE;

	GEntityPlayer* pTarget = pField->FindPlayer(uidTargetPlayer);
	if (!pTarget) 
		return CR_FALSE;

	if (pField != pTarget->GetField())	
		return CR_FALSE;	// 대상자와 다른 필드에 있음
	
	if (!pTarget->IsPartyLeader())
		return CR_FALSE;	// 파티 리더가 아님

	vector<GEntityPlayer*> vecFighters1;
	if (pPlayer->HasParty())
	{
		GParty* pParty = gsys.pPartySystem->FindParty(pPlayer->GetPartyUID());
		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); ++it)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pMember = pField->FindPlayer(uidMember);
			if (!pMember)
				continue; // 같은 필드에 있지 않음

			vecFighters1.push_back(pMember);
		}
	}
	else
		vecFighters1.push_back(pPlayer);


	vector<GEntityPlayer*> vecFighters2;
	if (pTarget->HasParty())
	{
		GParty* pParty = gsys.pPartySystem->FindParty(pTarget->GetPartyUID());
		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); ++it)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pMember = pField->FindPlayer(uidMember);
			if (!pMember)
				continue; // 같은 필드에 있지 않음

			vecFighters2.push_back(pMember);
		}
	}
	else
		vecFighters2.push_back(pTarget);



	pPlayer->GetField()->GetDuelMgr()->DuelRequest(vecFighters1, vecFighters2);
	pPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Duel::OnDuelQuestionRespond(MCommand* pCmd, MCommandHandler* pHandler)
{

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	bool bAnswer;
	if (pCmd->GetParameter(&bAnswer,	0, MPT_BOOL)==false) return CR_FALSE;

	if (!pPlayer->GetField())
		CR_FALSE;

	pPlayer->GetField()->GetDuelMgr()->DuelQuestionRespond(pPlayer->GetDuel(), bAnswer);
	pPlayer->OnDoSomething();

	return CR_TRUE;
}
