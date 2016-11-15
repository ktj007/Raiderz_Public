#pragma once

struct TD_LOGIN_GAME_SERVER_INFO;
struct TD_LOGIN_ACCOUNT_INFO;
struct TD_LOGIN_FIELD_INFO;

namespace minet
{
class MCommand;
}

class ZMoveServerLogic
{
public:
	bool RequestLoginGameServer(const MUID& uidRequester, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo);
	bool ResponseLoginGameServer(const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo);

	bool RequestReserveMoveGameServer(const MUID& uidFromGameServer, const MUID& uidRequester, int nToGameServerID, minet::MCommand* pCommand);
	bool ResponseReadyEnterMovePlayer(const MUID& uidFromGameServer, const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo);
	bool CancelMoveGameServer(MUID uidFromGameServer, MUID uidRequester);

	bool ReserveMoveLoginServerReq(const MUID& uidPlayer, const MUID& uidGameServer);
	bool ReadyEnterLoginServerRes(const MUID& uidFromGameServer, const MUID& uidPlayer, int nResult, const MUID& uidConnectionKey);
	bool CancelMoveLoginServer(const MUID& uidPlayer, const MUID& uidGameServerToStay);

private:
	bool RequestLoginGameServer_Check(const MUID& uidRequester, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo);
	void RequestLoginGameServer_Apply(const MUID& uidRequester, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo);

	void ResponseLoginGameServer_ResultFAIL(const MUID& uidRequester, int nResult);
	bool ResponseLoginGameServer_Check(const MUID& uidRequester, const MUID& uidToGameServer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo);
	void ResponseLoginGameServer_Apply(const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo);



	bool RequestReserveMoveGameServer_Check(const MUID& uidFromGameServer, const MUID& uidRequester, int nToGameServerID, minet::MCommand* pCommand);
	void RequestReserveMoveGameServer_Apply(const MUID& uidFromGameServer, const MUID& uidRequester, int nToGameServerID, minet::MCommand* pCommand);

	void ResponseReadyEnterMovePlayer_ResultFAIL(const MUID& uidFromGameServer, const MUID& uidRequester, int nResult, const MUID& uidToGameServer);
	bool ResponseReadyEnterMovePlayer_Check(const MUID& uidFromGameServer, const MUID& uidRequester, const MUID& uidToGameServer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo);
	void ResponseReadyEnterMovePlayer_Apply(const MUID& uidFromGameServer, const MUID& uidRequester, int nResult, const MUID& uidToGameServer, const TD_LOGIN_GAME_SERVER_INFO* pToGameServerInfo);

	bool CancelMoveGameServer_Check(const MUID& uidFromGameServer, const MUID& uidRequester);
	void CancelMoveGameServer_Apply(const MUID& uidFromGameServer, const MUID& uidRequester);



	bool ReserveMoveLoginServerReq_Check(const MUID& uidPlayer);
	void ReserveMoveLoginServerReq_Apply(const MUID& uidPlayer, const MUID& uidGameServer);

	bool ReadyEnterLoginServerRes_Check(const MUID& uidFromGameServer, const MUID& uidPlayer);
	void ReadyEnterLoginServerRes_Apply(const MUID& uidFromGameServer, const MUID& uidPlayer, int nResult, const MUID& uidConnectionKey);

	bool CancelMoveLoginServer_Check(const MUID& uidPlayer, const MUID& uidGameServer);
	void CancelMoveLoginServer_Apply(const MUID& uidPlayer, const MUID& uidGameServer);

};
