#pragma once

#include "MTypes.h"
#include "MXml.h"
#include <string>
#include "MTstring.h"
#include <map>
#include <set>
#include <deque>
using namespace std;

#include "CSCommonLib.h"
#include "CSMotionFactorInfo.h"
#include "CSItemData.h"
#include "CSChrInfo.h"
#include "CSDef.h"
#include "CSModifier.h"
#include "CSTalentDef.h"
#include "CSTalentInfoEnum.h"
#include "CSEffectInfo.h"

class CSTalentInfo;

// 되받아치기 데이터
struct CSRiposte
{
	TALENT_CATEGORY		nType;
	float				fRate;		// (1 = 100%)
	CSRiposte() : nType(TC_MELEE), fRate(0.0f) {}
};

/// 탤런트 판정 데이터
class CSHitSegment
{
public:
	vector<MCapsule>	m_vCapsules;			///< 충돌 영역 - 캡슐
	vector<bool>		m_vDoHitCheck;			///< 판정시캡슐이 거리계산에 들어갈지 여부 판단
	float				m_fCheckTime;			///< 충돌 체크 시간 - 서버에서 탤런트 시작후 이 시간이 흐른 후에 한번 체크한다.
	int					m_nAddtiveDamage;		///< 추가 데미지 - 탤런트 정보의 m_nDamage에 이 값을 추가해서 데미지를 계산한다.
	float				m_fHitMotionDelay;		///< 피격 애니메이션 딜레이 - 클라이언트에서 각 애니메이션의 자연스러운 연출을 위해 사용. 아직 클라이언트에 적용안됨.
	CSHitSegment() : m_fCheckTime(0.0f), m_fHitMotionDelay(0.0f), m_nAddtiveDamage(0) {}
};

/// 탤런트 판정 정보
class CSCOMMON_API CSTalentHitInfo
{
public:
	int						m_nID;
	vector<CSHitSegment>	m_vSegments;

	// 레퍼런스. TalentInfo Cooking할 때 레퍼런스 정보에서 값을 가져온다
	int						m_nReferenceID;
	int						m_nReferenceMode;

	bool					TestCollision(MCapsule& c);
	void					CopyTo(CSTalentHitInfo& dest)
	{
		// reference 정보는 복사하지 않는다.

		dest.m_nID = this->m_nID;
		dest.m_vSegments = this->m_vSegments;
	}

	CSTalentHitInfo();
	void Clear();
};

/// Spell 정보
class CSSpellInfo
{
public:
	int						m_nTalentID;
	float					m_fStartTime;

	CSSpellInfo(int nID, float fStart) : m_nTalentID(nID), m_fStartTime(fStart) {}
};

/// 발사체 정보
class CSProjectileInfo
{
public:
	// 공통
	TALENT_PROJECTILE_TYPE		m_nType;						// 발사체 타입
	TALENT_PROJECTILE_VISUAL_TYPE m_nVisualType;				// 발사체 비쥬얼 타입
	
	tstring						m_strBoneName;					// 발사 위치를 측정하는 본 이름
	tstring						m_strEffectName;				// 발사체 이펙트
	tstring						m_strAttackEffectName;			// 발사체 피격 이펙트
	tstring						m_strHitGroundEffectName;		// 발사체 지형 피격 이펙트
	tstring						m_strDefenceEffectName;			// 발사체 방어 이펙트
	tstring						m_strFireEffectName;			// 발사체 발사 이펙트

	float						m_fSpeed;						// 발사체 속도
	float						m_fHitRadius;					// 발사체 판정 크기
	float						m_fRange;						// 발사체의 사정거리(미터, m)

	// 서버
	vec3						m_vLocalStartPos;				// 발사 위치 (여성, 기본)

	// 클라
	int							m_nSegmentIndexForHitCapsuleType;				// 판정 세그먼트
	int							m_nCapsuleIndexForHitCapsuleType;				// 판정 캡슐

	CSProjectileInfo()
	{
		m_nType										= TPT_NONE;
		m_nVisualType								= TPVT_STRAIGHT;

		m_strBoneName.clear();
		m_strEffectName.clear();
		m_strAttackEffectName.clear();
		m_strHitGroundEffectName.clear();
		m_strDefenceEffectName.clear();
		m_strFireEffectName.clear();

		m_vLocalStartPos							= vec3::ZERO;

		m_fSpeed									= 100.0f;
		m_fHitRadius								= 30.0f;
		m_fRange									= 0.0f;

		m_nSegmentIndexForHitCapsuleType			= -1;
		m_nCapsuleIndexForHitCapsuleType			= -1;
	}
};

/// 탤런트 이벤트 정보
class CSTalentEventInfo
{
public:
	TALENT_EVENT		m_nEvent;
	float				m_fTime1;
	float				m_fTime2;
	tstring				m_strCustomEvent;
	tstring				m_strParam1;
	tstring				m_strParam2;
	int					m_nParam1;
	int					m_nParam2;
	bool				m_bFollow;
	bool				m_bServerDir;

	vec2				m_vLocal;

	CSProjectileInfo	m_ProjectileInfo;						// 발사체 정보를 이벤트에서 처리

	CSTalentEventInfo() : m_nEvent(TE_NONE), m_fTime1(-1.0f), m_fTime2(-1.0f), m_nParam1(0), m_nParam2(0), m_bFollow(true), m_vLocal(0, 0), m_bServerDir(false) {}

	// map 컨테이너 비교용도
	bool operator() (CSTalentEventInfo& lhs, CSTalentEventInfo& rhs) const
	{
		return lhs.m_fTime1 < rhs.m_fTime1;
	}
};

/// 탤런트 이벤트
class CSTalentEvents
{
public:
	int							m_nRefID;
	int							m_nRefMode;

	vector<CSTalentEventInfo>	m_vecEvents;		///< 일반적인 이벤트
	vector<CSTalentEventInfo>	m_vecActEvents;		///< Act로부터 시간 계산된 이벤트(플레이어 마법 때 사용)

	CSTalentEvents()
	{
		m_nRefID	= 0;
		m_nRefMode	= 0;
	}

	bool HasEvent()
	{
		return !(m_vecEvents.empty() && m_vecActEvents.empty());
	}

	bool HasEvent(TALENT_EVENT eEvent)
	{
		vector<CSTalentEventInfo>::iterator itEvents = m_vecEvents.begin();
		while(itEvents != m_vecEvents.end())
		{
			if((*itEvents).m_nEvent == eEvent)
				return true;

			itEvents++;
		}
		vector<CSTalentEventInfo>::iterator itActEvents = m_vecActEvents.begin();
		while(itActEvents != m_vecActEvents.end())
		{
			if((*itActEvents).m_nEvent == eEvent)
				return true;

			itActEvents++;
		}

		return false;
	}
};

/// 탤런트 정보의 저항 관련
struct GTalentResist
{
	TALENT_RESIST_METHOD		m_nMethod;		///< 저항 방식
	TALENT_RESIST_TYPE			m_nType;		///< 저항 타입
	uint8						m_nDifficulty;	///< 저항 난이도

	GTalentResist() : m_nMethod(TRM_BINARY), m_nType(TRT_NONE), m_nDifficulty(255) {}
};

/// 탤런트 정보의 흡수 관련
struct GTalentDrain
{
	float				m_fRate;
	TALENT_DRAIN_TYPE	m_nType;
	GTalentDrain() : m_fRate(0.0f), m_nType(TALENT_DRAIN_HP) {}
	bool IsDrainTalent() const { return (m_fRate != 0.0f); }
};

/// 판정 필터
struct GTalentHitFilter
{
	bool	bMotionFactor[MF_SIZE];
	bool	bAltitude[AT_SIZE];
	GTalentHitFilter()
	{
		for (int i = 0; i < MF_SIZE; i++) bMotionFactor[i] = true;
		for (int i = 0; i < AT_SIZE; i++) bAltitude[i] = true;

		bMotionFactor[MF_KNOCKDOWN] = false;
		bMotionFactor[MF_THROWUP] = false;
		bMotionFactor[MF_UPPERED] = false;
		bAltitude[AT_SKY] = false;
	}
};

struct CSBuffEnchantInfo
{
	int						nID;	
	CSEffectInfo		EffectInfo;		// 버프를 가지게 되는 대상의 정보
	TALENT_CONDITION	nEnchantEvent;		// 버프를 가지게 되는 조건
	GTalentResist		Resist;

	int					nRange;			// 버프 범위

	CSBuffEnchantInfo() : 
	nID(INVALID_BUFF_ID), nEnchantEvent(TC_NONE) {}
};

// 탤런트 발동조건 정보
struct CSTalentConditionInfo
{
	struct Release
	{
		// 버프 효과를 잃는 조건 타입
		TALENT_CONDITION	nType;
		// 버프 효과를 잃는 추가 조건 타입
		vector<TALENT_CONDITION>	vecAdditionType;
		// 버프 효과를 잃는 조건 인자
		int	nParam;		

		Release()
		{
			nType  = TC_NONE;
			nParam = 0;
		}
	} infoRelease;

	// 버프 효과를 얻는 조건 타입
	TALENT_CONDITION	nEffectConditionType;	


	CSTalentConditionInfo()
	{
		nEffectConditionType	= TC_BUFF_GAIN_N_PERIOD;
	}
};

struct CSCOMMON_API CSTalentWeaponAllowdInfo
{
	set<int>		m_setWeapons;
	bool			m_bAllAllowed;
	bool			m_bNeedEquipShield;
	CSTalentWeaponAllowdInfo()
	{
		m_bAllAllowed = true;
		m_bNeedEquipShield = false;
	}
	bool IsAllowed(TALENT_WEAPON_ALLOWED nWeapon);
	bool IsAllowed(WEAPON_TYPE nWeaponType, bool bEquipShield);
};

struct CSTalentActAnimationTimeInfo
{
	WEAPON_TYPE		nWeaponType;
	float			fAniTime;

	CSTalentActAnimationTimeInfo()
	{
		nWeaponType	= WEAPON_NONE;
		fAniTime	= 0.0f;
	}
};

struct CSTalentActAnimationTime
{
	vector<CSTalentActAnimationTimeInfo> vecInfo;

	CSTalentActAnimationTime()
	{
		vecInfo.clear();
	}

	float GetTime(WEAPON_TYPE nWeaponType = WEAPON_NONE)
	{
		for(vector<CSTalentActAnimationTimeInfo>::iterator itTime = vecInfo.begin(); itTime != vecInfo.end(); itTime++)
		{
			if((*itTime).nWeaponType == nWeaponType)
				return (*itTime).fAniTime;
		}

		return 0.0f;
	}

	CSTalentActAnimationTimeInfo* GetActAnimationTimeInfo(WEAPON_TYPE nWeaponType = WEAPON_NONE)
	{
		for(vector<CSTalentActAnimationTimeInfo>::iterator itTime = vecInfo.begin(); itTime != vecInfo.end(); itTime++)
		{
			if((*itTime).nWeaponType == nWeaponType)
				return &(*itTime);
		}

		return NULL;
	}
};

enum TALENT_FINISH_TYPE
{
	TALENT_FINISH_NONE = 0,
	TALENT_FINISH_THROW_UP,
	TALENT_FINISH_KNOCK_BACK,
	TALENT_FINISH_RANDOM
};

struct CSTalentFinishEffect
{
	TALENT_FINISH_TYPE	m_nFinishType;
	bool				m_bBreakableParts;
	CSTalentFinishEffect() : m_nFinishType(TALENT_FINISH_NONE), m_bBreakableParts(false) {}
};

const int MAX_TALENT_MODE = WEAPON_MAX;

class CSCOMMON_API CSTalentInfoModes
{
public:
	bool			m_bExistMode;
	CSTalentInfo*	m_Modes[MAX_TALENT_MODE];

	CSTalentInfoModes();
	~CSTalentInfoModes();
	void ClearButNotDelete();
};

struct CSTalentInfoNPCChangeMode
{
	bool			m_bChangeMode;
	NPC_MODE		m_nModeTo;
	CSTalentInfoNPCChangeMode() : m_bChangeMode(false), m_nModeTo(NPC_MODE_DEFAULT) {}
};

struct RequireBuff
{
	vector<int> vecIncludeBuffID;		// 해당 버프가 있으면 효과 얻음
	vector<int> vecExcludeBuffID;		// 해당 버프가 있으면 효과 무시
	bool		bKeepIncludeBuff;
	bool		bKeepExcludeBuff;

	RequireBuff(): bKeepIncludeBuff(true), bKeepExcludeBuff(true)	{}
};

enum WEAPON_REFRENCE
{
	WR_NONE = 0,
	WR_LEFT,
	WR_RIGHT,
	WR_BOTH,

	WR_MAX
};

enum MOVING_USE_TYPE
{
	MUT_NONE = 0,
	MUT_CASTING,
	MUT_USE,
	MUT_ALL
};

enum TARGETING_TYPE
{
	TTT_STICKY,
	TTT_LINE,
	TTT_GROUND
};

/// 탤런트 정보
// 사용처는 (클라, 서버, 모두)로 표기한다.
class CSCOMMON_API CSTalentInfo
{
protected:
	bool					m_bCooked;
	CSTalentInfoModes		m_Modes;
public:
	int						m_nID;							// id (모두)
	int						m_nMode;						// mode - weapon 등으로 한 id의 탤런트에 여러개의 TalentInfo를 사용하고 싶을때 사용함(모두)
	int						m_nOverride;


	// 일반 ---------------------------
	tstring					m_szName;						// 탤런트 이름(모두)
	int						m_nHPCost;						// hp cost(Not Used) (모두)
	int						m_nENCost;						// en cost(Not Used) (모두)
	int						m_nSTACost;						// sta cost(Not Used) (모두)
	TALENT_CATEGORY			m_nCategory;					// 카테고리(모두)
	TALENT_DAMAGETYPE		m_nDmgType;
	TALENT_TYPE				m_nTalentType;					// 탤런트 타입 (모두)
	TALENT_SKILL_TYPE		m_nSkillType;					// 탤런트 타입중 스킬일때의 타입 (모두)SwordTrailEffectForFocus
	TALENT_STYLE			m_nStyle;						// 탤런트 스타일
	TALENT_EXTRA_PASSIVE_TYPE	m_nExtraPassive;			// 탤런트 타입이 extra passive일 때 이 항목의 패시브 능력을 가진다. (모두)
	int							m_nExtraPassiveParam;		// 패시브의 추가적인 수치
	TALENT_EXTRA_PASSIVE_TYPE	m_nExtraPassive2;			// 탤런트 타입이 extra passive일 때 이 항목의 패시브 능력을 가진다. (모두)
	int							m_nExtraPassiveParam2;		// 패시브의 추가적인 수치

	TALENT_EXTRA_ACTIVE_TYPE	m_nExtraActive;				// 탤런트 타입이 extra active일 때 해당 스킬을 사용하면 특정한 행동을 한다. (모두)
	int							m_nExtraActiveParam1;		// 액티브의 추가적인 수치
	int							m_nExtraActiveParam2;		// 액티브의 추가적인 수치
	uint32					m_nAICategories;				// AI를 위한 탤런트 타입 (서버)
	TALENT_CONDITION		m_nTiming;					// 발동 조건
	bool					m_bIgnoreMesmerize;				// Disabled 상태를 무시하고 사용 가능한지 여부(모두)
	float					m_fCoolTime;					// 쿨타임(초)
	float					m_fDurationTime;				// 탤런트 지속시간

	int						m_nTalentLine;					// 랭크 라인 (모두)
	int8					m_nRank;						// 랭크 (모두)
	float					m_fCastingTime;					// 캐스팅하는데 걸리는 시간 (모두)
	unsigned int			m_nEnabledStanceFlag;			// 탤런트 사용 가능한 스탠스 (모두)
	bool					m_bPerfectGuardOnly;			// 완벽방어로만 방어가능한지 여부 (모두)
	bool					m_bPartialGuardHit;				// 완벽방어일지라도 부분방어로 처리할지 여부 (모두)
	bool					m_bCancelable;					// 탤런트 사용중 피격시 취소 가능 여부 (모두)
	int						m_nHitCapsuleGroupIndex;		// 탤런트가 사용되는 동안 바뀔 NPC 캡슐그룹번호 (모두)
	int						m_nExtraHitCapsuleGroup;		// 탤런트가 ExtraPhase가 되면 바뀔 NPC 캡슐그룹번호 (모두)
	int						m_nExtraHitCapsuleGroup2;		// 탤런트가 ExtraPhase2가 되면 바뀔 NPC 캡슐그룹번호 (모두)
	int						m_nExtraHitCapsuleGroup3;		// 탤런트가 ExtraPhase3가 되면 바뀔 NPC 캡슐그룹번호 (모두)

	bool					m_bUntrainable;					// 탤런트 초기화 허용여부 (모두)

	bool					m_bAvoidable;					// 회피 가능
	bool					m_bAvailableOnGuard;			// 방어중 사용 가능 여부(모두)


	// 유효성 -------------------------
	CSEffectInfo			m_EffectInfo;					// 탤런트의 효과를 받는 대상 정보
	float					m_fRange;						// 발사체의 사정거리(미터, m)
	float					m_fRequireMoveSpeed;			// 탤런트를 사용할 때 필요한 이동속도

	// Art ----------------------------
	tstring					m_szCastingAnimation;			// 캐스팅시 애니메이션 (모두)
	tstring					m_szCastingLoopAnimation;		// 캐스팅 애니메이션 끝나고 루프 (클라)
	tstring					m_szUseAnimation;				// 탤런트 사용 애니메이션 (모두)
	tstring					m_szExtraActAnimation;			// 탤런트 사용 애니메이션 (모두)
	tstring					m_szExtraActAnimation2;			// 탤런트 사용 애니메이션 (모두)
	tstring					m_szExtraActAnimation3;			// 탤런트 사용 애니메이션 (모두)

	tstring					m_szIcon;						// 아이콘
	tstring					m_szTargetArt;					// 타겟 이펙트(Not Used)
	int						m_nTargetAttachmentPoint;		// 타겟 이펙트 붙는 위치(Not Used)

	MOVING_USE_TYPE			m_nMovableType;				// 이동 가능한 타입인가

	// 공격 ---------------------------
	bool					m_bUseAiming;					// 캐스팅후 Use시 타겟에 맞춰 방향 전환
	int						m_nHateAdj;	
	float					m_fHateFactor;

	float					m_fHitDelay;

	GTalentResist			m_Resist;

	// 판정
	CSTalentHitInfo			m_HitInfo;						// 판정 정보


	float					m_fHitStartTime;				// 판정 시작 시간
	float					m_fHitDurationTime;				// 판정 지속 시간
	vec3					m_vHitPosition;					// 판정 위치
	float					m_fHitSize;						// 판정 크기(일단 구로 판정함)


	// 피격자 효과
	int						m_nEffectTime;					// 효과 지속 시간
	float					m_fEffectAreaMin;				// 효과 최소거리 - melee형만 사용(Not Used)
	float					m_fEffectAngle;					// 효과 판정 각도 - melee형만 사용(Not Used)

	DAMAGE_ATTRIB			m_nDamageAttrib;					// 데미지 종류 (서버)
	int						m_nMinDamage;					// 최소 데미지
	int						m_nMaxDamage;					// 최대 데미지
	uint8					m_nDodgeDifficulty;				///< 회피 난이도(0~255)

	int						m_nMinHeal;						// 최소 회복량
	int						m_nMaxHeal;						// 최대 회복량

	int						m_nHeal;						// 치료(Not Used)
	float					m_fKnockBack;				// KnockBack(Not Used)

	MOTION_FACTOR_GROUP		m_MotionFactorGroup;
	CSTalentEvents			m_Events;						///< 이벤트 정보

	bool					m_bUseBloodEffect;				///< 탤런트사용으로 피격시 출혈 이펙트 사용

	RequireBuff				m_RequireBuff;

	CSBuffEnchantInfo		m_Buff1;
	CSBuffEnchantInfo		m_Buff2;

	CSInstantModifier		m_InstantModifier;
	CSActorModifier			m_ActorModifier;
	CSPlayerModifier		m_PlayerModifier;

	CSTalentWeaponAllowdInfo	m_WeaponAllowedInfo;

	// 검광
	tstring					m_strSwordTrailEffect;

	CSTalentFinishEffect	m_FinishEffect;			// 이 탤런트로 죽였을 때 효과
	// NPC만 사용
	bool					m_bSetDirection;		// 탤런트 쓸때 목표를 바라볼지 여부
	set<int>				m_setNPCID;				// 이 탤런트를 사용하는 NPC ID 집합
	CSTalentInfoNPCChangeMode	m_ChangeModeInfo;	// 모드 변경 정보

	bool					m_bNeedTraining;		// 배워야만 사용할수 있는지

	// 학습 필요 조건
	int						m_nTNeedLevel;			// 레벨
	int						m_nTNeedTalent;			// 선행 탤런트
	int						m_nTNeedStyleTP;		// 투자된 Style TP

	// 포커스 -------------------------
	// 탤런트를 쓰는데 필요한 포커스
	TALENT_FOCUS_TYPE		m_nRequireFocus;
	// 탤런트를 성공적으로 쓴후에 얻게 되는 포커스
	TALENT_FOCUS_TYPE		m_nNextFocus;


	bool					m_bValidation;			///< ResourceValidator 유효성검사를 할지, 말지 결정 - 기본값 true

	CSTalentActAnimationTime m_ActAnimationTime;

	bool					m_bByLicense;		// 아이템 사용으로 학습하는 탤런트
	bool					m_bUsableAtDead;		// 죽은 상태에서 쓸수 있는 탤런트

// CLIENT만 사용 -------------
public:
	tstring					m_szDesc;				///< 설명(클라)
	float					m_fExtraMotionTime;		///< Extra 모션의 플레이 시간 - TOOL에서 사용
	float					m_fExtraMotionTime2;	///< Extra2 모션의 플레이 시간 - TOOL에서 사용
	float					m_fExtraMotionTime3;	///< Extra3 모션의 플레이 시간 - TOOL에서 사용
//	float					m_fHitMotionDelay;		///< 히트 모션의 딜레이 - 사용 안함
	bool					m_bAffectedMotionSpd;	///< 모션 속도에 영향받는 탈랜트인지 여부

	TARGETING_TYPE			m_eTargetingType;		///< 타겟팅 타입
// CLIENT만 사용 -------------^

// SERVER만 사용 -------------
public:
	TALENT_GUARD_DIRECTION		m_nGuardDirection;		///< 방어 스킬일 경우 방어 가능한 방향

	struct _TALENT_MOVE_INFO
	{
		vec3	vPos;
		float	fRot;
		_TALENT_MOVE_INFO() : vPos(vec3::ZERO), fRot(0.0f) {}
		_TALENT_MOVE_INFO(vec3 pos, float rot) : vPos(pos), fRot(rot) {}
	};

	vector<_TALENT_MOVE_INFO>	m_MovingInfoList;

	vec3					m_vCastingPos;			// Casting 탤런트 종료 위치
	float					m_fCastingRot;			// Casting 탤런트 종료 방향
	vec3					m_vCastingLoopPos;		// Casting Loop 탤런트 종료 위치
	float					m_fCastingLoopRot;		// Casting Loop 탤런트 종료 방향
	vec3					m_vExtraPos;			// Extra 탈렌트의 종료 위치
	float					m_fExtraRot;			// Extra 탈렌트의 종료 방향
	vec3					m_vExtra2Pos;			// Extra2 탈렌트의 종료 위치
	float					m_fExtra2Rot;			// Extra2 탈렌트의 종료 방향
	vec3					m_vExtra3Pos;			// Extra3 탈렌트의 종료 위치
	float					m_fExtra3Rot;			// Extra3 탈렌트의 종료 방향

	// 전투 공식 관련 -----------------
	GTalentDrain			m_Drain;
	bool					m_bCriticalEnable;
	float					m_fWeaponApplyRate;		// 무기 대미지 배율 (1 = 100%)
	WEAPON_REFRENCE			m_WeaponReference;		// 계산 공식에서 참조 되는 무기

	float					m_fCriticalApplyRate;	// 추가 치명 배율 (1 = 100%)

	// 전투 관련 ----------------------
	GTalentHitFilter		m_HitFilter;			// 판정 필터
	MF_STATE				m_nForceMF;				// 강제 모션 팩터 - 없으면 NONE이다.
	short					m_nForceMFWeight;		// 강제 모션 팩터 Weight


	// AI -----------------------------
	float					m_fExtraPhaseTime;		///< extra phase 의 시간
	float					m_fExtraPhaseTime2;		///< extra phase 의 시간
	float					m_fExtraPhaseTime3;		///< extra phase 의 시간

	// -----------------------------
	bool					m_bUseInvincibleTime;
	float					m_fInvincibleTimeStart;	// 무적 시작 시간
	float					m_fInvincibleTimeEnd;	// 무적 종료 시간
	bool					m_bUseSuperarmorTime;	// 슈퍼아머(모션팩터 안걸림) 사용 여부
	float					m_fSuperarmorTimeStart;	// 슈퍼아머(모션팩터 안걸림) 시작 시간
	float					m_fSuperarmorTimeEnd;	// 슈퍼아머(모션팩터 안걸림) 종료 시간
	bool					m_bSeparateHit;			///< 분할 판정 여부 - 기본은 해당 탤런트에 한번 판정되면 끝이지만 이 값이 true이면 각각의 캡슐마다 판정한다.
	bool					m_bHitCapsulePosSync;	///< 판정 캡슐 위치가 수행하고 있는 엔티티의 위치를 따라갈지 여부
	float					m_fPostDelay;			// 탤런트 사후 딜레이

	bool					m_bUseAvoidTime;
	float					m_fAvoidTimeStart;	// 회피 시작 시간
	float					m_fAvoidTimeEnd;	// 회피 종료 시간

	int						m_nCooltimeLine;	// 쿨타임 라인 (모두)

	GUARD_CRASH_LEVEL		m_nGuardCrashLevel;	// 가드 크래시 레벨 (가드를 무시 정도)

	// Script -------------------------
	tstring					m_strLuaOnStart;
	tstring					m_strLuaOnAct;
	tstring					m_strLuaOnDelayedAct;
	tstring					m_strLuaOnFinish;
	tstring					m_strLuaOnCancel;
	


	bool IsToggle();
	int GetToggleBuffID();

// SERVER만 사용 -------------^

	CSTalentInfo();
	virtual ~CSTalentInfo()
	{
		m_MovingInfoList.clear();
	}
	virtual void Cooking();
	virtual void CopyAll(CSTalentInfo* pTalentInfo) {};		// 상속받은 클래스가 알아서 처리하기

	bool IsPassiveSkill() const;
	bool IsNeedTarget() const;
	bool IsMagicDamage() const;
	bool HasRequireFocus();
	bool HasNextFocus();
	bool HasDamage();
	bool HasHealEffect();
	bool HasMotionfactor();
	bool ExistMode() { return m_Modes.m_bExistMode; }

	CSTalentInfo* GetMode(int nMode);
	void InsertMode(CSTalentInfo* pModeTalentInfo);
	bool IsCooked() const { return m_bCooked; }

	bool IsPhysicalAttack(void) const;
	bool IsSelfRebirth() const;
};