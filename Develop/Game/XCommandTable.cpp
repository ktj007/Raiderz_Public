#include "stdafx.h"
#include "MCommandTable.h"
#include "MCommandDesc.h"
#include "CCommandTable_WorldLocator.h"

using namespace minet;


void XAddCommandTable()
{
	AddCommonCommandTable();
	AddLoginCommandTable();
	AddWorldLocatorCommandTable();


	BEGIN_CMD_DESC();

	C(MC_LOCAL_RELOAD_UI,				MCDT_LOCAL, MCF_ANY, "Reload UI");

	C(MC_LOCAL_CHANGE_GAMEFRAME,		MCDT_LOCAL, MCF_ANY, "Change GameFrame");
										P(MPT_INT,		"GameFrameName")

	C(MC_LOCAL_TEST_CHANGE_ENV,			MCDT_LOCAL, MCF_ANY, "Change Environment");
										P(MPT_CHAR,		"nTimeType")
										P(MPT_CHAR,		"nWeatherType")

	C(MC_LOCAL_TEST_SELF_MOTION_FACTOR,	MCDT_LOCAL, MCF_ANY, "Test Motion Factor");
										P(MPT_SINGLE_BLOB,		"talent_hit")
										P(MPT_SINGLE_BLOB,		"KnockbackInfo")

	END_CMD_DESC();
}