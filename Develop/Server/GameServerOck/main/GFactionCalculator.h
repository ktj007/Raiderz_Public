#pragma once

#include "CSFactionCalculator.h"

class GFactionCalculator : public CSFactionCalculator
{
public:
	bool CheckTradable(uint16 nFactionQuantity);	
	bool IsNormalThanRelation(uint16 nFactionQuantity);
};
