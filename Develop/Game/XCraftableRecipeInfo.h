#pragma once

class XRecipeInfo;

class XCraftableRecipeInfo
{
private:
	vector<TD_CRAFT_RECIPE>	m_vecRecipe;
	vector<XRecipeInfo*>	m_vecRecipeInfo;

	float					m_fMakeMod;

public:
	void Clear()
	{
		m_vecRecipe.clear();
		m_vecRecipeInfo.clear();
	}

	XRecipeInfo*			GetRecipeInfoByIndex(int nIndex);

	float&					GetMakeMod()		{ return m_fMakeMod;	}
	vector<TD_CRAFT_RECIPE>&	GetRecipeList()	{ return m_vecRecipe; }
	vector<XRecipeInfo*>&	GetRecipeInfoList()	{ return m_vecRecipeInfo; }

	int						GetRecipeCount()	{ return m_vecRecipeInfo.size();}
	int						GetProductItemIDByIndex(int nIndex);
	int						GetPriceByIndex(int nIndex);
	MWLua::table			GetMaterialInfoTableByIndex(int nIndex);
	MWLua::table			GetMaterialInfoTable(int nRecipeID);

	bool					IsCraftableItem(int nIndex);
};
