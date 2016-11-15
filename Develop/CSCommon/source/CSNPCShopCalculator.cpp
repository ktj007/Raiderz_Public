#include "StdAfx.h"
#include "CSNPCShopCalculator.h"
#include "AMath.h"
#include "CSFactionCalculator.h"

int CSNPCShopCalculator::CalcRepairPrice(int nLostedDruability, int nRepairGrade, uint8 nEnchantCount)
{
	int nRepairPricePerDurability = CalcRepairPricePerDurability(nRepairGrade);
	float fEnchantCountFactor = CalcRepairEnchantCountFactor(nEnchantCount);

	int nRepairPrice = int(nRepairPricePerDurability * nLostedDruability * fEnchantCountFactor);

	if (0 < nLostedDruability)
	{
		nRepairPrice = max(1, nRepairPrice);
	}
	else
	{
		nRepairPrice = max(0, nRepairPrice);
	}	

	return nRepairPrice;
}


int CSNPCShopCalculator::CalcRepairPricePerDurability(int nRepairGrade)
{
	nRepairGrade = min(nRepairGrade, 100);
	nRepairGrade = max(nRepairGrade, 0);

	float fRepairPricePerDurability = (pow((float)nRepairGrade,2)/2 + (float)nRepairGrade+1) * 0.5f;

	return static_cast<int>(fRepairPricePerDurability);
}

float CSNPCShopCalculator::CalcRepairEnchantCountFactor(uint8 nEnchantCount)
{
	float fEnchantCountFactor = AMath::Pow((nEnchantCount * 0.2f), 2) + 1;

	return fEnchantCountFactor;
}

int CSNPCShopCalculator::CalcBuyPrice(int nBaseBuyPrice, float fBaseBuyMod, FACTION_RELATION_TYPE nRelation)
{
	float fBuyMod = CalcBuyMod(fBaseBuyMod, nRelation);

	int nBuyPrice = 0;
	if (INT_MAX / fBuyMod < nBaseBuyPrice)
	{
		nBuyPrice = INT_MAX;
	}
	else
	{
		nBuyPrice = (int)AMath::Round(nBaseBuyPrice * fBuyMod);
	}

	return nBuyPrice;
}

int CSNPCShopCalculator::CalcSellPrice( int nBaseSellPrice, float fBaseSellMod, FACTION_RELATION_TYPE nRelation )
{
	float fSellMod = CalcSellMod(fBaseSellMod, nRelation);

	int nSellPrice = 0;
	if (INT_MAX / fSellMod < nBaseSellPrice)
	{
		nSellPrice = INT_MAX;
	}
	else
	{
		nSellPrice = (int)AMath::Round(nBaseSellPrice * fSellMod);
	}

	return nSellPrice;
}

float CSNPCShopCalculator::CalcBuyMod(float fBaseBuyMod, FACTION_RELATION_TYPE nFRT)
{
	float fBuyMod;

	if (nFRT == FRT_GOOD)
	{
		fBuyMod = fBaseBuyMod * NPC_SHOP_BUY_MOD_FACTION_GOOD;
	}
	else if (nFRT >= FRT_EXCELLENT)
	{
		fBuyMod = fBaseBuyMod * NPC_SHOP_BUY_MOD_FACTION_EXCELLENT;
	}
	else
	{
		fBuyMod = fBaseBuyMod;
	}

	return fBuyMod;
}

float CSNPCShopCalculator::CalcSellMod(float fBaseBuyMod, FACTION_RELATION_TYPE nFRT)
{
	float fBuyMod;

	if (nFRT == FRT_GOOD)
	{
		fBuyMod = fBaseBuyMod * NPC_SHOP_SELL_MOD_FACTION_GOOD;
	}
	else if (nFRT >= FRT_EXCELLENT)
	{
		fBuyMod = fBaseBuyMod * NPC_SHOP_SELL_MOD_FACTION_EXCELLENT;
	}
	else
	{
		fBuyMod = fBaseBuyMod;
	}

	return fBuyMod;
}