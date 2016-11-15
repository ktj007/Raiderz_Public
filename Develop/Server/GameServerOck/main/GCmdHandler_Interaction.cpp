#include "StdAfx.h"
#include "GCmdHandler_Interaction.h"
#include "CCommandTable.h"
#include "GInteractionSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GPlayerInteraction.h"
#include "GNPCMgr.h"
#include "GNPCInteractor.h"

GCmdHandler_Interaction::GCmdHandler_Interaction(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_INTERACTION_INTERACTION_REQ,	OnInteraction);
	SetCmdHandler(MC_NPCINTERACTION_IELEMENT_REQ,	OnNPCInteractionElement);
	SetCmdHandler(MC_NPCINTERACTION_END_REQ,		OnNPCInteractionEnd);
}

MCommandResult GCmdHandler_Interaction::OnInteraction(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pEntityPlayer) return CR_FALSE;

	MUID nTarUID;
	vec3 vPos, vDir;
	if (pCmd->GetParameter(&nTarUID, 0, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vPos, 1, MPT_VEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vDir, 2, MPT_VEC)==false) return CR_FALSE;

	pEntityPlayer->SetPos(vPos);
	pEntityPlayer->SetFacingDir(vDir);
	pEntityPlayer->SetDir(vDir);

	gsys.pInteractionSystem->Interaction(pEntityPlayer, nTarUID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Interaction::OnNPCInteractionElement(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pEntityPlayer) return CR_FALSE;

	MUID uidNPC;
	int	nIElementID;
	if (pCmd->GetParameter(&uidNPC,			0, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nIElementID,	1, MPT_INT)==false) return CR_FALSE;

	GEntityNPC* pNPC =  gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	if (NULL == pNPC) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().InteractionElement(pEntityPlayer, pNPC, nIElementID))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pEntityPlayer);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Interaction::OnNPCInteractionEnd(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pEntityPlayer) return CR_FALSE;

	gsys.pInteractionSystem->GetNPCInteractor().End(pEntityPlayer);

	return CR_TRUE;
}