#pragma once

#include <string>
#include "MTstring.h"
#include <vector>
#include "CSSoul.h"
#include "CSCommonLib.h"
#include "CSModifier.h"

#include "MTypes.h"

using namespace std;


/// DB에서 가저온 아이템을 분류하기 위해서 사용된다.
const enum SH_ITEM_SLOT_TYPE
{
	SLOTTYPE_NONE			= -1,
	
	SLOTTYPE_DELETED		= 0,
	SLOTTYPE_INVENTORY		= 1,
	SLOTTYPE_EQUIP			= 2,
	SLOTTYPE_STORAGE		= 3,
	SLOTTYPE_GUILDSTORAGE	= 4,

	SLOTTYPE_MAIL			= 101,	// 삭제 예정.		
};

/// 캐릭터 아이템 슬롯
const enum SH_ITEM_SLOT 
{
	ITEMSLOT_NONE = 255,		// 장착불가능한 아이템. 255값은 서버에서 상수로 사용함.
	ITEMSLOT_ALL = ITEMSLOT_NONE,

	// DEFAULT =============================
	ITEMSLOT_ARMOR_START = 0,
	ITEMSLOT_HEAD = ITEMSLOT_ARMOR_START,	//모자.
	ITEMSLOT_FACE,	//선글라스등.
	ITEMSLOT_HANDS,
	ITEMSLOT_FEET,
	ITEMSLOT_BODY,
	ITEMSLOT_LEG,
	ITEMSLOT_ARMOR_END,
	
	ITEMSLOT_LFINGER = ITEMSLOT_ARMOR_END,
	ITEMSLOT_RFINGER,
	ITEMSLOT_NECK,
	ITEMSLOT_CHARM,
	ITEMSLOT_LWEAPON,
	ITEMSLOT_RWEAPON,
	ITEMSLOT_LWEAPON2,
	ITEMSLOT_RWEAPON2,
	ITEMSLOT_DEFAULT_BEGIN = ITEMSLOT_HEAD,
	ITEMSLOT_DEFAULT_MAX = ITEMSLOT_RWEAPON2,
	// DEFAULT ============================^

	// LOOK ================================
	ITEMSLOT_LOOK_HEAD,
	ITEMSLOT_LOOK_HANDS,
	ITEMSLOT_LOOK_FEET,
	ITEMSLOT_LOOK_BODY,
	ITEMSLOT_LOOK_LEG,
	ITEMSLOT_LOOK_BEGIN = ITEMSLOT_LOOK_HEAD,
	ITEMSLOT_LOOK_MAX = ITEMSLOT_LOOK_LEG,
	// LOOK ===============================^

	ITEMSLOT_INN_CRAFTING,
	ITEMSLOT_INN_BED,
	ITEMSLOT_INN_STEWARD,
	ITEMSLOT_INN_CARPET,
	ITEMSLOT_INN_FIREPLACE,
	ITEMSLOT_INN_MURAL,
	ITEMSLOT_INN_TABLE,
	ITEMSLOT_INN_FUNITURE,
	
	ITEMSLOT_MAX
};

const enum SH_ITEM_SWITCH_WEAPON
{
	ITEMSWITCH_PRIMARY = 0,
	ITEMSWITCH_SECONDARY,
};


static const TCHAR* CSItemSlotStr[] =		// 임의로 정했음, xml에 이걸로 기술.
{
	_T("head"),
	_T("face"),
	_T("hands"),
	_T("feet"),
	_T("body"),
	_T("leg"),
	_T("lfinger"),
	_T("rfinger"),
	_T("neck"),
	_T("charm"),

	_T("lweapon"),
	_T("rweapon"),
	_T("lweapon2"),
	_T("rweapon2"),

	_T("look_head"),
	_T("look_hands"),
	_T("look_feet"),
	_T("look_body"),
	_T("look_leg"),

	_T("inn_crafting"),
	_T("inn_bed"),
	_T("inn_steward"),
	_T("inn_carpet"),
	_T("inn_fireplace"),
	_T("inn_mural"),
	_T("inn_table"),
	_T("inn_funiture"),

	_T("none")
};


const enum SOUND_TYPE
{
	SOUND_TYPE_UNIVERSAL = 0,// 
	SOUND_TYPE_CLOTH,		// 
	SOUND_TYPE_LEATHER,		// 
	SOUND_TYPE_CHAIN,		// 
	SOUND_TYPE_IRON,		// 
	SOUND_TYPE_WOOD,		// 
	SOUND_TYPE_CUTLERY,		// 
	SOUND_TYPE_PAPER,		// 
	SOUND_TYPE_STONE,		// 
	SOUND_TYPE_LIQUID,		// 
	SOUND_TYPE_JEWEL,		// 
	SOUND_TYPE_ORGAN,		// 
	SOUND_TYPE_HAMMER,		// 
	SOUND_TYPE_COIN,		// 

	SOUND_TYPE_MAX
};

/// 아이템 타입
const enum SH_ITEM_TYPE
{
	ITEMTYPE_NONE = 0,

	ITEMTYPE_WEAPON,			// 무기
	ITEMTYPE_ARMOR,				// 방어구 및 악세사리
	ITEMTYPE_HOUSING,			// 하우징

	ITEMTYPE_USABLE,			// 사용성
	
	ITEMTYPE_AMMUNITION,		// 어뮤니션
	ITEMTYPE_INVENTORY,			// 사용되지 않고 자리만 차지하는 아이템	

	ITEMTYPE_GATHER,			// 채집도구

	ITEMTYPE_ENCHANTSTONE,		// 강화
	ITEMTYPE_DYE,				// 염색

	//SoulHunterZ
	ITEMTYPE_ATTUNESTONE,

	ITEMTYPE_COSTUME,

	ITEMTYPE_MAX
};

/// 아이템 등급
enum ITEM_TIER
{
	ITEM_TIER_VERY_COMMON = 0,		// 매우 흔함
	ITEM_TIER_COMMON,				// 흔함
	ITEM_TIER_RARE,					// 희귀
	ITEM_TIER_TREASURE,				// 보물
	ITEM_TIER_LEGENDARY,			// 전설
	ITEM_TIER_EPIC,					// 에픽
	ITEM_TIER_MAX
};

/// 무기 타입
enum WEAPON_TYPE
{
	WEAPON_NONE			= 0,
	WEAPON_1H_SLASH		= 1,	// 베기 속성의 한손 무기
	WEAPON_1H_BLUNT		= 2,	// 파쇄 속성의 한손 무기
	WEAPON_1H_PIERCE	= 3,	// 찌르기 속성의 한손 무기
	WEAPON_TWO_HANDED	= 4,	// 양손 검
	WEAPON_STAFF		= 5,	// 지팡이
	WEAPON_ARCHERY		= 6,	// 활
	WEAPON_2H_BLUNT		= 7,	// 양손 둔기

	WEAPON_DUAL_WIELD	= 8,	// 무기가 다른 양손	
	WEAPON_DUAL_PIERCE	= 9,	// 양손 단검
	
	WEAPON_MAX,
	WEAPON_ALL,
};

// 무기별 이동속도
static const int MoveSpeedByWeapon[WEAPON_MAX] = 
{
	375,
	375,
	375,
	375,
	300,
	350,
	350,
	300,
	450,
	375,
};

/// 방어구 타입
enum ARMOR_TYPE
{
	ARMOR_NONE		= 0,
	ARMOR_CLOTH		= 1,	// 천옷
	ARMOR_LIGHT		= 2,	// 가죽 갑옷
	ARMOR_MEDIUM	= 3,	// 체인 갑옷
	ARMOR_HEAVY		= 4,	// 판금 갑옷
	ARMOR_SHIELD	= 5,	// 방패
	ARMOR_ACCESSORY	= 6,	// 악세사리
	ARMOR_AMULET	= 7,	// 애뮬렛(영혼 흡수)

	ARMOR_MAX
};

/// 사용 타입
enum USABLE_TYPE
{
	USABLE_NONE				= 0,
	USABLE_TALENT_USE		= 1,	// 탤런트 사용
	USABLE_TALENT_TRAIN		= 2,	// 탤런트 학습
	USABLE_TALENT_UNTRAIN	= 3,	// 탤런트 초기화
	USABLE_INTERACTION		= 4,	// NPC와 인터랙션
	USABLE_INTERACTION_DEAD	= 5,	// 죽은 NPC와 인터랙션
	USABLE_QUEST_ADD		= 6,	// 퀘스트 추가	

	USABLE_MAX
};

/// 보여지기 위한 아이템 슬롯
enum SH_FEATURE_ITEMSLOT
{
	FEATURE_ITEMSLOT_HEAD = 0,
	FEATURE_ITEMSLOT_FACE,
	FEATURE_ITEMSLOT_HANDS,
	FEATURE_ITEMSLOT_FEET,
	FEATURE_ITEMSLOT_BODY,
	FEATURE_ITEMSLOT_LEG,
	FEATURE_ITEMSLOT_LWEAPON,
	FEATURE_ITEMSLOT_RWEAPON,
	FEATURE_ITEMSLOT_LWEAPON2,
	FEATURE_ITEMSLOT_RWEAPON2,
	FEATURE_ITEMSLOT_MAX,
	FEATURE_ITEMSLOT_NONE = FEATURE_ITEMSLOT_MAX,
};

enum ENCHANT_COLOR
{
	ENCHANT_NONE = 0,
	ENCHANT_RED,
	ENCHANT_ORANGE,
	ENCHANT_BLUE,
	ENCHANT_GREEN,
	ENCHANT_WHITE,
	ENCHANT_YELLOW,
	ENCHANT_S,
	ENCHANT_N,
};

static const TCHAR* ENCHANT_COLOR_STR[] =		// 임의로 정했음, xml에 이걸로 기술.
{
	_T("X"),
	_T("R"),
	_T("O"),
	_T("B"),
	_T("G"),
	_T("W"),
	_T("Y"),
	_T("S"),
	_T("N"),
};



struct CSItemMaterial
{
	int				nID;		///< 현재 Mtrl 정보의 Mesh 인덱스
	int				nSubCount;	///< Multi/Sub Mtrl의 개수. Standard 이면 1개.
	vector<string>	vecSubMtrls;///< 실제 참조할 RMaterial의 인덱스.
};

struct ITEM_SLOT_INFO
{
	SH_ITEM_SLOT_TYPE nSlotType;
	int nSlotID;

	ITEM_SLOT_INFO()
	: nSlotType(SLOTTYPE_NONE)
	, nSlotID(0)
	{}

	ITEM_SLOT_INFO(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID)
	: nSlotType(nSlotType)
	, nSlotID(nSlotID)
	{}
};

enum DAMAGE_ATTRIB;
typedef vector<CSItemMaterial*>		CSItemMaterialVector;

/// 모든 아이템 인스턴스가 공유할 수 있는 파라미터.
class CSCOMMON_API CSItemData
{
public:
		int				m_nID;				///< 아이템의 고유한!!! ID
		tstring			m_strName;			///< 아이템의 이름 - 문자열테이블 키워드($$ITEM_NAME_#)
		tstring			m_strDesc;			///< 아이템 설명 - 문자열테이블 키워드($$ITEM_DESC_#)
		ITEM_TIER		m_nTier;			///< 아이템 등급

		SH_ITEM_TYPE	m_ItemType;			///< 아이템의 종류
		SH_ITEM_SLOT	m_nItemSlot;		///< 아이템이 어디에 장착되느냐
		SH_ITEM_SLOT	m_nSubSlot;			///< 추가로 사용하는 슬롯

		WEAPON_TYPE		m_nWeaponType;		///< 무기 타입
		DAMAGE_ATTRIB	m_nDamageAttrib;	///< 데미지타입
		bool			m_bLeftWeaponEquipable;	///< 무기인 경우 왼손에도 장비 가능한지 여부
		
		SOUND_TYPE		m_ItemSoundType;	///< 아이템 사운드 종류
		ARMOR_TYPE		m_nArmorType;		///< 방어구 타입

		USABLE_TYPE		m_nUsableType;		///< 사용 타입
		vector<int>		m_vecUsableParam;	///< 사용 타입이 사용하는 파라미터
		
		bool			m_bUsableConsume;	///< 사용 타입이 사용후 소모되는지 여부

		GATHER_TYPE		m_nGatherType;		///< 채집 종류

		int				m_nHousingNPC;		///< 하우징 아이템을 장착하면 방에 생기는 NPC

		int				m_nStackAmount;		///< 이 아이템이 몇개까지 겹칠 수 있느냐 - 기본값은 1

		int				m_nMaxDurability;	///< 최대내구

		int				m_nBaseBuyingPrice;		///< 플레이어가 상점에서 사는 기본 가격
		int				m_nBaseSellingPrice;	///< 플레이어가 상점에 파는 기본 가격

		int				m_nMinDamage;		///< 최소 데미지
		int				m_nMaxDamage;		///< 최대 대미지
		int				m_nAP;				///< 방어력	
		int				m_nMagicAP;			///< Magic Armor

		bool			m_bTradable;		///< 거래 가능 여부
		bool			m_bSellable;		///< 판매 가능 여부
		bool			m_bDiscardable;		///< 버리기 가능 여부		
		bool			m_bClaimRequired;	///< 착용시 귀속 되는지?
		bool			m_bUnique;			///< 하나만 소유 가능한지?
		bool			m_bQuestRelated;	///< 퀘스트 관련 아이템인지?
		bool			m_bRecipeMaterial;	///< 레시피 재료인지?
		float			m_fCraftOptionRate;	///< 생산 부가 확률

		int				m_nRepairGrade;		///< 수리 난이도
		int				m_bRepairable;		///< 수리 가능 여부

		SOUL_TYPE		m_nSoulType;		///< 영혼 종류
		uint8			m_nSoulMaxQuantity;	///< 영혼 최대 흡수량
		int				m_nSoulLinkedItem;	///< 영혼을 최대로 흡수했을때 변경되는 영혼석 ItemID

		//SoulHunterZ
		//int				m_nEXP;
		//int				m_nNextLvlReqXP;
		//uint8			m_nAttunementLevel;

		tstring			m_strMeshName;		///< 아이템 메시 이름, 꼭 장비품이 아니더라도 바닥에 굴러다니거나 할때 필요? ...하진 않은가?
		bool			m_bMeshNameBySex;	///< 성별로 다른 아이템 메시 이름인지 여부, ex) meshname= "01" 이면 female_01, male_01 로 각각 대체
		tstring			m_strParentName;	///< 아이템이 붙을 노드 이름
		tstring			m_strParentNameLeft;///< 왼손에 붙을 경우 아이템이 붙을 노드 이름(무기)

		vector<tstring>	m_strNodeNames;		///< 아이템의 노드 이름
		tstring			m_strMeshPath;		///< 메시 경로

		unsigned int	m_nTexColor;		///< 텍스쳐 컬러
		bool			m_bUseTexColor;		///< 텍스쳐 컬러를 사용하는가?
		tstring			m_strSwordTrailEffect; ///< 검광 이펙트(무기에만 쓰입니다.)

		float			m_fSpellPower;

		int				m_nConditionsInfoID;	///< 장비 or 사용 조건
		vector<int>		m_vecDyeColor;			///< 염색제 색상
		bool			m_bDyeable;				///< 염색 가능 여부

		// 2008.03.17 by jun
		CSItemMaterialVector	m_vecMtrls;		///< Sub Material

		bool			m_bValidation;			///< ResourceValidator 유효성검사를 할지, 말지 결정 - 기본값 true
		tstring			m_strIconName;

		CSPlayerModifier	m_PlayerModifier;	///< Item 기본 보정
		CSActorModifier		m_ActorModifier;	///< Item 기본 보정		

		struct ENCHANT_STONE
		{
			int nLinkedBuffID; // 연결될 버프
			float fSuccessRate; // 성공 확률
			int nLimitLevel;
			bool bAllowAllEquipments;
			vector<SH_ITEM_SLOT>	vecLimitEquipments;
			bool bAllowAllWeapons;
			vector<WEAPON_TYPE>		vecLimitWeapons;
			ENCHANT_COLOR nColor;
			int	nTier;

			ENCHANT_STONE(): nLinkedBuffID(INVALID_BUFF_ID), fSuccessRate(100.0f), nLimitLevel(0), bAllowAllEquipments(true), bAllowAllWeapons(true), nColor(ENCHANT_NONE), nTier(0) {}
		} m_EnchantStone;

		vector<ENCHANT_COLOR> m_vecEquipEnchantColor;
		
public:		
		CSItemData();
		virtual ~CSItemData();
		virtual void Cooking();

		bool IsEnchantStone() const;

		tstring GetMainNodeName() const
		{
			if (m_strNodeNames.empty()) return tstring(_T(""));
			return m_strNodeNames[0];
		}

		void CookNodeNames();
};





#define EQUIPMENT_ITEM_NODE_NAME_ADDITIVE_SUFFIX			_T("_item")