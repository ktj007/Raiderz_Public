#ifndef _GLOOT_INFO_MGR_H_
#define _GLOOT_INFO_MGR_H_

#define LOOT_XML_TAG_MAIET			"maiet"
#define LOOT_XML_TAG_LOOT			"LOOT"
#define LOOT_XML_TAG_MONEY			"SILVER"
#define LOOT_XML_TAG_ITEM			"ITEM"
#define LOOT_XML_TAG_SOUL			"SOUL"
#define LOOT_XML_TAG_BUFF			"BUFF"

#define LOOT_XML_ATTR_ID			"id"
#define LOOT_XML_ATTR_RATE			"rate"
#define LOOT_XML_ATTR_MIN			"min"
#define LOOT_XML_ATTR_MAX			"max"
#define LOOT_XML_ATTR_TALENT		"talent"
#define LOOT_XML_ATTR_QUEST			"quest"
#define LOOT_XML_ATTR_BPART			"bpart"
#define LOOT_XML_ATTR_QUESTPVP_FIELD "questpvpfield"
#define LOOT_XML_ATTR_TYPE			"type"
#define LOOT_XML_ATTR_BPART1		"bpart1"
#define LOOT_XML_ATTR_BPART2		"bpart2"
#define LOOT_XML_ATTR_BPART3		"bpart3"
#define LOOT_XML_ATTR_BPART4		"bpart4"
#define LOOT_XML_ATTR_VALIDATION	"validation"
#define LOOT_XML_ATTR_BPARTLOOT		"bpart"



#define LOOT_XML_VALUE_TRUE			"TRUE"
#define LOOT_XML_VALUE_FALSE		"FALSE"

class GLootInfo;

typedef map<int, GLootInfo*> LOOT_INFO_MAP;
class GLootInfoMgr : public MTestMemPool<GLootInfoMgr>
{
private:
	LOOT_INFO_MAP m_mapLootInfo;
private:
	void ParseLOOT(MXmlElement* pElement, MXml* pXml);
	void ParseMONEY(MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo);
	void ParseITEM(MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo);
	void ParseSOUL(MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo);
	void ParseBUFF( MXmlElement* pElement, MXml* pXml, GLootInfo* pLootInfo );

public:
	GLootInfoMgr();
	~GLootInfoMgr();
	void Clear();

	bool Load(const wchar_t* szFileName);
	void Insert(GLootInfo* pLootInfo);
	GLootInfo* Get(int nLootID);
	bool IsExist(int nID);

	LOOT_INFO_MAP& GetContainer();
	
};

#endif //_GLOOT_INFO_MGR_H_
