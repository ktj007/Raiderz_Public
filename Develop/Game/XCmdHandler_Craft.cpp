#include "StdAfx.h"
#include "XCmdHandler_Craft.h"
#include "XRecipeInfoMgr.h"
#include "XRecipeInfo.h"
#include "XMyPlayer.h"
#include "XController.h"
#include "XInteractionInfo.h"
#include "XPost_Craft.h"
#include "XSoundNonPlayer.h"
#include "XEffectInvoker.h"
#include "XItemManager.h"
#include "XStrings.h"

XCmdHandler_Craft::XCmdHandler_Craft(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CRAFT_SHOW,			OnShow);
	SetCmdHandler(MC_CRAFT_MAKE,			OnMake);
	SetCmdHandler(MC_CRAFT_RECIPE_INSERT,	OnRecipeInsert);
	SetCmdHandler(MC_CRAFT_RECIPE_DELETE,	OnRecipeDelete);
}

MCommandResult XCmdHandler_Craft::OnShow(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.MyInfo.CraftableRecipeInfo.Clear();

	vector<TD_CRAFT_RECIPE>& vecRecipeID = gvar.MyInfo.CraftableRecipeInfo.GetRecipeList();
	float& fMakeMod =  gvar.MyInfo.CraftableRecipeInfo.GetMakeMod();
	if (pCommand->GetBlob(vecRecipeID,	0)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fMakeMod,	1, MPT_FLOAT)==false) return CR_ERROR;
	
	vector<XRecipeInfo*>& vecRecipeInfo = gvar.MyInfo.CraftableRecipeInfo.GetRecipeInfoList();
	for each (TD_CRAFT_RECIPE tdCraftRecipe in vecRecipeID)
	{
		XRecipeInfo* pRecipeInfo = info.recipe->Get(tdCraftRecipe.m_nRecipeID);
		if (pRecipeInfo == NULL)
		{
			_ASSERT(0);
			continue;
		}

		vecRecipeInfo.push_back(pRecipeInfo);
	}

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CRAFTING", "BEGIN");
	}

	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionInteraction(XGetInteractionInfo().InteractionTargetUID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Craft::OnMake(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nRecipeID;
	if (pCommand->GetParameter(&nRecipeID,	0, MPT_INT)==false) return CR_ERROR;

	// 아이템 만들기 완료
	XGetSoundNonPlayer()->PlayVoice( XGetInteractionInfo().InteractionTargetUID, "CRAFTDONE");

	XRecipeInfo* pRecipeInfo = info.recipe->Get( nRecipeID);
	if ( pRecipeInfo)
	{
		XItemData* pItemData = info.item->GetItemData(pRecipeInfo->m_nProductItemID);
		if (pItemData)
		{
			wstring strMsg = CSFormatString::Format( XGetStr( L"ITEM_CRAFTED"), FSParam(pItemData->GetName()));
			global.ui->OnPresentationLower(strMsg, PRESENTATION_ICON_EXCLAMATION);
		}
	}

	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(gvar.Game.pMyPlayer, wstring(L"buff_dye_done"), wstring(), wstring());

	//레시피 삭제 옵션
	if (XGETCFG_GAME_RECIPE_REMOVE == true)
	{
		XPostCraft_RecipeDelete(nRecipeID);

		// 알림창
		if (global.script)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "RECIPE", "DELETE", nRecipeID);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Craft::OnRecipeInsert(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nRecipeID;
	if (pCommand->GetParameter(&nRecipeID,	0, MPT_INT)==false) return CR_ERROR;

	XRecipeInfo* pRecipeInfo = info.recipe->Get(nRecipeID);
	if (pRecipeInfo == NULL) return CR_TRUE;

	gvar.MyInfo.nRecievedRecipe++;

	gvar.MyInfo.RecipeInfo.push_back(nRecipeID);

	global.script->GetGlueGameEvent().OnGameEvent( "RECIPE", "REFRESH");

	return CR_TRUE;


}

MCommandResult XCmdHandler_Craft::OnRecipeDelete(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nRecipeID;
	if (pCommand->GetParameter(&nRecipeID,	0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.RecipeInfo.erase(nRecipeID);

	global.script->GetGlueGameEvent().OnGameEvent( "RECIPE", "REFRESH");

	return CR_TRUE;
}