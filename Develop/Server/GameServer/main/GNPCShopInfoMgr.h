#ifndef _G_TRADE_INFO_MGR_H_
#define _G_TRADE_INFO_MGR_H_

class GNPCShopInfo;

#define TRADE_XML_TAG_MAIET					"maiet"
#define TRADE_XML_TAG_NPCSHOP				"NPCSHOP"
#define TRADE_XML_TAG_ITEM					"ITEM"
#define TRADE_XML_ATTR_ID					"id"
#define TRADE_XML_ATTR_DIALOG				"dialog"
#define TRADE_XML_ATTR_BUYMOD				"buymod"
#define TRADE_XML_ATTR_SELLMOD				"sellmod"
#define TRADE_XML_ATTR_FACTION				"faction"
#define TRADE_XML_ATTR_REPAIRABLE			"repairable"
#define TRADE_XML_ATTR_CONDITION			"condition"

typedef map<int, GNPCShopInfo*> MAP_NPCSHOPINFO;

class GNPCShopInfoMgr : public MTestMemPool<GNPCShopInfoMgr>
{
private:
	MAP_NPCSHOPINFO	m_mapNPCShopInfo;

private:
	void ParseNPCShop(MXmlElement* pElement, MXml* pXml);
	void ParseItem(MXmlElement* pElement, MXml* pXml, GNPCShopInfo* pNPCShopInfo);

public:
	GNPCShopInfoMgr();
	~GNPCShopInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	void Insert(GNPCShopInfo* pNPCShopInfo);
	GNPCShopInfo* Get(int nID);	
	MAP_NPCSHOPINFO& GetContainer()	{ return m_mapNPCShopInfo; }		

	bool IsRepairable(int nID);
};

#endif//_G_TRADE_INFO_MGR_H_
