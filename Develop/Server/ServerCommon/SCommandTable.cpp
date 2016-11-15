#include "stdafx.h"
#include "SCommandTable.h"
#include "MCommandDesc.h"

using namespace minet;

void AddServerCommonCommandTable()
{
	BEGIN_CMD_DESC();

	C(MSC_PROXY_PLAYER_ADD,					MCDT_MACHINE2MACHINE, MCF_S2C, "Add Proxy Player")
											P(MPT_SINGLE_BLOB, "tdProxyPlayerInfo")								

	C(MSC_PROXY_PLAYER_REMOVE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Remove Proxy Player")
											P(MPT_INT, "nCID")

	C(MSC_PROXY_PLAYER_UPDATE,				MCDT_MACHINE2MACHINE, MCF_S2C, "Update Proxy Player")
											P(MPT_SINGLE_BLOB, "tdProxyPlayerInfo")

	C(MSC_PROXY_ALL_PLAYER_INFO,			MCDT_MACHINE2MACHINE, MCF_S2C, "Refresh Proxy Players")
											P(MPT_BLOB, "tdProxyPlayerInfo")

	C(MSC_ADMIN_IS_SERVABLE,				MCDT_MACHINE2MACHINE, MCF_C2S, "Is Servable")
											P(MPT_BOOL, "IsServable")

	END_CMD_DESC();
}
