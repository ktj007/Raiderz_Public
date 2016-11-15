#pragma once

struct PmUserData;

class LCmdRouter_Player
{
public:
	static void PostAddPlayerReq(const MUID& uidPlayer, AID nAID, const wstring& strUserID);
	static void PostDeletePlayer(AID nAID, const MUID& uidPlayer);

	static void PostPmangAddPlayerReq(const MUID& uidPlayer, AID nAID, wstring strUserID, const PmUserData* pUserData);

	static void PostPlayerState_SELECT_CHAR(AID nAID, const MUID& uidPlayer);
};
