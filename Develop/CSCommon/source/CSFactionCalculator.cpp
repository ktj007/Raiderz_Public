#include "StdAfx.h"
#include "CSFactionCalculator.h"
#include "CSDef.h"

uint16 CSFactionCalculator::m_FactionTable[FRT_MAX] = { 0, 15999, 27999, 35999, 47999, 64000, 65000 };


FACTION_RELATION_TYPE CSFactionCalculator::CalculRelation(uint16 nFactionQuantity)
{
	if (m_FactionTable[FRT_NONE] <= nFactionQuantity &&
		m_FactionTable[FRT_WORST] >= nFactionQuantity)
	{
		return FRT_WORST;
	}

	if (m_FactionTable[FRT_WORST] < nFactionQuantity &&
		m_FactionTable[FRT_BAD] >= nFactionQuantity)
	{
		return FRT_BAD;
	}

	if (m_FactionTable[FRT_BAD] < nFactionQuantity &&
		m_FactionTable[FRT_NORMAL] >= nFactionQuantity)
	{
		return FRT_NORMAL;
	}

	if (m_FactionTable[FRT_NORMAL] < nFactionQuantity &&
		m_FactionTable[FRT_GOOD] >= nFactionQuantity)
	{
		return FRT_GOOD;
	}

	if (m_FactionTable[FRT_GOOD] < nFactionQuantity &&
		m_FactionTable[FRT_EXCELLENT] >= nFactionQuantity)
	{
		return FRT_EXCELLENT;
	}

	if (m_FactionTable[FRT_EXCELLENT] < nFactionQuantity &&
		m_FactionTable[FRT_TRUST] >= nFactionQuantity)
	{
		return FRT_TRUST;
	}

	return FRT_NONE;
}

uint16 CSFactionCalculator::GetMinQuantity()
{
	return m_FactionTable[FRT_NONE];
}

uint16 CSFactionCalculator::GetBadMinQuantity()
{
	return m_FactionTable[FRT_WORST]+1;
}

uint16 CSFactionCalculator::GetBadMaxQuantity()
{
	return m_FactionTable[FRT_BAD];
}

uint16 CSFactionCalculator::GetNormalMinQuantity()
{
	return m_FactionTable[FRT_BAD]+1;
}

uint16 CSFactionCalculator::GetNormalMaxQuantity()
{
	return m_FactionTable[FRT_NORMAL];
}

uint16 CSFactionCalculator::GetGoodMinQuantity()
{
	return m_FactionTable[FRT_NORMAL]+1;
}

uint16 CSFactionCalculator::GetGoodMaxQuantity()
{
	return m_FactionTable[FRT_GOOD];
}

uint16 CSFactionCalculator::GetMaxQuantity()
{
	return m_FactionTable[FRT_TRUST];
}

bool CSFactionCalculator::IsNormalThanRelation( uint16 nFactionQuantity )
{
	FACTION_RELATION_TYPE eType = CalculRelation(nFactionQuantity);
	return (eType >= FRT_NORMAL);
}

float CSFactionCalculator::GetCraftMakeMod( FACTION_RELATION_TYPE nFRT )
{
	if (nFRT == FRT_GOOD)
	{
		return CRAFT_MAKE_MOD_FACTION_GOOD;
	}
	else if (nFRT >= FRT_EXCELLENT)
	{
		return CRAFT_MAKE_MOD_FACTION_EXCELLENT;
	}

	return 1.0f;
}

uint16 CSFactionCalculator::GetQuantity( FACTION_RELATION_TYPE nFRT )
{
	if (FRT_NONE >= nFRT || FRT_MAX <= nFRT) return 0;

	return m_FactionTable[nFRT-1] + 1;
}
