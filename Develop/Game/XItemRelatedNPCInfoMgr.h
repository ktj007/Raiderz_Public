#pragma once

#define  ITEMRELATEDNPC_XML_TAG_MAIET		"maiet"
#define  ITEMRELATEDNPC_XML_TAG_ITEM		"item"
#define  ITEMRELATEDNPC_XML_TAG_NPC			"npc"
#define  ITEMRELATEDNPC_XML_ATTR_ID			"id"

class XItemRelatedNPCInfo;


typedef map<int, XItemRelatedNPCInfo*> MAP_ITEMRELATEDNPCINFO;

class XItemRelatedNPCInfoMgr
{
private:
	MAP_ITEMRELATEDNPCINFO	m_mapInfo;

private:
	bool ParseItem(MXmlElement* pElement, MXml* pXml, XItemRelatedNPCInfo* pInfo);
	void ParseNPC(MXmlElement* pElement, MXml* pXml, XItemRelatedNPCInfo* pInfo);


public:
	XItemRelatedNPCInfoMgr(void);
	~XItemRelatedNPCInfoMgr(void);

	void Clear();

	bool Load(const wchar_t* szFileName);	
	void Insert(XItemRelatedNPCInfo* pInfo);

	XItemRelatedNPCInfo* Get(int nItemID);
	bool IsExist(int nItemID);
};
