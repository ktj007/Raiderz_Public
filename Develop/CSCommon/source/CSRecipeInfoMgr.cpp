#include "StdAfx.h"
#include "CSRecipeInfoMgr.h"
#include "CSRecipeInfo.h"
#include "algorithm"
#include "CSStrings.h"
#include "MTypes.h"
#include "ASTLUtil.h"
#include "MLocale.h"


CSRecipeInfoMgr::CSRecipeInfoMgr()
{
}

CSRecipeInfoMgr::~CSRecipeInfoMgr()
{
	Clear();
}

void CSRecipeInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapRecipeInfo);
}

bool CSRecipeInfoMgr::Load(const TCHAR* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(RECIPE_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), RECIPE_XML_TAG_RECIPE))
		{
			CSRecipeInfo* pNewInfo = New();
			if (true == ParseRecipe(pElement, &xml, pNewInfo))
			{
				Insert(pNewInfo);
			}
			else
			{
				SAFE_DELETE(pNewInfo);
			}
		}
	}

	Cooking();

	return true;
}

bool CSRecipeInfoMgr::ParseRecipe(MXmlElement* pElement, MXml* pXml, CSRecipeInfo* pInfo)
{
	int nID;
	_Attribute(&nID, pElement, RECIPE_XML_ATTR_ID);

	CSRecipeInfo* pExistInfo = Get(nID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo)
	{
		return false;
	}

	pInfo->m_nID = nID;

	_Attribute(&pInfo->m_nProductItemID, pElement, RECIPE_XML_ATTR_ITEM);
	_Attribute(&pInfo->m_nAmount, pElement, RECIPE_XML_ATTR_AMOUNT);

	for (int i = 1; MAX_RECIPE_MATERIAL_TYPE >= i; i++)
	{
		char szItemlD[16];
		char szItemAmount[16];

		sprintf_s(szItemlD, "%s%d", RECIPE_XML_ATTR_MAT, i);
		sprintf_s(szItemAmount, "%s%d_%s", RECIPE_XML_ATTR_MAT, i, RECIPE_XML_ATTR_AMOUNT);

		int nItemID;
		int nItemAmount;
		if (false == _Attribute(nItemID, pElement, szItemlD)) continue;
		if (false == _Attribute(nItemAmount, pElement, szItemAmount)) continue;

		pInfo->m_vecMaterial.push_back(RecipeMaterial(nItemID, nItemAmount));
		pInfo->m_setMaterialItemID.insert(nItemID);
	}

	_Attribute(&pInfo->m_nPrice, pElement, RECIPE_XML_ATTR_PRICE);
	_Attribute(&pInfo->m_nConditionID, pElement, RECIPE_XML_ATTR_CONDITION);

	return true;
}

void CSRecipeInfoMgr::Insert(CSRecipeInfo* pInfo)
{
	if (NULL == pInfo) return;

	if (NULL != Get(pInfo->m_nID))
	{
		_ASSERT(0);
		return;
	}

	m_mapRecipeInfo.insert(MAP_CSRECIPEINFO::value_type(pInfo->m_nID, pInfo));
}

CSRecipeInfo* CSRecipeInfoMgr::Get(int nID)
{
	MAP_CSRECIPEINFO::iterator itor = m_mapRecipeInfo.find(nID);
	if (m_mapRecipeInfo.end() == itor) return NULL;

	return (*itor).second;
}

void CSRecipeInfoMgr::Cooking()
{
	for each (const MAP_CSRECIPEINFO::value_type& data in m_mapRecipeInfo)
	{
		CSRecipeInfo* pRecipeInfo = data.second;
		if (NULL == pRecipeInfo) continue;

		for each (const RecipeMaterial& material in pRecipeInfo->m_vecMaterial)
		{
			m_setRecipeMaterialItemID.insert(material.m_nItemID);
		}
	}	
}

bool CSRecipeInfoMgr::IsMaterialItemID( int nItemID )
{
	return IsExist(m_setRecipeMaterialItemID, nItemID);
}