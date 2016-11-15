#pragma once

class GQPEventInfo;
class GQPESpawnInfo;
class GQPERewardInfo;
class GQPEEndInfo;
class GQPEItemInfo;

#define QPEVENT_XML_TAG_MAIET			"maiet"

#define QPEVENT_XML_TAG_QPEVENT			"QPEVENT"
#define QPEVENT_XML_TAG_BEGIN			"BEGIN"
#define QPEVENT_XML_TAG_END				"END"
#define QPEVENT_XML_TAG_SPAWN			"SPAWN"
#define QPEVENT_XML_TAG_DESPAWN			"DESPAWN"
#define QPEVENT_XML_TAG_NOTICE			"NOTICE"
#define QPEVENT_XML_TAG_REWARD			"REWARD"
#define QPEVENT_XML_TAG_ITEM			"ITEM"

#define QPEVENT_XML_ATTR_ID				"id"
#define QPEVENT_XML_ATTR_NPCID			"npcid"
#define QPEVENT_XML_ATTR_TITLE			"title"
#define QPEVENT_XML_ATTR_TEXT			"text"
#define QPEVENT_XML_ATTR_DELAYTIME		"delaytime"
#define QPEVENT_XML_ATTR_DESPAWNTIME	"despawntime"
#define QPEVENT_XML_ATTR_TYPE			"type"
#define QPEVENT_XML_ATTR_PARAM1			"param1"
#define QPEVENT_XML_ATTR_PARAM2			"param2"
#define QPEVENT_XML_ATTR_WINNERTEAM		"winnerteam"
#define QPEVENT_XML_ATTR_MONEY			"money"
#define QPEVENT_XML_ATTR_XP				"xp"
#define QPEVENT_XML_ATTR_AMOUNT			"amount"


typedef map<int, GQPEventInfo*> MAP_QPEVENTINFO;

class GQPEventInfoMgr : public MTestMemPool<GQPEventInfoMgr>
{
public:
	GQPEventInfoMgr();
	~GQPEventInfoMgr();

	void Clear();
	bool Load(const wchar_t* szFileName);	
	void Insert(GQPEventInfo* pInfo);
	GQPEventInfo* Get(int nID);
	vector<GQPEventInfo*> GetAll();

private:
	void ParseQPEvent(MXmlElement* pElement, MXml* pXml);
	void ParseBegin(MXmlElement* pElement, MXml* pXml, GQPEventInfo* pInfo);
	void ParseEnd(MXmlElement* pElement, MXml* pXml, GQPEventInfo* pInfo);
	void ParseNotice(MXmlElement* pElement, MXml* pXml, vector<wstring>& vecStrNotice);
	void ParseSpawn(MXmlElement* pElement, MXml* pXml, vector<GQPESpawnInfo*>& vecSpawnInfo);
	void ParseDespawn(MXmlElement* pElement, MXml* pXml, vector<int>& vecDespawnNPCID);
	void ParseReward(MXmlElement* pElement, MXml* pXml, GQPEEndInfo* pEndInfo);
	void ParseItem(MXmlElement* pElement, MXml* pXml, vector<GQPEItemInfo*>& vecItemInfo);

private:
	MAP_QPEVENTINFO	m_mapInfo;
};