#include "StdAfx.h"
#include "GRecipeInfoValidator.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GValidateLogger.h"
#include "GRecipeInfoMgr.h"
#include "GRecipeInfo.h"

bool GRecipeInfoValidator::Check()
{
	if (gmgr.pRecipeInfoMgr == NULL)	return true;

	_VLI(FILENAME_RECIPE);

	MAP_CSRECIPEINFO& mapCSRecipInfo = gmgr.pRecipeInfoMgr->GetContainer();
	for each (pair<int, CSRecipeInfo*> pairCSRecipeInfo in mapCSRecipInfo)
	{
		GRecipeInfo* pRecipeInfo = static_cast<GRecipeInfo*>(pairCSRecipeInfo.second);
		_ASSERT(NULL != pRecipeInfo);
		if (NULL == pRecipeInfo) continue;

		CheckRecipe(pRecipeInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}


void GRecipeInfoValidator::CheckRecipe(GRecipeInfo* pRecipeInfo)
{
	_VLE(_T(RECIPE_XML_TAG_RECIPE));
	_VLA(_T(RECIPE_XML_ATTR_ID), pRecipeInfo->m_nID);
	_VLP;

	_VLOGGER->ValidateID(pRecipeInfo->m_nID);
	_VLOGGER->ValidateItemID(pRecipeInfo->m_nProductItemID, _T(RECIPE_XML_ATTR_ITEM));

	int nMaterialSize = pRecipeInfo->m_vecMaterial.size();

	if (MAX_RECIPE_MATERIAL_TYPE < nMaterialSize)
	{
		_VLOGGER->Log(L"재료 아이템의 종류는 %d개를 초과 할 수 없습니다.", MAX_RECIPE_MATERIAL_TYPE);
	}

	int i=1;
	for each(const RecipeMaterial& material in pRecipeInfo->m_vecMaterial)
	{
		 wchar_t  szItemlD[16];
		 wchar_t  szItemAmount[16];

		 swprintf_s(szItemlD, L"%s%d", _T(RECIPE_XML_ATTR_MAT), i);
		 swprintf_s(szItemAmount, L"%s%d_%s", _T(RECIPE_XML_ATTR_MAT), i, _T(RECIPE_XML_ATTR_AMOUNT));

		_VLOGGER->ValidateItemID(material.m_nItemID, szItemlD);
		_VLOGGER->ValidateMinusORZero(material.m_nAmount, szItemAmount);

		i++;
	}
	
	_VLOGGER->ValidateMinus(pRecipeInfo->m_nPrice, _T(RECIPE_XML_ATTR_PRICE));

	if (0 != pRecipeInfo->m_nConditionID)
	{
		_VLOGGER->ValidateConditionID(pRecipeInfo->m_nConditionID, _T(RECIPE_XML_ATTR_PRICE));
	}
}