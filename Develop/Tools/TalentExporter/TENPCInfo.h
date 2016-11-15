/************************************************************************/
/* 1. 캐릭터툴에서 씁니다.                                              */
/************************************************************************/

#ifndef _TENPCINFO_H
#define _TENPCINFO_H

#include "CSChrInfo.h"
#include "MXml.h"

#define NPC_XML_TAG_MAIET			"maiet"
#define NPC_XML_TAG_NPC				"NPC"
#define NPC_XML_TAG_ITEM			"ITEM"

#define NPC_XML_ATTR_ID				"id"
#define NPC_XML_ATTR_PARENT			"parent"
#define NPC_XML_ATTR_TYPE			"Type"
#define NPC_XML_ATTR_NAME			"Name"
#define NPC_XML_ATTR_MAX_HP			"MaxHP"
#define NPC_XML_ATTR_AP				"AP"
#define NPC_XML_ATTR_MODEL			"Model"
#define NPC_XML_ATTR_HITTEST_INDEX	"HitTestIndex"
#define NPC_XML_ATTR_MESH_NAME		"MeshName"
#define NPC_XML_ATTR_MESH_NODE_NAME "NodeName"
#define NPC_XML_ATTR_MESH_PATH		"MeshPath"
#define NPC_XML_ATTR_HUGE			"Huge"
#define NPC_XML_ATTR_SCALE			"Scale"
#define NPC_XML_ATTR_MODE			"Mode"
#define NPC_XML_ATTR_INTERACT		"Interact"
#define NPC_XML_ATTR_SHOW_NAME		"ShowName"
#define NPC_XML_ATTR_MONSTER		"Monster"
#define NPC_XML_ATTR_ALIGN_TERRAIN	"AlignTerrain"
#define NPC_XML_ATTR_TITLE_BONE		"TitleBone"
#define NPC_XML_ATTR_TITLE_HEIGHT	"TitleHeight"
#define NPC_XML_ATTR_BLOOD_EFFECT_NAME	"BloodEffectName"

#define NPC_XML_ATTR_MESH_PARTS_PATH	"MeshPartsPath"
#define NPC_XML_ATTR_PARTS_HEAD			"PartsHead"
#define NPC_XML_ATTR_PARTS_HAIR			"PartsHair"
#define NPC_XML_ATTR_PARTS_FACE			"PartsFace"
#define NPC_XML_ATTR_PARTS_CHEST		"PartsChest"
#define NPC_XML_ATTR_PARTS_LEG			"PartsLeg"
#define NPC_XML_ATTR_PARTS_HANDS		"PartsHands"
#define NPC_XML_ATTR_PARTS_FEET			"PartsFeet"
#define NPC_XML_ATTR_PARTS_WEAPON_R		"PartsWeaponR"
#define NPC_XML_ATTR_PARTS_WEAPON_L		"PartsWeaponL"
#define NPC_XML_ATTR_TEX_COLOR			"TexColor"
#define NPC_XML_ATTR_TEX_SKIN			"TexSkin"
#define NPC_XML_ATTR_CLAN				"Clan"
#define NPC_XML_ATTR_SKIN_COLOR			"SkinColor"
#define NPC_XML_ATTR_HAIR_COLOR			"HairColor"

#define NPC_XML_ATTR_INTERACTION_LOOT_ANI	"ILootAni"
#define NPC_XML_ATTR_INTERACTION_LOOT_TIME	"ILootTime"


class TENPCInfo : public CSNPCInfo
{
private:
	friend class TENPCInfoMgr;
public:
	enum _CONST
	{
		MESH_PARTS_COUNT = 9
	};

	vector<TENPCInfo*>	vecModes;			///< NPC 모드(일반, 하늘, 땅속 등)

public:
	TENPCInfo() : CSNPCInfo()
	{
	}
	void Clear();
};

class TENPCInfoMgr : public CSNPCInfoMgr
{
private:

protected:
	void ParseNPC(MXmlElement* pElement, MXml* pXml);

public:
	TENPCInfoMgr();
	virtual ~TENPCInfoMgr();
	void Clear();
	static TENPCInfoMgr& GetInstance();
	bool Load();
	TENPCInfo* Get(int nID);
	bool Exist(int nID);
};

#endif