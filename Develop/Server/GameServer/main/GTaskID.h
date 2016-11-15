#ifndef _GTASK_ID_H
#define _GTASK_ID_H

/// Task ID
enum GTaskID
{
	GTASK_NA		= 0,
	GTASK_IDLE,					///< 대기
	GTASK_MOVE,					///< 지정 위치로 이동
	GTASK_COMBAT_MOVE,			///< 점진적 이동
	GTASK_TALENT,				///< 스킬 사용
	GTASK_GUARD,				///< 방어
	GTASK_LUA_CALL,				///< 루아함수 호출
	GTASK_SPEAK,				///< 대화함수 호출
	GTASK_ADJUST,				///< 여러 동적 수치들 변경 (guard_rate등)
	GTASK_AGGRO,				///< 어그로 테이블 조작
	GTASK_BREAKPART,			///< 브레이커블 파츠 파괴
	GTASK_MOTIONFACTOR,			///< Motionfactor
	GTASK_CHANGE_CAPSULEGROUP,	///< 충돌판정 그룹 변경
	GTASK_DAMAGE,				///< 피해입음
	GTASK_FLEE,					///< 도망치기
	GTASK_YELL,					///< 도움요청하기
	GTASK_WAITFOR,				///< 특정 액터가 NextAction할때까지 대기하기

	GTASK_TEST,
};

enum GTaskEvent
{
	TE_NA				 = 0,
	TE_PATH_MOVE_BLOCKED,
	TE_PATH_FIND_FAILED,
};

/// task 결과
enum GTaskResult
{
	TR_NONE,
	TR_RUNNING,
	TR_COMPLETED,
	TR_CANCELED
};

#endif	