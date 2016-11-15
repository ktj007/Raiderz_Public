/************************************************************************/
/* 1. 캐릭터툴에서 씁니다.                                              */
/************************************************************************/

#ifndef _XNPCINFO_H
#define _XNPCINFO_H

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

struct TNPCMtrl
{
	int				nID;		///< 현재 Mtrl 정보의 Mesh 인덱스
	int				nSubCount;	///< Multi/Sub Mtrl의 개수. Standard 이면 1개.
	vector<string>	vecSubMtrls;///< 실제 참조할 RMaterial의 인덱스.
};

class TNPCInfo : public CSNPCInfo
{
private:
	friend class TNPCInfoMgr;	
	bool				m_bUseMeshParts;
	bool				m_bUseTexColor;
public:
	enum _CONST
	{
		MESH_PARTS_COUNT = 9
	};

	vector<TNPCInfo*>	vecModes;			///< NPC 모드(일반, 하늘, 땅속 등)
	int					nPartsItemID[MESH_PARTS_COUNT];	
	bool				bAlignTerrain;
	unsigned int		nTexColor;
	string				strTexSkin;
	string				strInteractionLootAni;		///< 인터랙션 루팅 할때 쓸 애니메이션

	// 2008.03.17 by jun
	vector<TNPCMtrl*>	vecMtrls;			///< Sub Material
	string				strClan;

	unsigned int		nSkinColor;					///< 피부색(인간형만 적용)
	unsigned int		nHairColor;					///< 머리색(인간형만 적용)

public:
	TNPCInfo() : CSNPCInfo(), m_bUseMeshParts(false), bAlignTerrain(false), m_bUseTexColor(false)
	{
		for (int i = 0; i < MESH_PARTS_COUNT; i++) nPartsItemID[i] = 0;
	}
	void Clear();
	bool IsUseMeshParts() { return m_bUseMeshParts; }
	bool IsUseTexColor()	{ return m_bUseTexColor; }
};

class TNPCInfoMgr : public CSNPCInfoMgr
{
private:
	

protected:
	void ParseNPC(MXmlElement* pElement, MXml* pXml);
	void ParseNPC_SubMtrl(MXmlElement* pElement, MXml* pXml);
	void ParseNPC_Parts(TNPCInfo* pNPCInfo, MXmlElement* pElement, MXml* pXml);

	unsigned int GetColorAttribute(MXmlElement* pElement, const char* szAttrName);

public:
	TNPCInfoMgr();
	virtual ~TNPCInfoMgr();
	void Clear();
	static TNPCInfoMgr& GetInstance();
	bool Load();
	TNPCInfo* Get(int nID);
	bool Exist(int nID);
	void Save_SubMtrl(const char* filename);
	bool AddSubMtrl_NPC(MXmlElement* pNPCElement, int nNPCID);

	bool					CheckMotionInfoModify();
};

#endif