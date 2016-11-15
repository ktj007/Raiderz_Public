#ifndef _XMYACTION_ID_H
#define _XMYACTION_ID_H

/// 내 액션 상태
enum ACTION_STATE
{
	ACTION_STATE_NONE				= -1,
	ACTION_STATE_IDLE				= 0,		///< IDLE
	ACTION_STATE_JUMP,							///< 점프
	ACTION_STATE_FALLING,						///< 떨어지는 중
	ACTION_STATE_CHANGE_STANCE,					///< 자세 변환
	ACTION_STATE_HIT,							///< 피격
	ACTION_STATE_GUARD,							///< 방어
	ACTION_STATE_NORMAL_ATTACK,					///< 평타
	ACTION_STATE_USE_TALENT,					///< 텔런트 사용
	ACTION_STATE_SHOOT_ARROW,					///< 활로 기본 공격

	ACTION_STATE_DIE,							///< 죽음
	ACTION_STATE_USE_ITEM,						///< 아이템 사용
	ACTION_STATE_WEAPON_CHANGE,					///< 무기 변경
	ACTION_STATE_LOOTINGITEM,					///< 시체루팅, 인터랙션아이템
	ACTION_STATE_HOLD,							///< 아무 것도 못움직이는 상태 - 주로 UI 인터랙션용으로 사용

	ACTION_STATE_REBIRTH,						///< 부활

	ACTION_STATE_GRAPPLED,						///< 잡기

	ACTION_STATE_MAPOBJ_INTERACTION,			///< 맵오브젝트와 인터랙션
	ACTION_STATE_SIT,							///< 앉기

	ACTION_STATE_CHANGE_CHANNEL,				///< 채널 이동
	ACTION_STATE_QUEST_INTERACTION,				///< 퀘스트 인터랙션
	ACTION_STATE_DEACTIVATIVE,					///< 활동 불능 상태

	ACTION_STATE_SWALLOWED,						///< 먹기

	ACTION_STATE_GESTURE,						///< 동작들

	ACTION_STATE_INTERACTION,					///< NPC와 인터렉션

	ACTION_STATE_ENCHANT,						///< 인첸트

	ACTION_STATE_DYE,							///< 염색
	//SoulHunterZ
	ACTION_STATE_ATTUNE,

	MAX_ACTION_STATE
};









#endif // _XMYACTION_IDLE_H