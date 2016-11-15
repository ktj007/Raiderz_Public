#include "StdAfx.h"
#include "XRecipeInfoMgr.h"
#include "XRecipeInfo.h"

XRecipeInfoMgr::XRecipeInfoMgr(void)
{
}

XRecipeInfoMgr::~XRecipeInfoMgr(void)
{
}

bool XRecipeInfoMgr::ParseRecipe(MXmlElement* pElement, MXml* pXml, CSRecipeInfo* pInfo)
{
	if (false == __super::ParseRecipe(pElement, pXml, pInfo)) return false;

	XRecipeInfo* pXInfo = static_cast<XRecipeInfo*>(pInfo);

	return true;
}

CSRecipeInfo* XRecipeInfoMgr::New()
{
	return new XRecipeInfo();
}

XRecipeInfo* XRecipeInfoMgr::Get(int nID)
{
	return static_cast<XRecipeInfo*>(__super::Get(nID));
}

int XRecipeInfoMgr::GetCraftableItemQuantity(int nID)
{
	XRecipeInfo* pRecipeInfo = Get(nID);
	if (NULL == pRecipeInfo) return 0;

	return GetCraftableItemQuantity(pRecipeInfo);
}

int XRecipeInfoMgr::GetCraftableItemQuantity(XRecipeInfo* pRecipeInfo)
{
	int nCratableItemQuantity = 0;
	for each (RecipeMaterial material in pRecipeInfo->m_vecMaterial)
	{
		int nEnoughMaterialPiece = gvar.MyInfo.Inventory.GetItemAmountByID(material.m_nItemID) / material.m_nAmount;
		if (0 >= nEnoughMaterialPiece) return 0;
		if (nEnoughMaterialPiece < nCratableItemQuantity)
		{
			nCratableItemQuantity = nEnoughMaterialPiece;
		}
	}	

	return nCratableItemQuantity;
}

bool XRecipeInfoMgr::IsCraftableRecipe( int nRecipeID )
{
	XRecipeInfo* pRecipeInfo = Get(nRecipeID);
	return IsCraftableRecipe(pRecipeInfo);
}

bool XRecipeInfoMgr::IsCraftableRecipe(XRecipeInfo* pRecipeInfo)
{
	if (NULL == pRecipeInfo) return false;

	for each (RecipeMaterial material in pRecipeInfo->m_vecMaterial)
	{
		int nSum = gvar.MyInfo.Inventory.GetItemAmountByID(material.m_nItemID) +
			gvar.MyInfo.EquipmentSlot.GetItemQuantityByID(material.m_nItemID);

		if (material.m_nItemID > 0  &&
			nSum < material.m_nAmount)		return false;
	}

	return true;
}