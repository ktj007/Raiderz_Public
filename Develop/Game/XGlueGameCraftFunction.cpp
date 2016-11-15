#include "stdafx.h"
#include "XGlueGameCraftFunction.h"
#include "XRecipeInfoMgr.h"
#include "XRecipeInfo.h"
#include "XPost_Craft.h"
#include "XMyPlayer.h"
#include "XEventID.h"

int XGlueGameCraftFunction::GetItemIDWithRecipe(int nRecipeID)
{
	if (nRecipeID == 0) return NULL;

	XRecipeInfo* pRecipe =  info.recipe->Get(nRecipeID);
	if (pRecipe == NULL) return NULL;

	return pRecipe->m_nProductItemID;
}

//int XGlueGameCraftFunction::GetRecipeRank(int nRecipeID)
//{
//	if (nRecipeID == 0) return 0;
//
//	XRecipeInfo* pRecipe =  info.recipe->Get(nRecipeID);
//	if (pRecipe == NULL) return 0;
//
//	return pRecipe->m_nRank;
//}

//int XGlueGameCraftFunction::GetOptionToolOfRecipe( int nRecipeID )
//{
//	if (nRecipeID == 0) return 0;
//
//	XRecipeInfo* pRecipe =  info.recipe->Get(nRecipeID);
//	if (pRecipe == NULL) return 0;
//
//	return pRecipe->m_nOptionToolItemID;
//}

const char* XGlueGameCraftFunction::GetMatOfRecipe( int nRecipeID )
{
	if (nRecipeID == 0) return 0;

	XRecipeInfo* pRecipe =  info.recipe->Get(nRecipeID);
	if (pRecipe == NULL) return 0;

	char text[256] ="";
	gvar.Temp.szTemp = "";

	for each (RecipeMaterial material in pRecipe->m_vecMaterial)
	{
		sprintf_s(text, 256, "%d,%d", material.m_nItemID, material.m_nAmount);
		gvar.Temp.szTemp = text;
	}

	return 	gvar.Temp.szTemp.c_str();
}

//const char* XGlueGameCraftFunction::GetSubMatOfRecipe( int nRecipeID )
//{
//	if (nRecipeID == 0) return 0;
//
//	XRecipeInfo* pRecipe =  info.recipe->Get(nRecipeID);
//	if (pRecipe == NULL) return 0;
//
//	char text[256] ="";
//	gvar.Temp.szTemp = "";
//	if (pRecipe->m_nSubMat1Count != 0)
//	{
//		sprintf_s(text, 256, "%d,%d", pRecipe->m_nSubMat1ItemID,pRecipe->m_nSubMat1Count);
//		gvar.Temp.szTemp = text;
//	}
//	if (pRecipe->m_nSubMat2Count != 0)
//	{
//		sprintf_s(text, 256, ",%d,%d", pRecipe->m_nSubMat2ItemID,pRecipe->m_nSubMat2Count);
//		gvar.Temp.szTemp += text;
//	}
//
//	return gvar.Temp.szTemp.c_str();
//}

int XGlueGameCraftFunction::GetCraftableItemQuantity(int nRecipeID)
{
	if (nRecipeID == 0) return NULL;

	return info.recipe->GetCraftableItemQuantity(nRecipeID);
}

//float XGlueGameCraftFunction::GetCraftableItemRate(int nRecipeID, float fToolOptionRate)
//{
//	if (nRecipeID == 0) return 0.f;
//
//	return info.recipe->GetCraftableItemRate(nRecipeID, gvar.MyInfo.EquipmentSlot.GetSumOfCraftOptionRate(), fToolOptionRate);
//}

void XGlueGameCraftFunction::StartCraftingItem( int nRecipeID , int nCount)
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return;

	if (nRecipeID == 0 || nCount == 0) return;
	XRecipeInfo* pRecipe =  info.recipe->Get(nRecipeID);
	if (pRecipe == NULL) return;

//	CRAFTING_RECIPE_COUNT data;
//	data.nRecipeID = nRecipeID;
//	data.nCount = nCount;

//	XEvent evt;
//	evt.m_nID = XEVTL_CRAFTING_REQ;
//	evt.m_pData = &data;
//	gg.omgr->PostEvent(gvar.MyInfo.MyUID, evt);
}

void XGlueGameCraftFunction::CraftingCancelReq()
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return;

	//XEvent evt;
	//evt.m_nID = XEVTL_CRAFTING_CANCEL_REQ;
	//gg.omgr->PostEvent(gvar.MyInfo.MyUID, evt);
}
