#include "stdafx.h"
#include "CCommandTable_WorldLocator.h"
#include "MCommandDesc.h"

using namespace minet;

void AddWorldLocatorCommandTable()
{
	BEGIN_CMD_DESC();

	C(MWC_COMM_WORLD_INFO_REQ,		MCDT_MACHINE2MACHINE, MCF_C2S, "World Info Req")
									
	C(MWC_COMM_WORLD_INFO_RES,		MCDT_MACHINE2MACHINE, MCF_S2C, "World Info Res")
									P(MPT_BLOB,	"WorldInfo")

	END_CMD_DESC();
}
