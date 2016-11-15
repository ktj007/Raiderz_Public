#ifndef _G_CRAFT_INFO_MGR_H_
#define _G_CRAFT_INFO_MGR_H_

class GCraftInfo;

#define CRAFT_XML_TAG_MAIET			"maiet"

#define CRAFT_XML_TAG_CRAFT			"CRAFT"
#define CRAFT_XML_ATTR_ID			"id"
#define CRAFT_XML_ATTR_DIALOG		"dialog"
#define CRAFT_XML_ATTR_MAKEMOD		"makemod"
#define CRAFT_XML_ATTR_FACTIONID	"faction"

#define CRAFT_XML_TAG_RECIPE		"RECIPE"

typedef map<int, GCraftInfo*> MAP_CRAFTINFO;

class GCraftInfoMgr : public MTestMemPool<GCraftInfoMgr>
{
private:
	MAP_CRAFTINFO	m_mapInfo;

private:
	void ParseCraft(MXmlElement* pElement, MXml* pXml);
	void ParseRecipe(MXmlElement* pElement, MXml* pXml, GCraftInfo* pCraftInfo);

public:
	GCraftInfoMgr();
	~GCraftInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	GCraftInfo* New();
	void Insert(GCraftInfo* pInfo);
	GCraftInfo* Get(int nID);
	MAP_CRAFTINFO& GetContainer()	{ return m_mapInfo; }		
};

#endif//_G_CRAFT_INFO_MGR_H_
