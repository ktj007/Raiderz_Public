#pragma once

#include "CSCommonLib.h"
#include "CSFactionInfo.h"

class CSCOMMON_API CSFactionCalculator
{
private:
	static uint16 m_FactionTable[FRT_MAX];

public:
	static FACTION_RELATION_TYPE CalculRelation(uint16 nFactionQuantity);

	static uint16 GetMinQuantity();

	static uint16 GetBadMinQuantity();
	static uint16 GetBadMaxQuantity();	

	static uint16 GetNormalMinQuantity();	
	static uint16 GetNormalMaxQuantity();	

	static uint16 GetGoodMinQuantity();
	static uint16 GetGoodMaxQuantity();	

	static uint16 GetMaxQuantity();

	static bool IsNormalThanRelation( uint16 nFactionQuantity );

	static float GetCraftMakeMod(FACTION_RELATION_TYPE nFRT);

	static uint16 GetQuantity(FACTION_RELATION_TYPE nFRT);
};
