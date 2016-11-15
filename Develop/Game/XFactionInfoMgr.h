#pragma once

class XFactionInfo;

#define FACTION_XML_TAG_MAIET			"maiet"
#define FACTION_XML_TAG_FACTION			"FACTION"
#define FACTION_XML_ATTR_ID				"id"
#define FACTION_XML_ATTR_TITLE			"title"
#define FACTION_XML_ATTR_DESC			"desc"
#define FACTION_XML_ATTR_DEFAULT		"default"

typedef map<uint8, XFactionInfo*> MAP_FACTIONINFO;

class XFactionInfoMgr
{
private:
	MAP_FACTIONINFO	m_mapInfo;

protected:
	void ParseFaction(MXmlElement* pElement, MXml* pXml);

public:
	~XFactionInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	XFactionInfo* New(uint8 nID, uint16 nDefaultFaction, wstring strTitle, wstring strDesc);
	void Insert(XFactionInfo* pInfo);

	bool IsExist(uint8 nID);	
	XFactionInfo* Get(uint8 nID);

	virtual bool CreateFactionFromString(const std::wstring& _strXmlData) { return false; }	// mockFactionMgr
};
