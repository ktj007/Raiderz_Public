#pragma once

#include "ZServerAcceptor.h"

class ZLoginServerInfo;

/// 게임 서버의 로그인 요청을 들어주는 클래스
class ZLoginServerAcceptor : public ZServerAcceptor
{
public:
	ZLoginServerAcceptor(MUID uidClient, int nServerID) : ZServerAcceptor(SERVER_LOGIN, uidClient, nServerID) {}
	virtual ~ZLoginServerAcceptor() {}

	bool IsAlreadyExist() override;
	bool Accept() override;
	minet::MCommand* MakeResponseCommand() override;
	minet::MCommand* MakeFailCommand(int nCommandResult) override;
};
