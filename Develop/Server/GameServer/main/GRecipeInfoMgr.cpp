#include "StdAfx.h"
#include "GRecipeInfoMgr.h"
#include "GRecipeInfo.h"

GRecipeInfoMgr::GRecipeInfoMgr(void)
{
}

GRecipeInfoMgr::~GRecipeInfoMgr(void)
{
}

bool GRecipeInfoMgr::ParseRecipe(MXmlElement* pElement, MXml* pXml, CSRecipeInfo* pInfo)
{
	if (false == __super::ParseRecipe(pElement, pXml, pInfo)) return false;

	GRecipeInfo* pGInfo = static_cast<GRecipeInfo*>(pInfo);

	return true;
}

CSRecipeInfo* GRecipeInfoMgr::New()
{
	return new GRecipeInfo();
}

GRecipeInfo* GRecipeInfoMgr::Get(int nID)
{
	return static_cast<GRecipeInfo*>(__super::Get(nID));
}


