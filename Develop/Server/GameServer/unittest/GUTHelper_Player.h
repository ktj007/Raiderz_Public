#pragma once

#include "GDef.h"

class GUTHelper_Player
{
public:
	GUTHelper_Player(void);
	~GUTHelper_Player(void);

	GEntityPlayer* NewPlayer(GField* pField=NULL, vec3 pos=vec3(0,0,0), MUID uidNew=MUID(0,0), MARKER_INFO markerExit=MARKER_INFO());

	void SetMockPlayerPVPArea_Locate(GEntityPlayer* pPlayer);
	void SetMockPlayerPVPArea_LocateAndTeamOne(GEntityPlayer* pPlayer);
	void SetMockPlayerPVPArea_LocateAndTeamTwo(GEntityPlayer* pPlayer);

	void ResetMockPlayerPVPArea(GEntityPlayer* pPlayer);
};
