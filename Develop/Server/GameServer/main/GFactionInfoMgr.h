#pragma once

class GFactionInfo;

#define FACTION_XML_TAG_MAIET			"maiet"
#define FACTION_XML_TAG_FACTION			"FACTION"
#define FACTION_XML_ATTR_ID				"id"
#define FACTION_XML_ATTR_DEFAULT		"default"

typedef map<uint8, GFactionInfo*> MAP_FACTIONINFO;

class GFactionInfoMgr : public MTestMemPool<GFactionInfoMgr>
{
private:
	MAP_FACTIONINFO	m_mapInfo;

private:
	void ParseFaction(MXmlElement* pElement, MXml* pXml);

public:
	~GFactionInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	GFactionInfo* New(uint8 nID, uint16 nDefaultQuantity);
	void Insert(GFactionInfo* pInfo);
	
	bool IsExist(uint8 nID);	
	GFactionInfo* Get(uint8 nID);
};
