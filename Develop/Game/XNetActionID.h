#pragma once

/// NetPlayer 액션 상태
enum NET_ACTION_STATE
{
	NET_ACTION_STATE_NONE			= -1,
	NET_ACTION_STATE_NA				= 0,		///< 미분류


	NET_ACTION_STATE_GRAPPLED,					///< 잡혀짐
	NET_ACTION_STATE_SWALLOWED,					///< 먹혔음
	NET_ACTION_STATE_SIT,						///< 앉아있음
	NET_ACTION_STATE_GUARD,						///< 가드

	NET_ACTION_STATE_GESTURE,

	NET_ACTION_STATE_HIT,						///< 히트

	NET_ACTION_STATE_TALENT,					///< 탤런트

	MAX_NET_ACTION_STATE
};

