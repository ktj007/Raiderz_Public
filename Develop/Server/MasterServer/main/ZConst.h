#pragma once

/// 상수값
class ZConst
{
public:
	static int GAME_SERVER_DYNAMIC_FIELD_COUNT_LIMIT;	///< 게임 서버에 생성할 수 있는 동적필드의 최대 개수
	static int GAME_SERVER_PLAYER_COUNT_LIMIT;			///< 게임 서버에 접속할 수 있는 플레이어의 최대 개수
	
	static int FITNESS_ADDITIONAL_VALUE_TO_ALLOC_FIELD;	///< 동적 필드를 할당할 서버를 고를 때 특정 서버에게 주는 가산점
	static float TIME_UPDATE_CHANNEL_BEHOLDER_SEC;		///< ChannelBeholder 의 업데이트 주기 (초)
	static unsigned int TICK_EXPIRE_SILENT_CHANNEL;		///< ChannelBeholder 에서 사용하지 않는 채널을 제거하기까지 대기 시간 (ms)
	static int RATIO_CHANNEL_CROWD_FOR_NEW_CHANNEL;		///< 모든 채널이 몇% 이상 포화상태이면 새로운 채널을 만들지에 대한 비율

	static bool		INCLUDE_DEV_FIELD_LIST;					///< 개발중인 필드도 로딩할 지 여부
	static bool INSERT_WORLD_INFO;						///< 월드 추가 - 내부 테스트용


	/// 디버그용------------------------------------------------------------------

	

	static bool	SHOW_LUA_ERROR_LOG;
	static bool ALLOC_DYNAMIC_SERVER_ANOTHER_SERVER;

	static bool		DEBUG_CONSOLE_SHOW_GAMETICK;
	static int		DEBUG_CONSOLE_GAMETICK_TICK;

	static void Init();
};
