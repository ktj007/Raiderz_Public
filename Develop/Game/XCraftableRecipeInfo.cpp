#include "StdAfx.h"
#include "XCraftableRecipeInfo.h"
#include "XRecipeInfo.h"
#include "XRecipeInfoMgr.h"


XRecipeInfo* XCraftableRecipeInfo::GetRecipeInfoByIndex(int nIndex)
{
	if (nIndex < 0 || (int)m_vecRecipeInfo.size() <= nIndex) return NULL;

	return m_vecRecipeInfo[nIndex];
}

int XCraftableRecipeInfo::GetProductItemIDByIndex( int nIndex )
{
	XRecipeInfo* pRecipeInfo = GetRecipeInfoByIndex(nIndex);
	if (pRecipeInfo == NULL) return 0;

	return pRecipeInfo->m_nProductItemID;
}

int XCraftableRecipeInfo::GetPriceByIndex(int nIndex)
{
	XRecipeInfo* pRecipeInfo = GetRecipeInfoByIndex(nIndex);
	if (pRecipeInfo == NULL) return 0;

	return pRecipeInfo->m_nPrice * m_fMakeMod;
}

MWLua::table XCraftableRecipeInfo::GetMaterialInfoTable(int nRecipeID)
{
	MWLua::table t(global.script->GetLua()->GetState());

	XRecipeInfo* pRecipeInfo = info.recipe->Get(nRecipeID);
	if (pRecipeInfo == NULL)	return t;

	char buffer[32];

	for each (RecipeMaterial material in pRecipeInfo->m_vecMaterial)
	{
		MWLua::table item1(global.script->GetLua()->GetState());
		item1.set("id"		, material.m_nItemID);
		item1.set("amount"	, material.m_nAmount);
		sprintf(buffer,"%d", 1);
		t.set(buffer, item1);
	}	

	return t;
}

MWLua::table XCraftableRecipeInfo::GetMaterialInfoTableByIndex(int nIndex)
{
	if (nIndex < 0 || (int)m_vecRecipe.size() <= nIndex)
	{
		MWLua::table t(global.script->GetLua()->GetState());
		return t;
	}

	return GetMaterialInfoTable(m_vecRecipe[nIndex].m_nRecipeID);
}

bool XCraftableRecipeInfo::IsCraftableItem( int nIndex )
{
	if (nIndex < 0 || (int)m_vecRecipe.size() <= nIndex) return false;

	return m_vecRecipe[nIndex].m_bMakable;
}