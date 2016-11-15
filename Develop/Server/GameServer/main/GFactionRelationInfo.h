#pragma once

#include "MTypes.h"

enum FACTION_RELATION_TYPE;

class GFactionRelationInfo : public MTestMemPool<GFactionRelationInfo>
{
public:
	const uint8 m_nID1;
	const uint8 m_nID2;
	const FACTION_RELATION_TYPE m_nFRT;

public:
	GFactionRelationInfo(uint8 nID1, uint8 nID2, FACTION_RELATION_TYPE nFRT);
};
