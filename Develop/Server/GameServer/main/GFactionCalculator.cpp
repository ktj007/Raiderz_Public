#include "StdAfx.h"
#include "GFactionCalculator.h"
#include "GConst.h"

bool GFactionCalculator::CheckTradable(uint16 nFactionQuantity)
{
	return IsNormalThanRelation(nFactionQuantity);
}

bool GFactionCalculator::IsNormalThanRelation( uint16 nFactionQuantity )
{
	return CSFactionCalculator::IsNormalThanRelation(nFactionQuantity);
}