#pragma once

typedef map<int, set<int>> MAP_ITEMRECIPEPRODUCTINFO;

class XItem_RecipeProductInfoMgr
{
public:
	XItem_RecipeProductInfoMgr(void);
	~XItem_RecipeProductInfoMgr(void);

	void Clear();
	void Load(XItemManager* pItemManager, XRecipeInfoMgr* pRecipeInfoMgr);	

	int GetProductCount(int nMaterialItemID);
	int GetProductItemID(int nMaterialItemID, int i);

	int GetProductMinLevel(int nMaterialItemID);
	int GetProductMaxLevel(int nMaterialItemID);

	bool IsWeaponProductMaterial(int nMaterialItemID);
	bool IsShieldProductMaterial(int nMaterialItemID);
	bool IsAccessoryProductMaterial(int nMaterialItemID);
	bool IsArmorProductMaterial(int nMaterialItemID);
	bool IsUsableProductMaterial(int nMaterialItemID);
	bool IsHousingProductMaterial(int nMaterialItemID);
	bool IsEnchantProductMaterial(int nMaterialItemID);

private:
	const set<int>& Get(int nItemID);

private:
	MAP_ITEMRECIPEPRODUCTINFO	m_mapInfo;
	set<int>					m_dummuySet;
};
