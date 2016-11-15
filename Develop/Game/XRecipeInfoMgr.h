#ifndef _X_RECIPEINFO_INFO_H
#define _X_RECIPEINFO_INFO_H

#include "CSRecipeInfoMgr.h"

class CSRecipeInfo;
class XRecipeInfo;

class XRecipeInfoMgr : public CSRecipeInfoMgr
{
private:
	virtual bool ParseRecipe(MXmlElement* pElement, MXml* pXml, CSRecipeInfo* pInfo);

public:
	XRecipeInfoMgr();
	virtual ~XRecipeInfoMgr();

	virtual CSRecipeInfo* New();
	XRecipeInfo* Get(int nID);
	int GetCraftableItemQuantity(int nID);
	int GetCraftableItemQuantity(XRecipeInfo* pRecipeInfo);

	bool IsCraftableRecipe(int nRecipeID);
	bool IsCraftableRecipe(XRecipeInfo* pRecipeInfo);

};

#endif//_X_RECIPEINFO_INFO_H