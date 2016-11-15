#pragma once

#include "CSFactionCalculator.h"

class GFactionInfo;
class GFactionRelationInfo;
class GEntityPlayer;
enum FACTION_RELATION_TYPE;


class GUTHelper_Faction
{
public:
	static GFactionInfo* NewFactionInfo(uint16 nDefaultQuantity=CSFactionCalculator::GetNormalMinQuantity(), uint8 nID=INVALID_ID);
	static GFactionRelationInfo* NewFactionRelationInfo(uint8 nID1, uint8 nID2, FACTION_RELATION_TYPE nFRT);	

	static GFactionInfo* SetWorstFaction(GEntityPlayer* pPlayer, int nID=INVALID_ID);
	static GFactionInfo* SetBadFaction(GEntityPlayer* pPlayer, int nID=INVALID_ID);
	static GFactionInfo* SetGoodFaction(GEntityPlayer* pPlayer, int nID=INVALID_ID);
	static GFactionInfo* SetExcellentFaction(GEntityPlayer* pPlayer, int nID=INVALID_ID);	

	static GFactionInfo* SetFaction(GEntityPlayer* pPlayer, uint16 nQuantity, int nID=INVALID_ID);
};
