#pragma once

#include "ServerCommonLib.h"

// ServerCommon : 18001 ~ 17000 (SCommandTable)
enum SCommandTable
{	
	MSC_PROXY_PLAYER_ADD		= 18001,		///< 플레이어 정보 추가
	MSC_PROXY_PLAYER_REMOVE		= 18002,		///< 플레이어 정보 제거
	MSC_PROXY_PLAYER_UPDATE		= 18003,		///< 플레이어 정보 갱신
	MSC_PROXY_ALL_PLAYER_INFO	= 18004,		///< 모든 플레이어 정보

	MSC_ADMIN_IS_SERVABLE		= 18101,		///< 서비스 가능한 상태인지 여부를 마스터 서버에 알림
};

SCOMMON_API void AddServerCommonCommandTable();
