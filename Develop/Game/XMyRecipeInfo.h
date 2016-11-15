#pragma once

class XMyRecipeInfo
{
private:
	vector<int>				m_vecRecipe;
	//vector<XRecipeInfo*>	m_vecRecipeInfo;

public:
	void Clear()
	{
		m_vecRecipe.clear();
		//m_vecRecipeInfo.clear();
	}

	void push_back( int nRecipeID );
	void erase( int nRecipeID );

	size_t GetCount()					{ return m_vecRecipe.size();			}
	int GetRecipeID( int _index)		{ return m_vecRecipe[ _index];			}

	bool IsExist( int nRecipeID);

	MWLua::table GetRecipeTable();

	void CollectJustCraftableRecipeID(vector<int>& vecJustMakableRecipeID, int nItemID, int nAddAmount);
	int GetCraftableRecipeCount();
};

