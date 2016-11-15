#pragma once

/// NPC 액션 상태
enum NPC_ACTION_STATE
{
	NPC_ACTION_STATE_NONE			= -1,
	NPC_ACTION_STATE_NA				= 0,		///< 미분류
	NPC_ACTION_STATE_MOVE			= 1,		///< 이동
	NPC_ACTION_STATE_TALENT			= 2,		///< 탤런트 사용
	NPC_ACTION_STATE_HIT			= 3,		///< 피격
	NPC_ACTION_STATE_DIE			= 4,		///< 죽음

	MAX_NPC_ACTION_STATE
};
