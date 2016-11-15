#include "stdafx.h"
#include "MCommandTable.h"
#include "GCommandTable.h"
#include "MCommandDesc.h"
#include "MCommand.h"
#include "SCommandTable.h"

using namespace minet;

void GAddCommandTable()
{
	AddCommonCommandTable();
	AddServerCommonCommandTable();
	AddLoginCommandTable();
	AddMaster2GameCommandTable();
	AddApp2GameCommandTable();

	//BEGIN_CMD_DESC();

	//C(MC_LOCAL_GAME_START,			MCDT_LOCAL, "Player Game Start");
	//								P(MPT_UID, "uidPlayer");
	//								P(MPT_UID, "uidField");

	//C(MC_LOCAL_ENTER_TO_FIELD,		MCDT_LOCAL, "Enter To Field");
	//								P(MPT_UID, "uidPlayer");
	//								P(MPT_UID, "uidField");

	//C(MC_LOCAL_PLAYER_LOGOUT,		MCDT_LOCAL, "Player Logout");
	//								P(MPT_UID, "uidPlayer");

	//END_CMD_DESC();
}