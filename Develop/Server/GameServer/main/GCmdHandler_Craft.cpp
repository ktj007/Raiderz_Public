#include "StdAfx.h"
#include "GCmdHandler_Craft.h"
#include "CCommandTable.h"
#include "GCraftSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GNPCInteractionInfo.h"
#include "GEntityPlayer.h"
#include "GPlayerInteraction.h"

GCmdHandler_Craft::GCmdHandler_Craft(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CRAFT_MAKE_REQ,			OnRequestMake);
	SetCmdHandler(MC_CRAFT_RECIPE_INSERT_REQ,	OnRequestRecipeInsert);
	SetCmdHandler(MC_CRAFT_RECIPE_DELETE_REQ,	OnRequestRecipeDelete);
}

MCommandResult GCmdHandler_Craft::OnRequestMake(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nRecipeID;
	if (pCmd->GetParameter(&nRecipeID, 0, MPT_INT)==false) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_CRAFT)) return CR_FALSE;

	gsys.pCraftSystem->Make(pPlayer, pPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct(), nRecipeID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Craft::OnRequestRecipeInsert(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nRecipeID;
	if (pCmd->GetParameter(&nRecipeID, 0, MPT_INT)==false) return CR_FALSE;	

	gsys.pCraftSystem->InsertRecipe(pPlayer, nRecipeID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Craft::OnRequestRecipeDelete(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nRecipeID;
	if (pCmd->GetParameter(&nRecipeID, 0, MPT_INT)==false) return CR_FALSE;

	gsys.pCraftSystem->DeleteRecipe(pPlayer, nRecipeID);

	return CR_TRUE;
}