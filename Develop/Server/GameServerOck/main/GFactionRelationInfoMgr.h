#pragma once

class GFactionRelationInfo;
enum FACTION_RELATION_TYPE;

#define FACTIONREL_XML_TAG_MAIET		"maiet"
#define FACTIONREL_XML_TAG_FACTIONREL	"FACTIONREL"
#define FACTIONREL_XML_ATTR_ID1			"id1"
#define FACTIONREL_XML_ATTR_ID2			"id2"
#define FACTIONREL_XML_ATTR_RELATION	"relation"

typedef map<uint8, GFactionRelationInfo*> MAP_FACTIONRELATIONINFO;

typedef map<uint8, MAP_FACTIONRELATIONINFO> MAP_MAP_FACTIONRELATIONINFO;

class GFactionRelationInfoMgr : public MTestMemPool<GFactionRelationInfoMgr>
{
private:
	MAP_MAP_FACTIONRELATIONINFO	m_mapmapInfo;

private:
	void ParseFactionRelation(MXmlElement* pElement, MXml* pXml);	

public:
	~GFactionRelationInfoMgr(void);

	void Clear();

	bool Load(const wchar_t* szFileName);
	GFactionRelationInfo* New(uint8 nID1, uint8 nID2, FACTION_RELATION_TYPE nFRT);
	void Insert(GFactionRelationInfo* pInfo);
	
	bool IsExist(uint8 nID1, uint8 nID2);
	GFactionRelationInfo* Get(uint8 nID1, uint8 nID2);
	FACTION_RELATION_TYPE GetRelation(uint8 nID1, uint8 nID2);
};
