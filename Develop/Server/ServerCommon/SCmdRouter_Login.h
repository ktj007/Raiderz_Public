#pragma once

#include "ServerCommonLib.h"

namespace minet
{
	class MCommand;
}
class SCommandCenter;
struct TD_AccountCharInfo;
struct TD_LOGIN_GAME_SERVER_INFO;

using namespace minet;

/// LoginServer, GameServer 공용 커맨드 라우터
class SCOMMON_API SCmdRouter_Login
{
public:
	SCmdRouter_Login(SCommandCenter* pCommandCenter);

	/// Login Command : LS, GS -> C
	void ResponseLogin(const MUID& uidTargetPlayer, const int nResult, int nServerMode=SERVER_MODE_NORMAL);
	void ResponsePmangLogin(const MUID& uidTargetPlayer, const int nResult, int nServerMode=SERVER_MODE_NORMAL);

	void ResponseAccountCharList(const MUID& uidTargetPlayer, const vector<TD_AccountCharInfo>& vecAccountCharInfo);
	void ResponseInsertChar(const MUID& uidTargetPlayer, const int nResult);
	void ResponseDeleteChar(const MUID& uidTargetPlayer, const int nResult, int nCharIndex);
	void ResponseSelMyChar(const MUID& uidTargetPlayer, const int nResult);

	void MoveToGameServer(const MUID& uidTargetPlayer, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo);
	void DuplicatedPlayerLogin(const MUID& uidTargetPlayer);

	/// Gate Command : LS, GS -> C
	void PreparingToLoad(const MUID& uidTargetPlayer, int nReqFieldID);

private:
	SCommandCenter*	m_pCommandCenter;
};
