#pragma once

#include "CSMeshInfo.h"
#include "CSChrInfo.h"

#define NPC_XML_TAG_MAIET			"maiet"
#define NPC_XML_TAG_NPC				"NPC"

#define NPC_XML_ATTR_ID				"id"
#define NPC_XML_ATTR_PARENT			"parent"
#define NPC_XML_ATTR_NAME			"Name"
#define NPC_XML_ATTR_MESH_NAME		"MeshName"
#define NPC_XML_ATTR_MAX_HP			"MaxHP"
#define NPC_XML_ATTR_AP				"AP"
#define NPC_XML_ATTR_HUGE			"Huge"
#define NPC_XML_ATTR_SCALE			"Scale"
#define NPC_XML_ATTR_MODE			"Mode"
#define NPC_XML_ATTR_SPAWNING_MODE	"SpawningMode"
#define NPC_XML_ATTR_HITTEST_INDEX	"HitTestIndex"
#define NPC_XML_ATTR_GRADE			"Grade"
#define NPC_XML_ATTR_MIN_LEVEL		"MinLevel"
#define NPC_XML_ATTR_MAX_LEVEL		"MaxLevel"

#define NPC_XML_ATTR_SPEED			"Speed"
#define NPC_XML_ATTR_RUN_SPEED		"RunSpeed"
#define NPC_XML_ATTR_FLEE_SPEED_AMP	"FleeSpeedAmp"
#define NPC_XML_ATTR_ROTATE_SPEED	"RotateSpeed"
#define NPC_XML_ATTR_VALUE			"value"
#define NPC_XML_ATTR_TYPE			"Type"
#define NPC_XML_ATTR_RATE			"rate"
#define NPC_XML_ATTR_STATE			"state"
#define NPC_XML_ATTR_ATTACKABLE		"Attackable"
#define NPC_XML_ATTR_COUNTERATTACK	"CounterAttack"
#define NPC_XML_ATTR_COMBATABLE		"Combatable"
#define NPC_XML_ATTR_TAUNTABLE		"Tauntable"
#define NPC_XML_ATTR_GAINEXP		"GainEXP"
#define NPC_XML_ATTR_ROOTED			"Rooted"
#define NPC_XML_ATTR_AA				"AA"
#define NPC_XML_ATTR_SIGHTRANGE		"SightRange"
#define NPC_XML_ATTR_AUTOASSIST			"AutoAssist"
#define NPC_XML_ATTR_AUTOASSIST_RANGE	"AutoAssistRange"
#define NPC_XML_ATTR_ASSIST_AFTER_COMBAT	"AssistAfterCombat"
#define NPC_XML_ATTR_GATHER_TYPE		"GatherType"
#define NPC_XML_ATTR_SHOW_ENEMYINFO	"ShowEnemyInfo"

#define NPC_XML_ATTR_ALTITUDE			"Altitude"
#define NPC_XML_ATTR_GUARDTYPE			"GuardType"
#define NPC_XML_ATTR_INTERACT			"Interact"
#define NPC_XML_ATTR_ITEM_LOOT			"Item_Loot"
#define NPC_XML_ATTR_OPENINING_STATEMENT "OpeningStatement"


#define NPC_XML_ATTR_ICOND				 "ICond"
#define NPC_XML_ATTR_ITYPE				 "IType"
#define NPC_XML_ATTR_IACT				 "IAct"

#define NPC_XML_ATTR_IITEMID			 "IItemID"
#define NPC_XML_ATTR_IITEMCOND			 "IItemCond"
#define NPC_XML_ATTR_IITEMTYPE			 "IItemType"
#define NPC_XML_ATTR_IITEMTARGETCOND	 "IItemTargetCond"
#define NPC_XML_ATTR_IITEMACT			 "IItemAct"

#define NPC_XML_ATTR_FACTION_GAIN_ID	 "f_gain_id"
#define NPC_XML_ATTR_FACTION_GAIN_VAL	 "f_gain_val"
#define NPC_XML_ATTR_FACTION_LOSS_ID	 "f_loss_id"
#define NPC_XML_ATTR_FACTION_LOSS_VAL	 "f_loss_val"


#define NPC_XML_ATTR_BEATEN_MOD_VALUE			"BeatenModValue"
#define NPC_XML_ATTR_BEATEN_MOD_WEIGHT			"BeatenModWeight"

#define NPC_XML_ATTR_KNOCKBACK_MOD_VALUE		"KnockbackModValue"
#define NPC_XML_ATTR_KNOCKBACK_MOD_WEIGHT		"KnockbackModWeight"

#define NPC_XML_ATTR_STUN_MOD_VALUE			"StunModValue"
#define NPC_XML_ATTR_STUN_MOD_WEIGHT		"StunModWeight"

#define NPC_XML_ATTR_DEFLECT_MOD_VALUE		"DeflectModValue"
#define NPC_XML_ATTR_DEFLECT_MOD_WEIGHT		"DeflectModWeight"

#define NPC_XML_ATTR_THROWUP_MOD_VALUE		"ThrowupModValue"
#define NPC_XML_ATTR_THROWUP_MOD_WEIGHT		"ThrowupModWeight"

#define NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE	"KnockdownModValue"
#define NPC_XML_ATTR_KNOCKDOWN_MOD_WEIGHT	"KnockdownModWeight"

#define NPC_XML_ATTR_UPPERED_MOD_VALUE		"UpperedModValue"
#define NPC_XML_ATTR_UPPERED_MOD_WEIGHT		"UpperedModWeight"

#define NPC_XML_ATTR_IMMUNE "Immune_"


#define NPC_XML_ATTR_INTERACTION_LOOT_TIME	"ILootTime"
#define NPC_XML_ATTR_INTERACTION_DISTANCE	"InteractionDistance"

#define NPC_XML_ATTR_VALIDATION				"validation"


#define NPC_XML_VALUE_TYPE_NORMAL			"normal"
#define NPC_XML_VALUE_TYPE_FLOATING			"floating"
#define NPC_XML_VALUE_TYPE_SKY				"sky"
#define NPC_XML_VALUE_TYPE_UNDERGROUND		"underground"

#define NPC_XML_VALUE_TYPE_ABSOLUTE		"ABSOLUTE"
#define NPC_XML_VALUE_TYPE_PERFECT		"PERFECT"
#define NPC_XML_VALUE_TYPE_PARTIAL		"PARTIAL"

#define NPC_XML_ATTR_OBJECT_THINKABLE	"ObjectThinkable"

#define NPC_XML_ATTR_IS_MAJOR_LOD_NPC	"IsMajorLodNPC"


class GReloader;
class GNPCInfo;
struct GColtGroupInfo;
struct GColtCheck;
struct GColtAction;
class GNPCScriptBuilder;
class GEntityActor;
class GNPCInteractionElementInfo;
enum ITRIGGER_TYPE;


/// NPC 정보 관리자 - 싱글턴
class GNPCInfoMgr : public CSNPCInfoMgr, public MTestMemPool<GNPCInfoMgr>
{
private:
	friend GReloader;

	CSMeshInfoMgr		m_MeshInfoMgr;

private:

	bool LoadColt(GNPCInfo* pNPCInfo, GNPCScriptBuilder& nsb);
	// NPC 관련
	void ParseNPC(MXmlElement* pElement, MXml* pXml, int nExpectedID=-1);

	void ParseInteraction( MXmlElement* pElement, GNPCInfo* pNewNPCInfo );
	void ParseInteractionForClick( MXmlElement* pElement, GNPCInfo* pNewNPCInfo );
	void ParseInteractionForItem( MXmlElement* pElement, GNPCInfo* pNewNPCInfo );
	GNPCInteractionElementInfo*  ParseIElementCommon(MXmlElement* pElement, int i, ITRIGGER_TYPE nTriggerTyp);

	void ParseFaction(MXmlElement* pElement, GNPCInfo* pNewNPCInfo );

	void _InitNPCInfo( GNPCInfo* pNewNPCInfo );
	void _BindUsableTalent( GNPCInfo* pNewNPCInfo );
	void _HandleProcessNPCInfo( GNPCInfo* pParentNPCInfo, GNPCInfo* pNewNPCInfo );

	// COLT 관련
	void ParseCOLT(MXmlElement* pElement, MXml* pXml, GNPCInfo* pNPCInfo);
	void ParseScript(MXmlElement* pElement, MXml* pXml, GNPCScriptBuilder& nsb);
	void ParseCOMBAT(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParseREACTION(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParseAGGRO(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParseSTRESS(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParsePROLOGUE(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParseEPILOGUE(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParseIDLE(MXmlElement* pElement, MXml* pXml, GColtGroupInfo& ai);
	void ParseCHECK(MXmlElement* pElement, MXml* pXml, GColtCheck* Check, bool bIsCombat);
	void ParseDEFAULT(MXmlElement* pElement, MXml* pXml, GColtCheck& Check, bool bIsCombat);
	void ParseACTION(MXmlElement* pElement, MXml* pXml, GColtAction& Action, bool bIsCombat);
	

public:
	GNPCInfoMgr();
	virtual ~GNPCInfoMgr();
	
	void				Clear();
	virtual bool		Load(const wchar_t* szFileName, int nExpectedID=-1);
	void				Insert(GNPCInfo* pNPCInfo);
	void				Erase( int nID );
	bool				Exist(int nID);
	GNPCInfo*			Find(int nID);
	const set<int>&		CollectQuestNPC();
	bool				IsQuestNPC(int nID);		//퀘스트와 관련된 NPC인지(시작,보상)
	
	// 충돌메쉬 관련
	void				InitMeshInfo();

	void				Cooking();	
};
