#pragma once

class GFactionInfo;
enum FACTION_RELATION_TYPE;

class GPlayerFaction : public MTestMemPool<GPlayerFaction>
{
private:
	GFactionInfo*	m_pInfo;
	uint16			m_nQuantity;

public:
	GPlayerFaction(GFactionInfo* pInfo, uint16 nQuantity);

	void Increase(uint16 nQuantity);
	void Decrease(uint16 nQuantity);

	uint16 GetQuantity();
	FACTION_RELATION_TYPE GetRelation();
	GFactionInfo* GetInfo();
	uint8 GetID();
};
