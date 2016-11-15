#pragma once

class GPlayerFaction;
class GFactionInfo;
enum FACTION_RELATION_TYPE;

typedef map<uint8, GPlayerFaction*> MAP_PLAYERFACTION;

class GPlayerFactions : public MTestMemPool<GPlayerFactions>
{
private:
	MAP_PLAYERFACTION		m_mapPlayerFaction;

private:	
	GPlayerFaction*	Get(uint8 nID);

public:
	GPlayerFactions(void);
	~GPlayerFactions(void);

	void Clear();

	GPlayerFaction* Insert(GFactionInfo* pFactionInfo, uint16 nQuantity);
	void GetAll(vector<GPlayerFaction*>& outvecPlayerFaction);

	void Increase(uint8 nID, uint16 nQuantity);
	void Decrease(uint8 nID, uint16 nQuantity);

	bool IsExist(uint8 nID);
	uint16	GetQuantity(uint8 nID);
	uint16	GetQuantityPercent(uint8 nID);
	FACTION_RELATION_TYPE GetRelation(uint8 nID);	

	MAP_PLAYERFACTION& GetContainer()	{ return m_mapPlayerFaction;	}
};
