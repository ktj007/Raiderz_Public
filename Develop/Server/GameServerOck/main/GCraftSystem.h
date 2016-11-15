#ifndef _GCRAFT_SYSTEM_H_
#define _GCRAFT_SYSTEM_H_

#include "GITEM_STACK_AMT.h"

class GEntityPlayer;
class GCraftInfo;
class GRecipeInfo;

class GCraftSystem : public MTestMemPool<GCraftSystem>
{
private:
	void Show_Route(GEntityPlayer* pPlayer, const GCraftInfo* pCraftInfo);
	
	bool Make_Check(GEntityPlayer* pPlayer, GCraftInfo* pCraftInfo, GRecipeInfo* pRecipeInfo, int& outnPrice, GITEM_STACK_AMT_VEC& outvecRecpItemList);
	bool Make_Apply(GEntityPlayer* pPlayer, int nProductItemID, int nAmount, int nPrice, GITEM_STACK_AMT_VEC& vecRecpItemList);
	void Make_Route(GEntityPlayer* pPlayer, int nRecipeID);

	bool InsertRecipe_Check(GEntityPlayer* pPlayer, int nRecipeID);
	bool InsertRecipe_Apply(GEntityPlayer* pPlayer, int nRecipeID);
	void InsertRecipe_Route(GEntityPlayer* pPlayer, int nRecipeID);

	bool DeleteRecipe_Check(GEntityPlayer* pPlayer, int nRecipeID);
	bool DeleteRecipe_Apply(GEntityPlayer* pPlayer, int nRecipeID);
	void DeleteRecipe_Route(GEntityPlayer* pPlayer, int nRecipeID);

	float CalcMakeMod( GEntityPlayer* pPlayer, const GCraftInfo* pCraftInfo );

public:
	GCraftSystem();
	virtual ~GCraftSystem();	// 테스트 코드 작성을 하기 위해서 상속이 필요함.

	bool MakeRecipeItemList(GEntityPlayer* pPlayer, GRecipeInfo* pRecpInfo, GITEM_STACK_AMT_VEC& vecRecp);
	
	bool Show(GEntityPlayer* pPlayer, int nCraftID);
	bool Make(GEntityPlayer* pPlayer, int nCraftID, int nRecipeID);

	bool InsertRecipe(GEntityPlayer* pPlayer, int nRecipeID);
	bool DeleteRecipe(GEntityPlayer* pPlayer, int nRecipeID);
};

#endif//_GCRAFT_SYSTEM_H_
