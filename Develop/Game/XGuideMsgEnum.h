#pragma once

enum XGuideMsgEnum
{
	GUIDE_MSG_NA					= 0,
	GUIDE_MSG_LEVEL_UP				= 1,	///< 레벨업 했을 때
	GUIDE_MSG_CAN_LOOT				= 2,	///< 루팅 가능할 때
	GUIDE_MSG_CAN_INTERACT_TO_NPC	= 3,	///< NPC 앞에서 인터랙션 가능할 때

	// 다음은 루아에서 호출하는 메세지
	GUIDE_MSG_LUA_RECV_INVITE_PARTY		= 101,	///< 파티초대 팝업 메세지 받았을 때
	GUIDE_MSG_LUA_RECV_INVITE_GUILD		= 102,	///< 길드초대 팝업 메세지 받았을 때
	GUIDE_MSG_LUA_RECV_TRADE_REQUESTED	= 103,	///< 거래요청 팝업 메세지 받았을 때
	GUIDE_MSG_LUA_RECV_DUEL_CHALLENGED	= 104	///< 결투신청 팝업 메세지 받았을 때
};

/*
	XGuideMsgEnum을 추가할 때 game_defines.lua의 GUIDEMSG_TYPE도 함께 추가해야 합니다. - birdkr
*/