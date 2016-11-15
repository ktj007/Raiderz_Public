#ifndef _GJOB_ID_H
#define _GJOB_ID_H

/// Job ID
enum GJobID
{
	GJOB_NA				= 0,
	GJOB_IDLE,					///< 대기
	GJOB_MOVE_TO_POS,			///< 지정 위치로 이동
	GJOB_MOVE_TO_ENTITY,		///< 지정 엔티티로 이동
	GJOB_ESCAPE_FROM_AREA,		///< 일정 지역에서 벗어남

	GJOB_PATROL,				///< 순찰
	GJOB_ROAM,					///< 배회
	GJOB_TALENT,				///< 스킬 사용
	GJOB_GUARD,

	GJOB_TEST,

	GJOB_FLEE_FROM_ENTITY,		///< 지정 엔티티로부터 도망
	GJOB_FLEE_TO_HOME,			///< 전투 시작지역으로 도망
//	GJOB_ROUND_TO_POS,			///< 지정된 위치들의 코스로 이동
	GJOB_SCRIPT,				///< 스크립트
	GJOB_SPEAK,					///< 대화
	GJOB_ADJUST,				///< 여러 동적 수치들 변경 (guard_rate등)
	GJOB_AGGRO,				///< 어그로 테이블 조작
	GJOB_BREAKPART,			///< 브레이커블 파츠 파괴
	GJOB_MOTIONFACTOR,			///< Motionfactor
	GJOB_CHANGE_CAPSULEGROUP,	///< 충돌판정 그룹 변경
	GJOB_DAMAGE,				///< 피해입음
	GJOB_FLEE,					///< 도망치기
	GJOB_YELL,					///< 도움요청하기

	GJOB_WARP,					///< 특정 마커 위치로 액터 이동		
	GJOB_INSTANTBUFF,			///< 버프 효과를 가지게 만든다.
	GJOB_DESPAWN,				///< 디스폰
	GJOB_DIE,					///< 죽도록 만든다.
	GJOB_ASSIST,				///< 대상의 적을 공격한다.
	GJOB_ATTACK,				///< 대상을 공격한다.	
	GJOB_FACETO,				///< 대상을 바라본다.
	GJOB_WAITFOR,				///< 대상 액터가 NextAction할때까지 대기.
	GJOB_NEXTACTION,			///< 자신을 대기하는 액터들이 다음 행동을 할 수 있게 처리
	GJOB_SUMMON,				///< 부하 소환

	GJOB_SESSION_BLOCKING,		///< 세션 블러킹모드 변경
	GJOB_SESSION_WAIT,			///< 세션 대기
	GJOB_SESSION_ADD_NPC,		///< 세션에 NPC 추가
	GJOB_SESSION_REMOVE_NPC,	///< 세션에서 NPC 제거
	GJOB_SESSION_END,			///< 세션 종료
	GJOB_SESSION_CHANGE_STATE,	///< 세션 상태 변경
	GJOB_SESSION_MAKE_PEACE,	///< 세션에 관련된 NPC 모두 전투종료
	GJOB_SESSION_MAKE_COMBAT,	///< 세션에 관련된 NPC 모두 전투시작

	GJOB_MAX
};

/// task 결과
enum GJobResult
{
	JR_NONE,
	JR_RUNNING,
	JR_COMPLETED,
	JR_CANCELED,
	JR_TIMEOUT,
};


#endif