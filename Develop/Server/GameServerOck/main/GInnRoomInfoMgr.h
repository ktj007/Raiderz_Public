#ifndef _G_INN_ROOM_INFO_MGR_H_
#define _G_INN_ROOM_INFO_MGR_H_

class GInnRoomInfo;

#define INNROOM_XML_TAG_MAIET			"maiet"
#define INNROOM_XML_TAG_INN				"INNROOM"
#define INNROOM_XML_ATTR_ID				"id"
#define INNROOM_XML_ATTR_TYPE			"type"
#define INNROOM_XML_ATTR_FIELDGROUP		"fieldgroup"
#define INNROOM_XML_ATTR_DEFAULT_BED	"default_bed"
#define INNROOM_XML_ATTR_DEFAULT_STORAGE "default_storage"
#define INNROOM_XML_ATTR_DEFAULT_GUILDSTORAGE "default_guildstorage"

#define INNROOM_XML_ATTR_CRAFTING		"crafting"
#define INNROOM_XML_ATTR_BED			"bed"
#define INNROOM_XML_ATTR_CARPET			"carpet"
#define INNROOM_XML_ATTR_FIREPLACE		"fireplace"
#define INNROOM_XML_ATTR_MURAL			"mural"
#define INNROOM_XML_ATTR_TABLE			"table"
#define INNROOM_XML_ATTR_FUNITURE		"funiture"
#define INNROOM_XML_ATTR_STEWARD		"steward"
#define INNROOM_XML_ATTR_STORAGE		"storage"
#define INNROOM_XML_ATTR_GUILDSTORAGE	"guildstorage"

typedef map<uint8, GInnRoomInfo*>	MAP_INNROOMINFO;

class GInnRoomInfoMgr : public MTestMemPool<GInnRoomInfoMgr>
{
private:
	MAP_INNROOMINFO m_mapInnRoomInfo;

private:
	void ParseInnRoom(MXmlElement* pElement, MXml* pXml);

public:
	GInnRoomInfoMgr();
	~GInnRoomInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	GInnRoomInfo* NewInfo();
	void Insert(GInnRoomInfo* pInfo);
	GInnRoomInfo* Get(uint8 nID);
};

#endif
