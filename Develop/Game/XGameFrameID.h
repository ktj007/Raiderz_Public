#pragma once

// 게임 프레임
enum XGameFrameID
{
	NONE_FRAME_ID			= -1,
	LOGIN_FRAME_ID			= 0,			///< 로그인 프레임
	CHAR_FRAME_ID,							///< 캐릭터 선택 프레임
	PLAY_FRAME_ID,							///< 게임 프레임
	LOADING_FRAME_ID,

	REPLAY_FRAME_ID,
	DUMMYCLIENT_FRAME_ID,					///< 더미 클라이언트 프레임		- DummyClient 프로젝트에서 사용
	MINT_DESIGN_FRAME_ID,					///< UI 테스트 프레임
	ENGINE_DEBUG_FRAME_ID,					///< 외부 엔진 디버깅 프레임
	TEST_FRAME_ID,
	HELLOWORLD_FRAME_ID,					///< 새 프레임을 만들 때에는 이것을 가지고 복사해서 쓰세요. - birdkr
};


// 특별한 인자가 없을 경우 제일 먼저 시작하는 게임 프레임
const XGameFrameID START_FRAME_ID = LOGIN_FRAME_ID;

