#pragma once

typedef set<int>	SET_RECIPEID;

class GPlayerRecipe : public MTestMemPool<GPlayerRecipe>
{
private:
	SET_RECIPEID	m_setRecipeID;
	GEntityPlayer*	m_pOwner;

public:
	GPlayerRecipe(GEntityPlayer* pOwner);
	~GPlayerRecipe(void);

	void Insert(int nRecipeID);
	void Delete(int nRecipeID);

	bool IsExist(int nRecipeID);
	bool IsEmpty();

	const SET_RECIPEID& GetContainer();
};
