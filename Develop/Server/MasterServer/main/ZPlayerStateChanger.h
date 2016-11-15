#pragma once

class ZPlayer;

class ZPlayerStateChanger
{
public:
	ZPlayerStateChanger();
	~ZPlayerStateChanger();

	bool ChangeToSELECT_CHAR(AID nAID);
	bool ChangeToREADY_LOGIN_GAMESERVER(AID nAID);
	bool ChangeToIN_WORLD(AID nAID);
	bool ChangeToREADY_MOVE_GAMESERVER(MUID uidPlayer);
	bool ChangeToMOVE_GAMESERVER(MUID uidPlayer);
	bool ChangeToREADY_MOVE_LOGINSERVER(const MUID& uidPlayer);
	bool ChangeToMOVE_LOGINSERVER(const MUID& uidPlayer);

	bool Cancel_READY_LOGIN_GAMESERVER(AID nAID);
	bool Cancel_READY_MOVE_GAMESERVER(MUID uidPlayer);
	bool Cancel_READY_MOVE_LOGINSERVER(MUID uidPlayer);

private:
	bool ChangeToIN_WORLD_Check(AID nAID);

	bool ChangeToREADY_MOVE_LOGINSERVER_Check(const MUID& uidPlayer);

	bool ChangeToSELECT_CHAR_Check(AID nAID);

};
