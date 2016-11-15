#pragma once

struct PmUserData;
class ZPlayer;

class ZDuplicationLoginLogic
{
public:
	ZDuplicationLoginLogic(void);
	~ZDuplicationLoginLogic(void);
	
	bool IsDuplicated(AID nAID);
	void StartKickInWorldPlayer(MUID uidPlayer, AID nAID, const wstring& strUserID, PmUserData* pPmUserData);

	bool ExistWaitingPlayer(AID nAID);
	void FlushDuplicatedLogin(AID nAID, const MUID& uidLeavePlayer);

private:
	void KickWaitingDupLoginPlayer(AID nAID);
	void RouteKickDuplicatedPlayer(ZPlayer* pKickPlayer);

};
