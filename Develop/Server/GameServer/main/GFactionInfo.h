#pragma once

enum FACTION_RELATION_TYPE;

class GFactionInfo : public MTestMemPool<GFactionInfo>

{
public:
	const uint8		m_nID;
	const uint16	m_nDefaultQuantity;
	const FACTION_RELATION_TYPE	m_nDefaultFRT;

public:
	GFactionInfo(uint8 nID, uint16 nDefaultQuantity);
};
