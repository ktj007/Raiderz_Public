#ifndef _G_RECIPEINFO_INFO_H
#define _G_RECIPEINFO_INFO_H

#include "CSRecipeInfoMgr.h"

class CSRecipeInfo;
class GRecipeInfo;

class GRecipeInfoMgr : public CSRecipeInfoMgr, public MTestMemPool<GRecipeInfoMgr>
{
private:
	virtual bool ParseRecipe(MXmlElement* pElement, MXml* pXml, CSRecipeInfo* pInfo);

public:
	GRecipeInfoMgr();
	virtual ~GRecipeInfoMgr();
	
	virtual CSRecipeInfo* New();
	GRecipeInfo* Get(int nID);
};

#endif//_G_RECIPEINFO_INFO_H
