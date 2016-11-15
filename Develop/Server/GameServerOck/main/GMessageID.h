#ifndef _GMESSAGE_ID_H
#define _GMESSAGE_ID_H

#include "GDef.h"

class GLuaResult;
class GEntityPlayer;

/// 메세지 ID
enum GMessageID
{
	GMSG_UNKNOWN = 0,

	// AI에서 사용하는 이벤트
	GMSG_AI_EVT_THINK,				///< 뭔가 주위상황이 바뀌어졌을 때 AI가 뭔가 다음 액션을 취하기를 원할때 발생
	GMSG_AI_EVT_TRY_ATTACKED,		///< 공격을 시도 당했다.
	GMSG_AI_EVT_ATTACK,				///< 공격했다.
	GMSG_AI_EVT_ATTACKED,			///< 공격당했다.
	GMSG_AI_EVT_ARRIVED,			///< 목표로 하는 지점에 도착했다.
	GMSG_AI_EVT_BLOCKED,			///< 더이상 이동할 수 없다.
	GMSG_AI_EVT_DEAD,				///< 죽었다
	GMSG_AI_EVT_FORGET_TARGET,		///< 타겟을 잃어버렸다
	GMSG_AI_EVT_FIND_TARGET,		///< 타겟 발견
	GMSG_AI_EVT_LISTEN,				///< 소리를 들었다
	GMSG_AI_EVT_YELL,				///< 동료의 원조 요청
	GMSG_AI_EVT_HATETABLECHANGED,	///< 헤이트 테이블 값이 바뀌었다.
	GMSG_AI_EVT_STARTCOMBAT,		///< 전투 시작
	GMSG_AI_EVT_ENDCOMBAT,			///< 전투 종료
	GMSG_AI_EVT_FLEE,				///< 도망 칠때
	GMSG_AI_EVT_ENDPROLOGUE,		///< 전투 초기 상태 종료

	GMSG_AI_EVT_START_VICTORY,		///< 승리 상태 시작
	GMSG_AI_EVT_GRABBED,			///< 잡혔을 때
	GMSG_AI_EVT_CHANGETARGET,		///< 타겟이 바뀌었을 때
	GMSG_AI_EVT_ATTACKTURN,			///< 매 공격가능시점에서
	GMSG_AI_EVT_PEACE,				///< Peace 상태로 전이
	GMSG_AI_EVT_SESSION,			///< Session 상태로 전이
	GMSG_AI_EVT_SESSION_CHANGE_SCENE,	///< 세션 씬 변경
	GMSG_AI_EVT_SESSION_PUSHJOB,	///< 세션잡 추가
	GMSG_AI_EVT_SESSION_FINISHED,	///< 세션종료
	GMSG_AI_EVT_JOB_CLEAR,			///< JobMgr 초기화
	GMSG_AI_EVT_JOB_COMPLETED,		///< JobMgr에서 잡이 성공적으로 완료
	GMSG_AI_EVT_JOB_CANCELED,		///< JobMgr에서 잡이 취소
	GMSG_AI_EVT_JOB_TIMEDOUT,		///< JobMgr에서 잡이 시간만료
	GMSG_AI_EVT_JOB_FINISHED,			///< JobMgr에서 잡이 완료 (Finished, Canceld 모두)

	GMSG_AI_EVT_MISSION,			///< 미션 잡이 추가 되었다.

	// 스킬 관련
	GMSGL_HIT_TALENT,			///< 스킬에 맞았다


	GMSG_MAX
};

static const wchar_t* GMessageIDStr[GMSG_MAX] =
{
	L"GMSG_UNKNOWN",
	L"GMSG_AI_EVT_THINK",
	L"GMSG_AI_EVT_TRY_ATTACKED",
	L"GMSG_AI_EVT_ATTACK",
	L"GMSG_AI_EVT_ATTACKED",
	L"GMSG_AI_EVT_ARRIVED",
	L"GMSG_AI_EVT_BLOCKED",
	L"GMSG_AI_EVT_DEAD",
	L"GMSG_AI_EVT_FORGET_TARGET",
	L"GMSG_AI_EVT_FIND_TARGET",
	L"GMSG_AI_EVT_LISTEN",
	L"GMSG_AI_EVT_YELL",
	L"GMSG_AI_EVT_HATETABLECHANGED",
	L"GMSG_AI_EVT_STARTCOMBAT",
	L"GMSG_AI_EVT_ENDCOMBAT",
	L"GMSG_AI_EVT_FLEE",

	L"GMSG_AI_EVT_START_VICTORY",
	L"GMSG_AI_EVT_GRABBED",
	L"GMSG_AI_EVT_CHANGETARGET",
	L"GMSG_AI_EVT_ATTACKTURN",
	L"GMSG_AI_EVT_PEACE",

	L"GMSG_AI_EVT_MISSION",

	L"GMSGL_HIT_TALENT",
};


// 메세지 파라메타

struct EVENT_ATTACKED_INFO
{
	EVENT_ATTACKED_INFO()
	: uidAttacker(MUID::ZERO)
	, nType(DA_NONE)
	, nDamage(0)
	, nEffectSourceType(EST_NONE)
	, nEffectSourceID(INVALID_ID)
	{
	}

	MUID				uidAttacker;
	DAMAGE_ATTRIB		nType;
	uint32				nDamage;

	EFFECT_SOURCE_TYPE	nEffectSourceType;
	int					nEffectSourceID;
};

struct EVENT_ATTACK_INFO
{
	MUID				uidTarget;
	uint32				nDamage;
};

struct EVENT_FOUND_ENEMY
{
	MUID				uidTarget;
};

struct EVENT_DIALOG_EXIT
{
	GEntityPlayer*		pPlayer;
	int					nDialogID;
	int					nExit;
};

struct EVENT_YELL_INFO
{
	MUID				uidSender;
	MUID				uidTarget;
};

struct EVENT_START_COMBAT
{
	MUID				uidEnemy;
};

struct EVENT_FLEE_INFO
{
	FleeType nFleeType;
	float fDurationTime;

	EVENT_FLEE_INFO()
		: fDurationTime(0.0f)
		, nFleeType(Flee_FromEnemy)
	{

	}
};

#endif
