#ifndef _CS_CHRINFO_H
#define _CS_CHRINFO_H

#include "CSCommonLib.h"
#include "CSDef.h"
#include <string>
#include "MTstring.h"
#include <map>
#include <set>
#include <deque>
#include "MUID.h"
using namespace std;

#include "CSItemData.h"
#include "CSModifier.h"
#include "MTypes.h"
#include "CSTalentInfoEnum.h"
#include "CSTypes.h"

/// 종족
enum RACE
{
	RACE_NA		= -1,	///< 종족없음
	RACE_HUMAN,			///< 인간
	RACE_VOLO,			///< 볼로
	RACE_NEBIT,			///< 네빗
	RACE_MAX
};

/// 직업
enum CLASS
{
	ADVENTURER = 0,			// 기본 클래스
	WARRIOR,				// 근거리 전투의 전문가
	HUNTER,					// 화살로 목표를 노린다
	MAGE,					// 마법의 힘을 다루는 자
	DEFENDER,				// 강인한 체력의 소유자
	FENCER,					// 근접 무기의 달인
	RANGER,					// 원거리 무기의 달인
	ASSASSIN,				// 폴발적인 위력의 한방
	SORCERER,				// 원소계 마법의 전문가
	GUARDIAN,				// 신성과 암흑 마법의 전문가
};

/// 성별
enum SEX
{
	SEX_NA		= -1,	///< 성별 없음
	SEX_MALE,			///< 남자	
	SEX_FEMALE,			///< 여자
	SEX_SEXLESS,		///< 무성
	SEX_MAX
};

/// 엔티티 타입
enum ENTITY_TYPE
{
	// 서버 클라이언트 공용
	ET_ENTITY		= 0x1,
	ET_SYNC			= 0x2,
	ET_ACTOR		= 0x4,
	ET_PC			= 0x10,		///< 플레이어
	ET_NPC			= 0x20,		///< NPC - NPC, 몬스터 등 모든 NPC
	ET_ITEM			= 0x40,		///< 아이템
	ET_PROJECTILE	= 0x80,		///< 발사체
	ET_BUFF_ENTITY	= 0x100,	///< 지속시간있는 범위마법
	ET_ENTITY_BPART	= 0x200,	///< breakable part
	// 서버만 사용

	// 클라이언트만 사용
	ET_EFFECT		= 0x100,
};

enum ENTITY_TYPEID
{
	ETID_ENTITY		= 0,
	ETID_SYNC		= 1,
	ETID_ACTOR		= 2,
	ETID_PLAYER		= 3,
	ETID_NPC		= 4,
	ETID_ITEM		= 5,
	ETID_PROJECTILE	= 6,
	ETID_BUFF_ENTITY= 7,
	ETID_CUTSCENE	= 8,

	ETID_MAX
};

// 캐릭터 스탠스
enum CHAR_STANCE
{
	CS_NORMAL = 0,
	CS_BATTLE = 1
};

enum CHAR_DOING
{
	CD_NORMAL = 0,
	CD_INTERACTING,					///< 인터랙션
	CD_TRADING,						///< 교환
	CD_LOOTING,						///< 루팅
	CD_ENCHANTING,					///< 강화
	CD_DYEING,						///< 염색
	CD_CUTSCENING,					///< 컷씬
};

enum ALTITUDE_TYPE
{
	AT_NORMAL=0,			// 땅위
	AT_FLOATING,			// 땅위에 붙어있지만 떠있는(박쥐)
	AT_SKY,					// 하늘 위
	AT_UNDERGROUND,			// 땅속

	AT_SIZE
};

#define PLAYER_DEFAULT_HPMAX		100
#define PLAYER_DEFAULT_ENMAX		100
#define PLAYER_DEFAULT_SPMAX		100
#define PLAYER_DEFAULT_SPEED		100.0f

#define PLAYER_DEFAULT_FR			0
#define PLAYER_DEFAULT_CR			0
#define PLAYER_DEFAULT_PR			0
#define PLAYER_DEFAULT_LR			0
#define PLAYER_DEFAULT_HR			0
#define PLAYER_DEFAULT_UR			0

#define PLAYER_DEFAULT_STR			50
#define PLAYER_DEFAULT_DEX			50
#define PLAYER_DEFAULT_INT			50
#define PLAYER_DEFAULT_CHA			50
#define PLAYER_DEFAULT_CON			50

#define BASE_HP_RACE_HUMAN		(50)		// HP race - 인간 기본 체력
#define BASE_EN_RACE_HUMAN		(50)		// EN race - 인간 기본 에너지
#define BASE_SP_RACE_HUMAN		(50)		// SP race - 인간 기본 스태미너

/// 캐릭터의 저항치
struct CSActorResistances
{
	uint8	nValue[TRT_MAX];	// TRT_CHA는 사용하지 않음.

	CSActorResistances();
};

struct CSActorMFModifier
{
	PerMulVar<short>	Weight[MF_PRIORITY_SIZE];
	PerMulVar<short>	Value[MF_PRIORITY_SIZE];
};

/// 캐릭터(PC,NPC) 공통 정보
class CSCOMMON_API CHR_INFO
{
public:
	int		nLevel;			///< 레벨(1~60)

	// Attributes
	int		nMaxHP;			///< Max HP
	int		nMaxEN;			///< Max EN
	int		nMaxSTA;		///< Max STA
	int		nAP;			///< 아머 포인트
	int		nABS;			///< 물리 데미지 흡수력
	int		nHPRegen;		///< 초당 HP 자연 회복량
	int		nENRegen;		///< 초당 EN 자연 회복량
	int		nSTARegen;		///< 초당 STA 자연 회복량
	float	fSpeed;			///< 이동 속도
	
	CSActorMFModifier		MFModifier;
	CSActorResistances	Resist;	

	bool	bRooted;		///< 움직일 수 없다.

	ALTITUDE_TYPE		nAltitude;

	CHR_INFO();
};

struct CSCOMMON_API CHR_STATUS
{
public:
	int nHP;
	int nEN;
	int nSTA;
	CSInstantModifier	InstantModifier;
	CSActorModifier		ActorModifier;

	CHR_STATUS();
};

/// 이동 플래그 - 2byte만 사용하므로 플래그가 2바이트를 넘으면 안된다.
enum MOVEMENT_FLAG
{
	MOVEMENTFLAG_FORWARD		= 0x1,
	MOVEMENTFLAG_BACKWARD		= 0x2,
	MOVEMENTFLAG_LEFT			= 0x4,
	MOVEMENTFLAG_RIGHT			= 0x8,
	MOVEMENTFLAG_JUMPING		= 0x10,
	MOVEMENTFLAG_FALLING		= 0x20,
	MOVEMENTFLAG_USING_TALENT	= 0x40,
	MOVEMENTFLAG_MOTION_FACTOR	= 0x80,
	MOVEMENTFLAG_GUARD			= 0x100,
};

/////////////////////////////////////////////////////////////////////////////////////
// PC ///////////////////////////////////////////////////////////////////////////////

/// 플레이어 등급
enum GPlayerGrade
{
	PLAYER_GRADE_NORMAL			= 0,		///< 일반적인 플레이어
	PLAYER_GRADE_GM				= 1,		///< GM
	PLAYER_GRADE_TESTER			= 2,		///< 테스터
	PLAYER_GRADE_DEVELOPER		= 3			///< 개발자
};

// 플레이어 정보
class CSCOMMON_API PLAYER_INFO : public CHR_INFO
{
private:
	bool	m_bGod;					///< 피해를 입지 않음
	int		m_nMoney;				///< 소지한 돈
	int		m_nStorageMoney;		///< 보관함에 보관한 돈

public:
	bool	m_bGhost;					///< 다른 플레이어에게 안보일지 여부

	CID				nCID;				///< CID, DB에서 사용
	GPlayerGrade	nPlayerGrade;		///< 플레이어 등급
	
	RACE	nRace;				///< 종족
	SEX		nSex;				///< 성별
	int8	nFeatureHair;		///< 머리
	int8	nFeatureFace;		///< 얼굴
	short	nFeatureHairColor;	///< 머리색
	short	nFeatureSkinColor;	///< 피부색
	uint8	nEyeColor;			///< 눈색
	uint8	nMakeUp;			///< 화장
	uint8	nVoice;
	uint8	nTattooType;			///< 문신
	uint8	nTattooColor;
	short	nTattooPosX;			///< 문신 좌표 x
	short	nTattooPosY;			///< 문신 좌표 y
	uint8	nTattooScale;		///< 문신 크기

	TCHAR	szName[PLAYER_NAME_LEN+1];				///< 이름
	TCHAR	szSurname[PLAYER_SUR_NAME_LEN+1];		///< 성
	int		nXP;					///< 현재 경험치

	SBID	nSoulBindingID;			///< 소울 바인딩 위치 ID
	
	// Attributes
	int		nSTR;					///< 완력
	int		nDEX;					///< 근접 공격 스피드 보정 및 확률적 회피
	int		nINT;					///< 지력
	int		nCHA;					///< 카리스마?
	int		nCON;					///< HP의 최대량을 결정
	
	int		nGrab;					///< 잡기 능력치
	int		nBodyRun;				///< 몬스터 위에서 뛸 수 있는 능력치
	float	fMerchandize;			///< 물건을 사고 팔때의 할인, 할증율
	short	nFatigueQuantity;		///< 피로도
	uint8	nInnRoomID;				///< 등록한 여관방
	int		nGID;					///< 길드 ID

	int		nBlankTimeSec;			///< 로그아웃 한후 다시 로그인한 시간까지의 경과 시간(초단위).
	int		nPlayTimeSec;			///< 서버에 접속해서 게임을 진행한 시간(초단위).
	int		nConnPlayTimeSec;		///< 서버 접속시 플레이 시간. nPlayTimeSec - nConnnPlayTimeSec로 서버접속후 경과 시간을 구함.
	int		nDeadTime;				///< 마지막으로 죽은 시간(초단위).
	int		nElapsedDeadTimeSec;	///< 마지막으로 죽은 시간 이후로 지난 시간

	int		nLootGainMoney;		///< 접속후 지금까지 루팅으로 획득한 골드

	// Item
	SH_ITEM_SWITCH_WEAPON	m_nWeaponSet;

	// GM
	void SetGod(bool god) { m_bGod = god; }
	bool IsGod() { return m_bGod; }

	void SetMoney(int nMoney);
	int GetMoney();
	void SetStorageMoney(int nMoney);
	int GetStorageMoney();

	int GetDeltaPlayTime();

	float CalcAutoRebirthRemainTime();

	PLAYER_INFO();
};

/////////////////////////////////////////////////////////////////////////////////////
// NPC //////////////////////////////////////////////////////////////////////////////

enum NPC_TYPE
{
	NPC_TYPE_MONSTER = 0,					// 몬스터
	NPC_TYPE_NPC,						// NPC
	NPC_TYPE_OBJECT,					// 맵 오브젝트
	NPC_TYPE_GATHER,					// 채집
	NPC_TYPE_MAX
};

static const TCHAR* NPC_TYPE_STR[NPC_TYPE_MAX] = 
{
	_T("monster"),
	_T("npc"),
	_T("object"),
	_T("gather")
};	

enum NPC_MODE
{
	NPC_MODE_DEFAULT	= 0,
	NPC_MODE_1			= 1,
	NPC_MODE_2			= 2,
	NPC_MODE_3			= 3,
	NPC_MODE_4			= 4,
	NPC_MODE_5			= 5,
	NPC_MODE_6			= 6,
	NPC_MODE_7			= 7,
	NPC_MODE_8			= 8,
	NPC_MODE_9			= 9,
	NPC_MODE_10			= 10,

	NPC_MODE_MAX
};

enum NPC_ATTACKABLE_TYPE
{
	NAT_INVALID			= -1,
	NAT_NONE			= 0,
	NAT_ALWAYS			= 1,
	NAT_FACTION			= 2,

	NAT_MAX
};

enum NPC_RARITY
{
	NPC_RARITY_COMMON = 0,				// 일반
	NPC_RARITY_RARE,
	NPC_RARITY_EPIC,

	NPC_RARITY_MAX
};


#define DEFAULT_NPC_GRADE		3
#define PLAYER_GRADE			3

#define MIN_NPC_GRADE			0
#define MAX_NPC_GRADE			10

// 방어구 데미지 감쇄율 계산에 필요한 방어자 속성
enum ARMOR_ATTRIB
{
	ARMOR_ATT_NEUTRAL = 0,		// 플레이어는 무조건 Neutral이다.
	ARMOR_ATT_DRAGON1,
	ARMOR_ATT_DRAGON2,
	ARMOR_ATT_FIRE,
	ARMOR_ATT_WATER,
	ARMOR_ATT_LIGHTNING,
	ARMOR_ATT_POISON,
	ARMOR_ATT_HOLY,
	ARMOR_ATT_UNHOLY,
	ARMOR_ATT_IRON,
	ARMOR_ATT_STONE,
	ARMOR_ATT_WOOD,
	ARMOR_ATT_ANIMAL,
	ARMOR_ATT_UNDEAD,
	ARMOR_ATT_BUG,
	ARMOR_ATT_MAX
};

static const TCHAR* ARMOR_ATTRIB_STR[ARMOR_ATT_MAX] = 
{
	_T("neutral"),
	_T("dragon1"),
	_T("dragon2"),
	_T("fire"),
	_T("water"),
	_T("lightning"),
	_T("poison"),
	_T("holy"),
	_T("unholy"),
	_T("iron"),
	_T("stone"),
	_T("wood"),
	_T("animal"),
	_T("undead"),
	_T("bug")
};	

struct NPCSoundInfo
{
	tstring			HitSound1;
	tstring			HitSound2;
	tstring			HitSound3;
	tstring			DyingSound;
	tstring			EntitySound;
	tstring			CombatBGM;
	tstring			Aggro;

	void Clear();
};

enum NPC_AA_TYPE
{
	NAAT_INVALID = -1,
	NAAT_NONE = 0,
	NAAT_ALWAYS,
	NAAT_FACTION,

	NAAT_MAX
};

static const TCHAR* NPC_AA_TYPE_STRING[NAAT_MAX] = 
{
	_T("AA_NONE"),
	_T("AA_ALWAYS"),
	_T("AA_FACTION"),
};	

/// NPC 정보
class CSCOMMON_API CSNPCInfo : public CHR_INFO
{
public:
	int				nID;				///< NPC 고유의 ID
	NPC_TYPE		nNpcType;			///< NPC 타입
	tstring			strName;			///< 이름
	tstring			strNameInLua;		///< Lua에서 쓸 이름
	tstring			strSubtitle;		///< 
	tstring			strMeshPath;		///< 모델 폴더
	tstring			strMeshName;		///< 메쉬 이름
	tstring			strBloodEffectName; ///< 출혈 이펙트 이름
	int				nCHA;				///< NPC는 스탯중 CHA만 가진다.

	int				nMaxLevel;			///< NPC는 스폰될 때 nLevel(Min)~nMaxLevel 사이의 레벨에서 랜덤으로 만들어진다. 
	int8			nGrade;				///< 등급
	bool			bHuge;
	float			fRunSpeed;			///< 달리기 속도
	float			fFleeSpeedAmp;		///< 도망치는 속도 보정치
	float			fRotateSpeed;		///< 회전 속도 0이면 회전하는 과정이 따로 없다, 단위는 각도
	float			fScale;				///< 메시 스케일 값
	bool			bShowName;			///<
	bool			bInteract;			///< Interactive 가능여부
	ARMOR_ATTRIB	nArmorType;			///< 방어자 속성
	GUARD_TYPE		nDefaultGuardType;	///< 기본 방어 타입 (기본은 완전방어)

	float			fTitleHeight;		///< 이름 기준 본에서의 높이값
	tstring			strTitleBone;		///< 이름 기준 본의 이름
	
	tstring			strAniPrefix;				///< 애니메이션 접두어

	bool			m_bColTestObject;	///< 충돌 여부

	uint8			nHitTestIndex;		///< 기본으로 사용할 판정 인덱스
	NPC_MODE		nMode;				///< NPC 초기 모드번호(일반, 하늘, 땅속 등)
	NPC_MODE		nSpawningMode;		///< NPC가 스폰할 때 시작할 Mode
	NPC_AA_TYPE		nAA;				///< Always Agressive on PC

	float			fInteractionLootTime;		///< 인터랙션으로 루팅 할때 걸리는 시간
	int				nInteractionDistance;		///< 인터랙션 가능 거리. 이 값이 0이면 공식에 의해서, 0이 아니면 이 값으로 거리를 체크한다.

	bool			bValidation;				///< ResourceValidator 유효성검사를 할지, 말지 결정 - 기본값 true

	NPCSoundInfo	SoundInfo;			///< 사운드 정보

	int				nEatOwnerBuffID;	///< 먹은쪽이 얻는 버프 아이디
	int				nEatTargetBuffID;	///< 먹힌쪽이 얻는 버프 아이디
	float			fEatPeriod;			///< 먹기 효과주기시간

	uint8			m_nFactionLossID;			///< NPC를 죽였을때 잃는 팩션의 종류

	GATHER_TYPE		nGatherType;		///< 채집종류
	bool			bShowEnemyInfo;

	CSNPCInfo();

	void Clear();
};

/// NPC 정보 관리자 - 서버, 클라이언트 각자에서 상속받아 사용해야한다.
class CSCOMMON_API CSNPCInfoMgr : public map<int, CSNPCInfo*>
{
private:

protected:

public:
	CSNPCInfoMgr();
	virtual ~CSNPCInfoMgr();
};


inline CHAR_STANCE NegativeStance(CHAR_STANCE nStance)
{
	if (nStance == CS_NORMAL) return CS_BATTLE;
	return CS_BATTLE;
}

class CSCOMMON_API CSAltitudeHelper 
{
public:
	// Area 판정을 적용할 수 있는 고도인지 여부
	static bool IsAreaHittable(ALTITUDE_TYPE nAltitude);

	// 모션 팩터 적용을 하는지 여부
	static bool IsApplicableToMotionFactor(ALTITUDE_TYPE nAltitude);

	// 충돌이 가능한 고도인지 여부
	static bool IsColliable(ALTITUDE_TYPE nAltitude);
};
#endif // _CS_CHRINFO_H
