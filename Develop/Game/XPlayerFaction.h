#pragma once

enum FACTION_RELATION_TYPE;

class XPlayerFaction
{
public:
	uint8			m_nID;
	uint16			m_nQuantity;

public:

	XPlayerFaction(uint8 nID, uint16 nQuantity);

	FACTION_RELATION_TYPE GetRelation();
	uint16 GetQuantity();
};


typedef map<uint8, XPlayerFaction*> MAP_PLAYERFACTION;

/// 플레이어의 팩션 모음
class XPlayerFactions
{
private:	
	MAP_PLAYERFACTION	m_mapPlayerFaction;

private:
	XPlayerFaction* Insert(uint8 nID, uint16 nQuantity);
	XPlayerFaction*	Get(uint8 nID);

public:
	XPlayerFactions();
	~XPlayerFactions();

	void Clear();

	bool GetIndex( size_t nIndex, XPlayerFaction** pFaction);
	size_t GetSize() const			{ return m_mapPlayerFaction.size();		}

	void SetQuantity(uint8 nID, uint16 nQuantity);

	bool IsExist(uint8 nID);
	FACTION_RELATION_TYPE GetRelation(uint8 nID);
	uint16 GetQuantity(uint8 nID);

	MWLua::table GetFactionInfoTable();
};
