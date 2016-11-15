#include "stdafx.h"
#include "MCommandTable.h"
#include "MCommandManager.h"

namespace minet {

void MAddSharedCommandTable()
{
	BEGIN_CMD_DESC();

	C(MC_LOCAL_VERSION,					MCDT_LOCAL,				MCF_ANY, "Version description");

	C(MC_LOCAL_NET_CONNECT,				MCDT_LOCAL,				MCF_ANY, "Connect to server")
										P(MPT_STR, "IP");
										P(MPT_INT, "port");

	C(MC_LOCAL_NET_CONNECT_WITH_NIC,	MCDT_LOCAL,				MCF_ANY, "Connect to server (specified NetworkCard)")
										P(MPT_STR, "IP");
										P(MPT_INT, "port");
										P(MPT_STR, "NetworkCardIP")

	C(MC_LOCAL_NET_DISCONNECT,			MCDT_LOCAL,				MCF_ANY, "Disconnect to server");

	C(MC_LOCAL_NET_ONCONNECT,			MCDT_LOCAL,				MCF_ANY, "On Connect to server");

	C(MC_LOCAL_NET_ONDISCONNECT,		MCDT_LOCAL,				MCF_ANY, "On Disconnect");
										P(MPT_UID, "uid")

	C(MC_LOCAL_NET_ONDISCONNECT_HARD,	MCDT_LOCAL,				MCF_ANY, "On Hard Disconnect");
										P(MPT_UID, "uid")

	C(MC_LOCAL_NET_CHECKPING,			MCDT_LOCAL,				MCF_ANY, "Check ping time");
										P(MPT_UID, "uid")

	C(MC_LOCAL_NET_ONERROR,				MCDT_LOCAL,				MCF_ANY, "On Error");
										P(MPT_INT, "ErrorCode")

	C(MC_LOCAL_NET_ONACCEPT,			MCDT_LOCAL,				MCF_ANY, "On Login")
										P(MPT_UID, "uidComm");

	C(MC_LOCAL_NET_CLEAR,				MCDT_LOCAL,				MCF_ANY, "Clear Connection");
										P(MPT_UID, "uid")

	C(MC_NET_PING,						MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED,	MCF_ANY, "Ping")
										P(MPT_UINT, "TimeStamp")

	C(MC_NET_PONG,						MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED,	MCF_ANY, "Pong")
										P(MPT_UINT, "TimeStamp")

	C(MC_NET_REPLY_CONNECT,				MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED,	MCF_S2C, "Reply Connection")
										P(MPT_UID,	"uidHost");
										P(MPT_UID,	"uidAlloc");
										P(MPT_UINT,	"TimeStamp");

	C(MC_NET_REALLOC_CLIENT_UID,		MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED,	MCF_S2C, "Realloc Client UID")
										P(MPT_UID,	"uidAlloc");

	C(MC_NET_KEEP_ALIVE,				MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED,	MCF_ANY, "Keep Alive")


	END_CMD_DESC();
}
	
} // namespace minet