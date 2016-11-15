#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GCraftInfo;

class GVALIDATOR_API GCraftInfoValidator : public CSBaseValidator
{
public:
	void CheckCraft(GCraftInfo* pCraftInfo);
	void CheckRecipe(int nRecipeID);

public:
	virtual bool Check();
};
