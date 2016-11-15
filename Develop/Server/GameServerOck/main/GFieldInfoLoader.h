#pragma once

// Tag
#define FIELD_XML_TAG_MAIET			"maiet"
#define FIELD_XML_TAG_FIELD			"FIELD"
#define FIELD_XML_TAG_FIELDGROUP	"FIELDGROUP"
#define FIELD_XML_TAG_FIELDLIST		"FIELDLIST"
#define FIELD_XML_TAG_FIELDINFO		"FIELDINFO"

#define FIELD_XML_TAG_SPAWN			"SPAWN"
#define FIELD_XML_TAG_NPC			"NPC"
#define FIELD_XML_TAG_POS			"POS"
#define FIELD_XML_TAG_MARKER		"MARKER"
#define FIELD_XML_TAG_SENSOR		"SENSOR"
#define FIELD_XML_TAG_ENV			"ENV"
#define FIELD_XML_TAG_WEATHER		"WEATHER"
#define FIELD_XML_TAG_ADD			"ADD"
#define FIELD_XML_TAG_WARP			"WARP"
#define FIELD_XML_TAG_PVPAREA		"PVPAREA"


// Attribute
#define FIELD_XML_ATTR_ID			"id"
#define FIELD_XML_ATTR_PARENT		"parent"
#define FIELD_XML_ATTR_NAME			"name"
#define FIELD_XML_ATTR_ZONE			"zone"
#define FIELD_XML_ATTR_TYPE			"type"
#define FIELD_XML_ATTR_POS_X		"x"
#define FIELD_XML_ATTR_POS_Y		"y"
#define FIELD_XML_ATTR_POS_Z		"z"
#define FIELD_XML_ATTR_POS_RADIUS	"radius"
#define FIELD_XML_ATTR_RESPAWN_TIME	"respawn_time"
#define FIELD_XML_ATTR_START_TIME	"start_time"
#define FIELD_XML_ATTR_ENABLED		"enabled"
#define FIELD_XML_ATTR_EXIT			"exit"
#define FIELD_XML_ATTR_CHECKPOINT	"checkpoint"
#define FIELD_XML_ATTR_RANDOM_DIR	"random_dir"
#define FIELD_XML_ATTR_ROAM_RADIUS	"roam_radius"
#define FIELD_XML_ATTR_ROAM_DEPTH	"roam_depth"
#define FIELD_XML_ATTR_START_FIELD	"start_field"
#define FIELD_XML_ATTR_START_MARKER	"start_marker"
#define FIELD_XML_ATTR_COMBAT_RADIUS "combat_radius"
#define FIELD_XML_ATTR_MIN_X		"min_x"
#define FIELD_XML_ATTR_MIN_Y		"min_y"
#define FIELD_XML_ATTR_MIN_Z		"min_z"
#define FIELD_XML_ATTR_MAX_X		"max_x"
#define FIELD_XML_ATTR_MAX_Y		"max_y"
#define FIELD_XML_ATTR_MAX_Z		"max_z"
#define FIELD_XML_ATTR_BOUNDARY_MIN_X		"boundary_min_x"
#define FIELD_XML_ATTR_BOUNDARY_MIN_Y		"boundary_min_y"
#define FIELD_XML_ATTR_BOUNDARY_MAX_X		"boundary_max_x"
#define FIELD_XML_ATTR_BOUNDARY_MAX_Y		"boundary_max_y"
#define FIELD_XML_ATTR_RATE			"rate"
#define FIELD_XML_ATTR_GROUP		"group"
#define FIELD_XML_ATTR_TIME			"time"
#define FIELD_XML_ATTR_TIME_START	"time_s"
#define FIELD_XML_ATTR_TIME_END		"time_e"
#define FIELD_XML_ATTR_MAX_GATHER_MINE		"max_gather_mine"
#define FIELD_XML_ATTR_MAX_GATHER_HERB		"max_gather_herb"
#define FIELD_XML_ATTR_MAX_GATHER_RELIC		"max_gather_relic"
#define FIELD_XML_ATTR_MAX_GATHER_LUMBER	"max_gather_lumber"
#define FIELD_XML_ATTR_PORTALTALENT_USABLE	"returnable"
#define FIELD_XML_ATTR_QUESTPVP_NPC1		"qpvpnpc1"
#define FIELD_XML_ATTR_QUESTPVP_NPC2		"qpvpnpc2"
#define FIELD_XML_ATTR_QUESTPVP_REBIRTH1	"qpvprebirth1"
#define FIELD_XML_ATTR_QUESTPVP_REBIRTH2	"qpvprebirth2"
#define FIELD_XML_ATTR_FIXED_TIME			"fixed_time"

#define FIELD_XML_ATTR_DEST_MAP		"dest_map"
#define FIELD_XML_ATTR_DEST_X		"dest_x"
#define FIELD_XML_ATTR_DEST_Y		"dest_y"
#define FIELD_XML_ATTR_DEST_Z		"dest_z"
#define FIELD_XML_ATTR_DIR_X		"dir_x"
#define FIELD_XML_ATTR_DIR_Y		"dir_y"
#define FIELD_XML_ATTR_DIR_Z		"dir_z"

#define FIELD_XML_ATTR_PARNET		"parent"
#define FIELD_XML_ATTR_DEFAULT		"default"
#define FIELD_XML_ATTR_WEATHER		"weather"
#define FIELD_XML_ATTR_PERCENT		"percent"

#define FIELD_XML_ATTR_TITLE			"title"
#define FIELD_XML_ATTR_TYPE				"type"
#define FIELD_XML_ATTR_CONDITION		"condition"
#define FIELD_XML_ATTR_FIELD			"field"
#define FIELD_XML_ATTR_FIELD_GROUP		"field_group"
#define FIELD_XML_ATTR_GATE_TYPE		"gate_type"
#define FIELD_XML_ATTR_MARKER			"marker"
#define FIELD_XML_ATTR_QUEST			"quest"


#define FIELD_XML_ATTR_WARP_TYPE	"warp_type"
#define FIELD_XML_ATTR_VALIDATION	"validation"

#define FIELD_XML_ATTR_LEFT		"left"
#define FIELD_XML_ATTR_TOP		"top"
#define FIELD_XML_ATTR_RIGHT	"right"
#define FIELD_XML_ATTR_BOTTOM	"bottom"
#define FIELD_XML_ATTR_FACTION1	"faction1"
#define FIELD_XML_ATTR_FACTION2	"faction2"

#define FIELD_XML_ATTR_SECTOR_HEIGHT	"sector_height"

#define FIELD_XML_TAG_ROOM		"ROOM"
#define FIELD_XML_ATTR_ID		"id"
#define FIELD_XML_ATTR_ROOM_X		"x"
#define FIELD_XML_ATTR_ROOM_Y		"y"
#define FIELD_XML_ATTR_ROOM_Z		"z"
#define FIELD_XML_ATTR_ROOM_WIDTH		"width"
#define FIELD_XML_ATTR_ROOM_HEIGHT		"height"
#define FIELD_XML_ATTR_ROOM_DEPTH		"depth"
#define FIELD_XML_ATTR_ROOM_ROTATE		"rotate"

////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "GFieldInfo.h"
#include "GFieldInfoMgr.h"

class GFieldInfoLoader
{
private:
	void ParseSpawn(MXmlElement* pElement, MXml* pXml, GFieldInfo* pAreaInfo);
	void ParseMarker(MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo);
	void ParseSensor(MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo);	
	void ParseEnv(MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo);
	void ParseNPC(MXmlElement* pElement, MXml* pXml, SPAWN_INFO* pSpwanInfo, bool& bHasDefault);
	void ParsePvparea( MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo );

	void LoadFieldData(MXmlElement* pChild, MXml* pXml, GFieldInfo* pFieldInfo);

	void InsertGroup( GFieldInfoMgr::FIELD_GROUP_MAP* pFieldGroupMap, FIELD_GROUP_INFO* pInfo );
public:
	bool LoadField(GFieldInfo* pFieldInfo, wstring strFieldFileName, wstring strZonePath, CSZoneList& zoneList);
	bool LoadFieldGroupInfo(GFieldInfoMgr::FIELD_GROUP_MAP* pFieldGroupMap, const wchar_t* szFileName);
	bool ReloadField(GFieldInfo* pFieldInfo, wstring strFieldPath);
};