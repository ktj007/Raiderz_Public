#pragma once

// [2007/5/25 isnara] Talent 이펙트
enum IMPACTPOS
{
	MODEL_POS = 0,			// 모델 위치(원점)
	HIT_POS,				// 맞은 위치
	BONE_POS,				// 본 위치
	SHOCK_GROUND_POS,		// 지형에 충돌(영역에서만 사용)
	TALENT_HIT_CAPSULE_POS	// 탤런트 히트 캡슐(이펙트 타입이 사용인경우에만 사용하겠습니다.)
};
enum TALENT_EFFECT_BASE_TYPE
{
	TEBY_MELEE_ATTACK,		// 칼
	TEBY_MELEE_SECONDARY_ATTACK,		// 칼 보조
	TEBY_MAGIC_ATTACK,		// 마법
	TEBY_ARCHERY_ATTACK,	// 화살
	TEBY_GUN_ATTACK,		// 총
	TEBY_DEFENSE,			// 방어
	TEBY_NOSHIELD_DEFENSE,	// 방패없는 방어
	TEBY_MONSTER_DEFENSE,	// 몬스터 방어

	TEBY_BASE_MAX
};

enum TALENT_EFFECT_TYPE
{
	CAST_EFFECT = 0,		// 캐스트
	USE_EFFECT,				// 사용
	ATTACK_EFFECT,			// 피격
	MISS_EFFECT,			// 실패
	SHOCK_EFFECT,			// 충격
	DURATION_EFFECT,		// 지속
	DEFENSE_EFFECT,			// 방어
	CAST_END_EFFECT,		// 캐스트 종료

	BLOOD_EFFECT,			// 피격시에 대한 피 효과
	ATTACK_SECONDARY_EFFECT,// 서드 피격

	HIT_EFFECT_POSDIR,		// 피격 이펙트 위치방향

	TALENT_EFFECT_TYPE_MAX
};

enum TALENT_EFFECT_DIRECTION
{
	TED_NONE = 0,			// 방향성 없음
	TED_FRONT_EFFECT_USER,	// 이팩트 사용자 정면
	TED_AUTO_DIR_ATTACK_EFFECT, // 피격 이펙트가 칼 방향에 따라 자동 회전
};

enum TALENT_SHOVE_TYPE
{
	TST_NONE,
	TST_STOP,				// 멈춤
	TST_PASS,				// 관통
	TST_SCRAPE,				// 비비기
	TST_PUSH,				// 밀치기
};