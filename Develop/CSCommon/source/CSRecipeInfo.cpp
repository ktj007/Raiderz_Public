#include "StdAfx.h"
#include "CSRecipeInfo.h"
#include "AStlUtil.h"

CSRecipeInfo::CSRecipeInfo()
: m_nID(0)
, m_nProductItemID(0)
, m_nAmount(1)
, m_nPrice(0)
, m_nConditionID(0)
{
}

CSRecipeInfo::~CSRecipeInfo()
{

}

bool CSRecipeInfo::IsMaterialItemID( int nItemID )
{
	return IsExist(m_setMaterialItemID, nItemID);
}

int CSRecipeInfo::GetMaterialItemAmount( int nItemID )
{
	for each (const RecipeMaterial& material in m_vecMaterial)
	{
		if (nItemID == material.m_nItemID)
		{
			return material.m_nAmount;
		}
	}

	return 0;
}
