#pragma once

// 탤런트 타입
enum TALENT_TYPE
{
	TT_SKILL	= 0,		// 일반적인 탤런트
	TT_WEAPON	= 1,		// 무기에 대응되는 탤런트
	TT_ENCHANT	= 2,		// 아이템에 대응되는 탤런트
	TT_ITEM		= 3,		// 액티브 속성의 아이템에 대응되는 탤런트
	TT_GATHER   = 4,		// 채집 탤런트
};

/// 탤런트 카테고리 - 스킬 타입(TALENT_SKILL_TYPE)의 일반화
enum TALENT_CATEGORY
{
	TC_INVALID = -1,
	TC_COMMON = 0,
	TC_MELEE,
	TC_RANGE,
	TC_MAGIC,
	TC_MAX
};

enum TALENT_DAMAGETYPE
{
	TDT_INVALID = -1,
	TDT_PHYS = 0,
	TDT_MAGIC,
	TDT_MAX
};


/// 탤런트 스타일, 이값은 DB의 RZ_CODE에서도 사용하고 있기 때문에 변경이 있으면 DB도 반영을 해줘야 한다.
enum TALENT_STYLE
{
	TS_NONE = 0,

	TS_DEFENDER,
	TS_BERSERKER,
	TS_ASSASSIN,
	TS_SORCERER,
	TS_RANGER,
	TS_CLERIC,
	TS_COMMON,
	TS_LICENSE,

	TS_MAX	
};

static const TCHAR* TALENT_CATEGORY_STR[] = 
{
	_T("common"),
	_T("melee"),
	_T("range"),
	_T("magic")
};

// 탤런트 스킬 타입
enum TALENT_SKILL_TYPE
{
	ST_NONE = 0,		// 미지정
	ST_MELEE,			// 접근 공격
	ST_ARCHERY,			// 활 공격
	ST_GUN,				// 총 공격(원거리 즉시피해)
	ST_MAGIC,			// 실제로 사용하는 마법 (마법 탤런트 하나에 여러 실제 마법들이 연결됨), 버프도 포함
	ST_GUARD,			// 방어형	
	ST_PASSIVE,			// 패시브
	ST_BUFF,			// 버프
	ST_EXTRA_ACTIVE,	// 특수한 액티브
	ST_EXTRA_PASSIVE,	// 특수한 패시브
	ST_SOCIAL,			// 기능없이 동작만을 취한다 ex)인사
	ST_ITEM,			// 아이템을 이용한 탤런트 사용 ex)투척

	ST_MAX
};

static const TCHAR* TALENT_SKILL_TYPE_STR[] = 
{
	_T("ST_NONE"),
	_T("ST_MELEE"),
	_T("ST_ARCHERY"),
	_T("ST_GUN"),
	_T("ST_MAGIC"),
	_T("ST_GUARD"),
	_T("ST_PASSIVE"),
	_T("ST_BUFF"),
	_T("ST_EXTRA_ACTIVE"),
	_T("ST_EXTRA_PASSIVE"),
	_T("ST_MOVE"),
	_T("ST_ITEM"),
	_T("ST_MAGICAREA"),
};

/// 특수한 패시브 탤런트 타입
enum TALENT_EXTRA_PASSIVE_TYPE
{
	TEPT_NONE		= 0,

	// 무기 숙련 ========================================
	TEPT_BASE_WEAPON_ONE_HANDED,	// 한손 무기 숙련
	TEPT_BASE_WEAPON_1H_SLASH,		// 한손 검 숙련
	TEPT_BASE_WEAPON_1H_BLUNT,		// 한손 둔기 숙련
	TEPT_BASE_WEAPON_1H_PIERCE,		// 단검 숙련
	TEPT_BASE_WEAPON_TWO_HANDED,	// 양손 검 숙련
	TEPT_BASE_WEAPON_STAFF,			// 지팡이 숙련
	TEPT_BASE_WEAPON_ARCHERY,		// 활 숙련
	TEPT_BASE_WEAPON_2H_BLUNT,		// 양손 둔기 숙련
	TEPT_BASE_WEAPON_DUAL_PIERCE,	// 쌍 단검 숙련	
	
	TEPT_BASE_WEAPON_GUN,			// 총 숙련	
	TEPT_BASE_WEAPON_BOOK,			// 책 숙련


	// 기타 숙련 ========================================
	TEPT_BASE_SNEAK,				// 은신 숙련
	TEPT_BASE_ELEMENTAL,			// 마법 숙련
	TEPT_BASE_HOLY,					// 신성 숙련

	// 공격 특화 ========================================
	TEPT_SPECIALIZATION_SLASH,		// 베기 특화
	TEPT_SPECIALIZATION_BLUNT,		// 파쇄 특화
	TEPT_SPECIALIZATION_PIERCE,		// 찌르기 특화
	TEPT_SPECIALIZATION_ELEMENTAL,	// 마법 특화
	TEPT_SPECIALIZATION_HOLY,		// 신성 특화

	// 유틸리티 ========================================
	TEPT_UTILITY_SAFEFALL,			// 기초 낙법
	TEPT_UTILITY_SAFEFALL2,			// 중급 낙법

	// 갑옷 숙련 ========================================
	TEPT_BASE_ARMOR_LIGHT,			// 기초갑옷 숙련
	TEPT_BASE_ARMOR_MEDUIM,			// 중급갑옷 숙련
	TEPT_BASE_ARMOR_HEAVY,			// 상급갑옷 숙련
	
	// 탤런트 ========================================
	TEPT_TALENT_SHOUT_RADIUS,				// 외치기 관련 탤런트들 적용 범위 증가
	TEPT_TALENT_SAZAHU_SPECIALIZATION,		// 사자후 특화
	// ---------------------------------------------

	// 포커스 ======================================
	TEPT_FOCUS_USE_BERSERK,
	TEPT_FOCUS_USE_COUNTER,
	TEPT_FOCUS_BERSERK_DECREASE_MAXBUBBLE,		// 버서커 필요버블 갯수 감소
	TEPT_FOCUS_BERSERK_EXTENDTIME,		// 버서커 지속시간 증가
	TEPT_FOCUS_COUNTER_EXTENDTIME,		// 반격 지속시간 증가
	TEPT_FOCUS_ADVENT_EXTENDTIME,	// 각성 지속시간 증가
	// ---------------------------------------------

	// 여기서부터 제대로
	TEPT_QUICK_RISE,				///< 빠른 기상
	TEPT_QUICK_DRAW,				///< 신속한 전투태세
	TEPT_3COMBO,					///< 3콤보 가능
	TEPT_SWORD_DRAWING,				///< 발도
	TEPT_UPPERCUT,					///< 띄우기
	TEPT_RISING_ATTACK,				///< 기상 공격
	TEPT_GUARD,						///< 방어 전문화
	TEPT_COUNTER,					///< 카운터 어택	
	TEPT_MAGICAL_DYNAMICS,			///< 마법 역학
	TEPT_ITEM_THROWING,				///< (아이템 사용)투척
	TEPT_ITEM_TRAP,					///< (아이템 사용)덫 사용


	TEPT_MAX,
};

/// 특수한 액티브 탤런트 타입
enum TALENT_EXTRA_ACTIVE_TYPE
{
	TEAT_NONE		= 0,
	// pc만 사용 ----
	TEAT_PORTAL,			// 포탈
	TEAT_PORTAL_SOULBINDING,// 포탈 - 소울바인딩
	TEAT_REBIRTH,			// 부활
	TEAT_TAUNT,				// 도발
	TEAT_BUFF_ENTITY,		// 버프 엔티티 생성
	TEAT_SPAWN,				// 스폰

	// 포커스 관련 ------------
	TEAT_TOGGLE_PRECISION,		// 저격 토글
	TEAT_TOGGLE_SNEAK,			// 은신 토글
	TEAT_ADVENT,				// 강신
	// pc만 사용 ----^

	// npc만 사용 ----
	TEAT_NPC_ONLY_GRAPPLE,		// 잡기, 물기
	TEAT_NPC_ONLY_EAT,			// 먹기
	// npc만 사용 ----^


	// 디스펠 
	TEAT_DISPEL,
	TEAT_CURE_POISON,
	TEAT_CURE_DISEASE,
	TEAT_CURE_CURSE,
	TEAT_DISPEL_MESMERIZE,
	TEAT_DISPEL_DEATHPENALTY,
	TEAT_DISPEL_BUFF,
	TEAT_DISPEL_FOCUS,
	TEAT_DISPEL_BPART,

	TEAT_MAX,
};

/// 인공지능을 위한 탤런트 카테고리
enum TALENT_AI_CATEGORY
{
	TAC_UTILITY_CLASS		= 0,
	TAC_OFFENSIVE_DD		= 1,
	TAC_OFFENSIVE_DOT		= 2,
	TAC_OFFENSIVE_AE		= 3,
	TAC_OFFENSIVE_PB		= 4,
	TAC_OFFENSIVE_DEBUFF	= 5,
	TAC_DEFENSIVE_BUFF		= 6,
	TAC_DEFENSIVE_HEALING	= 7,
	TAC_DEFENSIVE_ENCHANT	= 8,
};

/// 기본 공격의 타입
enum TALENT_NORMAL_ATTACK_TYPE
{
	NA_NA		= -1,
	NA_NORMAL1 = 0,			// 일반 공격 콤보1
	NA_NORMAL2,				// 일반 공격 콤보2
	NA_NORMAL3,				// 일반 공격 콤보3	
	NA_NORMAL4,				// 일반 공격 콤보4
	NA_NORMAL5,				// 일반 공격 콤보5
	NA_NORMAL6,				// 일반 공격 콤보6
	NA_ADVANCE,				// 전진 공격
	NA_DODGE_FRONT,			// 텀블링
	NA_DODGE_BACK,
	NA_DODGE_LEFT,
	NA_DODGE_RIGHT,
	NA_UPPER,				// 띄우기
	NA_DOWN,				// 하단 공격
	NA_SUPER,				// 무기별 특수 공격
	NA_RISING,				// 기상 공격
	NA_SWORD_DRAWING,		// 발도 공격
	NA_UPPERCOMBO1,			// 띄우기 콤보
	NA_COUNTER_ATTACK,		// 카운터 어택

	// 여기서부터는 클라이언트만 사용(콤보 테이블 만들기 위함 - birdkr
	NA_USE_TALENT__CLIENT_ONLY,			
	NA_GUARD__CLIENT_ONLY,

	NA_MAX
};

// 발사체 타입
enum TALENT_PROJECTILE_TYPE
{
	TPT_NONE = 0,
	TPT_MISSILE,			// 직선으로 날아감
	TPT_MISSILE_GUIDED,		// 유도
	TPT_HITCAPSULE_GUIDED	// 판정캡슐 유도
};

// 발사체 비쥬얼 타입
enum TALENT_PROJECTILE_VISUAL_TYPE
{
	TPVT_CURVE,				// 유도
	TPVT_STRAIGHT,			// 미사일
	TPVT_PARABOLA			// 포물선
};

/// 탤런트 사용의 단계
enum TALENT_PHASE
{
	TALENT_PHASE_NONE = -1,
	TALENT_PHASE_PREPARE=0,		// 준비 단계(캐스팅)
	TALENT_PHASE_ACT,			// 실제로 실행
	TALENT_PHASE_POST_DELAY,	// Post Delay 처리
	TALENT_PHASE_FINISH,		// 마무리
	TALENT_PHASE_CANCELED,		// 중간에 취소됨
	TALENT_PHASE_EXTRA,			// 탤런트 타입에 따라 유동적으로 사용할 수 있는 단계 - 보통은 사용하지 않는다.
	TALENT_PHASE_EXTRA2,		// 탤런트 타입에 따라 유동적으로 사용할 수 있는 단계 - 보통은 사용하지 않는다.
	TALENT_PHASE_EXTRA3,		// 탤런트 타입에 따라 유동적으로 사용할 수 있는 단계 - 보통은 사용하지 않는다.
	TALENT_PHASE_MAX
};

/// 탤런트 계열
enum TALENT_LINE
{
	TL_CLOSE_COMBAT_MASTERY=1,	///< 근접 공격 마스터리
	TL_SLASH_MASTERY=2,			///< 베기 마스터리
	TL_REVIVE1=1001,			///< 부활 계열
	TL_REVIVE2=1002,			///< 부활 계열
};


/// 탤런트 이벤트 목록
enum TALENT_EVENT
{
	TE_NONE					= 0,
	TE_FIRE_EFFECT			,
	TE_LAUNCH_PROJECTILE	,		// 발사체 발사 - 클라이언트, 서버 사용
	TE_EFFECT				,		// 이펙트 출력 - 클라이언트 전용
	TE_SOUND				,		// 사운드 출력 - 클라이언트 전용
	TE_CAMERA				,		// 카메라 효과 - 클라이언트 전용
	TE_DAMAGE				,		// 데미지 발생 - 클라이언트 전용
	TE_CUSTOM				,		// 사용자 효과 - 클라이언트 전용
	TE_DELAYED_ACT			,		// 지연 효과   - 클라이언트, 서버 사용
	TE_EXTRAACTIVE_1		,		// Extra Active 1 - 서버 전용
	TE_EXTRAACTIVE_2		,		// Extra Active 2 - 서버 전용
	TE_CAMEAR_LOCK			,		// 카메라 연출용 -  클라이언트 전용
	TE_GROUND_FIRE_EFFECT	,		// 지형 발사 이펙트 - 클라이언트 전용
	TE_MAX
};



enum TALENT_CONDITION
{
	TC_NONE = 0,
	TC_USE_TALENT,			// 탤런트르 사용한 경우
	TC_FINISH_TALENT,		// 탤런트르 완료한 경우
	TC_BUFF_GAIN,			// 버프가 걸린 경우
	TC_BUFF_GONE,			// 버프를 잃은 경우
	TC_BUFF_GAIN_N_PERIOD,	// 버프의 걸릴 떄와 주기적 효과
	TC_BUFF_PERIOD,			// 버프의 주기적 효과
	TC_BUFF_DISPELLED,		// 버프가 디스펠 됐을 때
	TC_BUFF_DUPLICATED,		// 버프가 중복해서 걸렸을 때
	TC_BUFF_EXPIRED,		// 버프가 제거 됐을 때
	TC_BUFF_HIT,			// 버프로 피격되었을 때
	TC_BUFF_MAX_STACKED,	// 버프가 최대로 중첩되었을 때
	TC_BUFF_STACKED,		// 버프가 중첩되었을 때
	TC_HIT_TALENT,			// 탤런트로 피격했을때
	TC_HIT_SUCCESS,			// 탤런트로 피격을 한번이라도 시했을때
	TC_HIT_ALL_TALENT,		// 탤런트로 모두 피격시켰을때 (Hitcapsule type only)
	TC_HIT_ME,				// 내가 피격 당했을때
	TC_MISS_ME,			// 내가 공격을 회피했을때
	TC_GUARD_ME,			// 내가 공격을 방어했을때
	TC_PERFECT_GUARD_ME,	// 내가 완벽 방어, 절대 방어했을때
	TC_CRITICAL_HIT_ME,		// 내가 공격을 크리티컬로 피격 당했을때
	TC_HIT_ENEMY,			// 상대가 공격에 피격됐을때
	TC_MISS_ENEMY,			// 상대가 공격을 피했을때
	TC_GUARD_ENEMY,			// 상대가 공격을 방어했을때
	TC_CRITICAL_HIT_ENEMY,	// 상대가 공격에 크리티컬로 피격됐을때
	TC_COUNTER,				// 상대의 공격을 무효화시키고 피격 시켰을 때
	TC_RESIST,				// 주기적인 저항체크 (버프용)
	TC_HIT_ME_N,			// 내가 피격 당한 횟수가 N 이상일 때 (버프용)
	TC_CRIHIT_ME_N,			// 내가 크리티컬 피격 당한 횟수가 N 이상일 때 (버프용)
	TC_DAMAGE,				// 내가 입은 데미지가 N 이상일 때 (버프용)
	TC_AFFECTED,			// Trigging 된 버프의 효과가 작용했을 때 (버프용)
	TC_MAGIC_ACT,			// 마법 발동할 떄
	TC_MAGIC_HIT_ME,		// 마법에 피격됐을 때
	TC_MAGIC_HIT_ENEMY,		// 마법을 가격했을 때
	TC_MELEE_HIT_ME,		// 접근 공격에 피격됐을 때
	TC_MELEE_HIT_ENEMY,		// 접근 공격을 가격했을 때
	TC_DO_SOMETHING,		// 뭔가 적극적인 행동을 했을 때
};

enum TALENT_GUARD_DIRECTION
{
	GUARD_DIRECTION_FRONT = 1,
	GUARD_DIRECTION_BACK  = 2,
	GUARD_DIRECTION_ALL	 = 3
};

/// 탤런트 사용시 사용가능한 무기
enum TALENT_WEAPON_ALLOWED
{
	TALENT_WEAPON_ALLOWED_1HS = 0,		///< 1hs는 사용 불가
	TALENT_WEAPON_ALLOWED_1HB,
	TALENT_WEAPON_ALLOWED_SHD,			///< 쉴드를 꼭 끼고 있어야 발동함
	TALENT_WEAPON_ALLOWED_2HD_H,		///< 현재의 2hd가 _h입니다. 중량형(heavy) 무기라는 뜻
	TALENT_WEAPON_ALLOWED_2HD_L,		///< 미구현. 나중에 구현할겁니다. 경량형 양손검(light)
	TALENT_WEAPON_ALLOWED_1HP_S,		///< 현재의 1hp. short 타입
	TALENT_WEAPON_ALLOWED_1HP_L,
	TALENT_WEAPON_ALLOWED_DWD,
	TALENT_WEAPON_ALLOWED_BOW,
	TALENT_WEAPON_ALLOWED_2HB,
	TALENT_WEAPON_ALLOWED_STF			///< 스태프
};


/// 저항 방식
enum TALENT_RESIST_METHOD
{
	TRM_NONE	= -1,

	TRM_BINARY = 0,		// 완전 저항
	TRM_PARTIAL,		// 부분 저항

	TRM_MAX
};

/// 저항 타입
enum TALENT_RESIST_TYPE
{
	TRT_NONE	= -1,

	TRT_FR		= 0,
	TRT_CR,
	TRT_PR,
	TRT_LR,
	TRT_HR,
	TRT_UR,
	TRT_CHA,

	TRT_MAX
};

enum TALENT_DRAIN_TYPE : unsigned char
{
	TALENT_DRAIN_HP = 0,
	TALENT_DRAIN_EN,
	TALENT_DRAIN_SP,

	TALENT_DRAIN_MAX
};

/// 캔슬 후 연계기 파라메타
enum CANCEL_NEXT_ATTACK_PARAM
{
	CANCEL_NEXT_ATTACK_PARAM_ALL		= 0,		///< 모두 캔슬
	CANCEL_NEXT_ATTACK_PARAM_UPPERED	= 1,		///< uppercombo만 나감
};

