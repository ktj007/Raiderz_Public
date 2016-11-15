#pragma once

#define  PRESET_XML_TAG_MAIET			"maiet"
#define  PRESET_XML_TAG_PRESET			"PRESET"
#define  PRESET_XML_TAG_TALENT			"TALENT"
#define  PRESET_XML_TAG_ITEM			"ITEM"

#define  PRESET_XML_ATTR_ID				"id"
#define  PRESET_XML_ATTR_LEVEL			"level"
#define  PRESET_XML_ATTR_TP				"tp"

class GPresetInfo;

typedef map<wstring, GPresetInfo*> MAP_PRESETINFO;

class GPresetInfoMgr : public MTestMemPool<GPresetInfoMgr>
{
public:
	GPresetInfoMgr(void);
	~GPresetInfoMgr(void);

	void Clear();

	bool Load(const TCHAR* szFileName);
	void Insert(GPresetInfo* pInfo);

	GPresetInfo* Get(wstring strID);

private:
	bool ParsePreset(MXmlElement* pElement, MXml* pXml, GPresetInfo* pInfo);
	void ParseTalent(MXmlElement* pElement, MXml* pXml, GPresetInfo* pInfo);
	void ParseItem(MXmlElement* pElement, MXml* pXml, GPresetInfo* pInfo);

private:
	MAP_PRESETINFO			m_mapInfo;
};
