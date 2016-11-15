#include "StdAfx.h"
#include "GCraftInfoValidator.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GValidateLogger.h"
#include "GCraftInfoMgr.h"
#include "GCraftInfo.h"

bool GCraftInfoValidator::Check()
{
	if (gmgr.pRecipeInfoMgr == NULL)	return true;

	_VLI(FILENAME_CRAFT);

	MAP_CRAFTINFO& mapCraftInfo = gmgr.pCraftInfoMgr->GetContainer();
	for each (pair<int, GCraftInfo*> pairCraftInfo in mapCraftInfo)
	{
		GCraftInfo* pCraftInfo = pairCraftInfo.second;
		_ASSERT(NULL != pCraftInfo);
		if (NULL == pCraftInfo) continue;

		CheckCraft(pCraftInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GCraftInfoValidator::CheckCraft( GCraftInfo* pCraftInfo )
{
	_VLE(_T(CRAFT_XML_TAG_CRAFT));
	_VLA(_T(CRAFT_XML_ATTR_ID), pCraftInfo->m_nID);
	_VLP;

	_VLOGGER->ValidateID(pCraftInfo->m_nID);
	_VLOGGER->ValidateDialogID(pCraftInfo->m_nDialogID, _T(CRAFT_XML_ATTR_DIALOG));
	_VLOGGER->ValidateMinusORZero(pCraftInfo->m_fBaseMakeMod, _T(CRAFT_XML_ATTR_MAKEMOD));

	if (0 != pCraftInfo->m_nFactionID)
	{
		_VLOGGER->ValidateFactionID(pCraftInfo->m_nFactionID, _T(CRAFT_XML_ATTR_FACTIONID));
	}	

	for each (int nRecipeID in pCraftInfo->m_setRecipeID)
	{
		CheckRecipe(nRecipeID);		
	}
}

void GCraftInfoValidator::CheckRecipe(int nRecipeID)
{
	_VLE(_T(CRAFT_XML_TAG_RECIPE));
	_VLP;

	_VLOGGER->ValidateRecipeID(nRecipeID, _T(CRAFT_XML_ATTR_ID));
}