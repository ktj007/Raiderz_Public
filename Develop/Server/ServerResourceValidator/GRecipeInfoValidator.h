#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GRecipeInfo;

class GVALIDATOR_API GRecipeInfoValidator : public CSBaseValidator
{
public:
	void CheckRecipe(GRecipeInfo* pRecipeInfo);

public:
	virtual bool Check();
};
