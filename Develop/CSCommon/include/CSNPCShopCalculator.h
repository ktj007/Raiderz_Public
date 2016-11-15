#pragma once

#include "CSCommonLib.h"
#include "CSDef.h"
#include "MTypes.h"

enum FACTION_RELATION_TYPE;

class CSCOMMON_API CSNPCShopCalculator
{
public:	
	static int CalcRepairPrice(int nLostedDruability, int nRepairGrade, uint8 nEnchantCount);
	static int CalcBuyPrice(int nBaseBuyPrice, float fBaseBuyMod, FACTION_RELATION_TYPE nRelation);
	static int CalcSellPrice(int nBaseSellPrice, float fBaseSellMod, FACTION_RELATION_TYPE nRelation);

	static float CalcBuyMod(float fBaseBuyMod, FACTION_RELATION_TYPE nFRT);
	static float CalcSellMod(float fBaseBuyMod, FACTION_RELATION_TYPE nFRT);

private:
	static int CalcRepairPricePerDurability(int nRepairGrade);
	static float CalcRepairEnchantCountFactor(uint8 nEnchantCount);
};
