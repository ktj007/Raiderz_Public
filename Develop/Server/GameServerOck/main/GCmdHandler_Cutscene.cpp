#include "StdAfx.h"
#include "GCmdHandler_Cutscene.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GPlayerCutscene.h"
#include "CCommandTable.h"

GCmdHandler_Cutscene::GCmdHandler_Cutscene(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CUTSCENE_BEGIN_REQ,	OnBeginReq);
	SetCmdHandler(MC_CUTSCENE_END_REQ,		OnEndReq);
}

MCommandResult GCmdHandler_Cutscene::OnBeginReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nCutsceneID;
	if (false == pCmd->GetParameter(&nCutsceneID, 0, MPT_INT)) return CR_FALSE;

	//pPlayer->BeginCutscene(nCutsceneID);
	pPlayer->GetCutscene().BeginCutscene(nCutsceneID);

	return CR_TRUE;
}

 
MCommandResult GCmdHandler_Cutscene::OnEndReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nCutsceneID;
	if (false == pCmd->GetParameter(&nCutsceneID, 0, MPT_INT)) return CR_FALSE;

	pPlayer->GetCutscene().EndCutscene(nCutsceneID);	

	return CR_TRUE;
}


