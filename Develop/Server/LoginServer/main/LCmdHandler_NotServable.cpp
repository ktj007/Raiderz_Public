#include "StdAfx.h"
#include "LCmdHandler_NotServable.h"
#include "LCommandTable.h"
#include "SCmdRouter_Login.h"
#include "LGlobal.h"
#include "LCommandCenter.h"


LCmdHandler_NotServable::LCmdHandler_NotServable(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_REQUEST_LOGIN,				OnLogin);
	SetCmdHandler(MC_COMM_REQUEST_LOGIN_ON_PMANG,		OnPmangLogin);

}

MCommandResult LCmdHandler_NotServable::OnLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCommandVersion;
	if (!pCommand->GetParameter(&nCommandVersion,	2,	MPT_INT))	return CR_ERROR;

	MUID uidPlayer = pCommand->GetSenderUID();

	if (!_Login(uidPlayer, nCommandVersion))
		return CR_ERROR;


	return CR_TRUE;
}

MCommandResult LCmdHandler_NotServable::OnPmangLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nCommandVersion;
	if (!pCommand->GetParameter(&nCommandVersion,	0,	MPT_INT))	return CR_ERROR;

	MUID uidPlayer = pCommand->GetSenderUID();

	if (!_Login(uidPlayer, nCommandVersion))
		return CR_ERROR;

	return CR_TRUE;
}

bool LCmdHandler_NotServable::_Login(MUID uidPlayer, int nCommandVersion)
{
	// 요청자 확인
	if (uidPlayer.IsInvalid()) return false;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		cmdRouter.ResponsePmangLogin(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);		
		return true;
	}

	// 즉시 응답 커맨드 전송
	cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN_SERVERS_ARE_STARTING);
	return true;
}