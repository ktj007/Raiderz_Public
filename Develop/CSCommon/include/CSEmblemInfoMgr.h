#pragma once

class CSEmblemInfo;

#define EMBLEM_XML_TAG_MAIET			"maiet"
#define EMBLEMS_XML_TAG_EMBLEMS			"EMBLEMS"
#define EMBLEMS_XML_TAG_EMBLEM			"EMBLEM"
#define EMBLEM_XML_ATTR_ID				"id"
#define EMBLEM_XML_ATTR_CONDITION		"condition"

typedef map<int, CSEmblemInfo*> EmblemMap;

class CSEmblemInfoMgr
{
public:
	CSEmblemInfoMgr(void);
	~CSEmblemInfoMgr(void);
	
	CSEmblemInfo* Get(int nID);
	void Clear();
	bool Load(const TCHAR* szFileName);
protected:
	virtual void OnLoadCompleted() {}
	void Insert(CSEmblemInfo* pEmblem);
private:
	void ParseEmblems( MXmlElement* pElement, MXml* pXml );
	void ParseEmblem( MXmlElement* pElement, MXml* pXml );
protected:
	EmblemMap	m_mapEmblems;
};
