#pragma once

#include "MWLua.h"

class XGlueFunction
{
public:
	virtual void DeclGlueFunc(MWLua* pLua) = 0;
};

