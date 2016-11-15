#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// 원래 로그인 서버에서 담당하지만 StandAlone를 위해 게임서버에서도 처리하는 커맨드 핸들러
class GCmdHandler_FakeLoginServer : public MCommandHandler, public MTestMemPool<GCmdHandler_FakeLoginServer>
{
public:
	GCmdHandler_FakeLoginServer(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestLogin);					///< 로그인
	DECL_CMD_HANDLER(OnRequestAccountCharList);			///< 캐릭터 리스트 요청
	DECL_CMD_HANDLER(OnRequestInsertCharacter);			///< 캐릭터 생성 요청
	DECL_CMD_HANDLER(OnRequestDeleteCharacter);			///< 캐릭터 삭제 요청
	DECL_CMD_HANDLER(OnRequestSelMyChar);				///< 캐릭터 선택

	DECL_CMD_HANDLER(OnRequestMoveLoginServer);			///< 캐릭터 선택 화면으로(로그인서버로) 이동 인증 요청

private:
	static void RouteResponseMoveLoginServer(int nResult, MUID uidPlayer);

};

