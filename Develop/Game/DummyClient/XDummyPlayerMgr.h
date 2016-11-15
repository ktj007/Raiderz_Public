#pragma once


enum QUESTPVP_TEAM;

class XPlayerForDummy;
typedef map<UIID, XPlayerForDummy*> MAP_PLAYERFORDUMMY;

class XPlayerForDummy
{
public:
	XPlayerForDummy(UIID nUIID, vec3 vPos, bool bDead=false, QUESTPVP_TEAM nQPVPTeam=static_cast<QUESTPVP_TEAM>(0));
	
	UIID nUIID;
	vec3 vPos;
	bool bDead;
	QUESTPVP_TEAM nQPVPTeam;	
};

class XDummyPlayerMgr
{
public:
	XDummyPlayerMgr(void);
	~XDummyPlayerMgr(void);

	minet::MCommandResult OnCommand(MCommand* pCommand);

	void Insert(UIID nUIID, vec3 vPos, bool bDead, QUESTPVP_TEAM nQPVPTeam = static_cast<QUESTPVP_TEAM>(0));
	void Delete(UIID nUIID);

	XPlayerForDummy* Get(UIID nUIID);	

	XPlayerForDummy* GetEnemy(vec3 vCenter, QUESTPVP_TEAM nQPVPTeam=static_cast<QUESTPVP_TEAM>(0));

private:
	MAP_PLAYERFORDUMMY m_mapPlayerForDummy;
};
