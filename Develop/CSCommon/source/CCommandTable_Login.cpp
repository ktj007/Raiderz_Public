
#include "stdafx.h"
#include "CCommandTable_Login.h"
#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

void AddLoginCommandTable()
{
	BEGIN_CMD_DESC();

	C(MC_COMM_REQUEST_LOGIN,						MCDT_MACHINE2MACHINE, MCF_C2S, "Request Login")
													P(MPT_WSTR, "UserID");
													P(MPT_WSTR, "Password");
													P(MPT_INT, "CommandVersion");

	C(MC_COMM_RESPONSE_LOGIN,						MCDT_MACHINE2MACHINE, MCF_S2C, "Response Login")
													P(MPT_INT,	"nResult")
													P(MPT_UID,	"AllocUID")
													P(MPT_UCHAR,"ServerMode")

	C(MC_COMM_REQUEST_LOGIN_ON_PMANG,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Login On Pmang")
													P(MPT_INT,			"CommandVersion");											
													P(MPT_WSTR,			"PmangLoginStream");

	C(MC_COMM_RESPONSE_LOGIN_ON_PMANG,				MCDT_MACHINE2MACHINE, MCF_S2C, "Response Login On Pmang")
													P(MPT_INT,	"nResult")
													P(MPT_UID,	"AllocUID")
													P(MPT_UCHAR,"ServerMode")

	C(MC_COMM_REQUEST_LOGIN_GAME_SERVER,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Login GameServer")
													P(MPT_UID, "uidConnectionKey")
													P(MPT_INT, "CommandVersion")

	C(MC_COMM_RESPONSE_LOGIN_GAME_SERVER,			MCDT_MACHINE2MACHINE, MCF_S2C, "Response Login GameServer")
													P(MPT_INT, "nResult")
													P(MPT_UID,	"AllocUID")
													P(MPT_USHORT,	"UIID")

	C(MC_COMM_ENTER_WORLD_MSG,						MCDT_MACHINE2MACHINE, MCF_S2C, "Enter World Msg")
													P(MPT_INT, "nResult")

	C(MC_COMM_ENTER_WORLD_LOCAL_MSG,				MCDT_LOCAL, MCF_ANY, "Enter World Msg (Local)")
													P(MPT_INT, "nResult")

	C(MC_COMM_MOVE_GAME_SERVER,						MCDT_MACHINE2MACHINE, MCF_S2C, "Move Game Server")
													P(MPT_BLOB, "LoginGameServerInfo")

	C(MC_COMM_REQUEST_MOVE_GAME_SERVER,				MCDT_MACHINE2MACHINE, MCF_C2S, "Request Move Game Server")
													P(MPT_UID, "ConnectionKey")
													P(MPT_INT, "CommandVersion")

	C(MC_COMM_RESPONSE_MOVE_GAME_SERVER,			MCDT_MACHINE2MACHINE, MCF_S2C, "Response Move Game Server")
													P(MPT_INT, "nResult")
													P(MPT_UID,	"AllocUID")
													P(MPT_USHORT,	"UIID")

	C(MC_COMM_REQUEST_MOVE_LOGIN_SERVER,			MCDT_MACHINE2MACHINE, MCF_C2S, "Request Move Login Server")
													P(MPT_UID, "ConnectionKey")
													P(MPT_INT, "CommandVersion")

	C(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER,			MCDT_MACHINE2MACHINE, MCF_S2C, "Response Move Login Server")
													P(MPT_INT, "nResult")
													P(MPT_UID,	"AllocUID")

	C(MC_COMM_REQUEST_LOGIN_GAME_SERVER_ON_PMANG,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Login GameServer On Pmang")
													P(MPT_UID, "uidConnectionKey")
													P(MPT_INT, "CommandVersion")
													P(MPT_WSTR, "PmangStatIndex")
													P(MPT_INT, "PmangPCCafeID")

	C(MC_COMM_REQUEST_MOVE_GAME_SERVER_ON_PMANG,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Move Game Server On Pmang")
													P(MPT_UID, "ConnectionKey")
													P(MPT_INT, "CommandVersion")
													P(MPT_WSTR, "PmangStatIndex")
													P(MPT_INT, "PmangPCCafeID")

	C(MC_COMM_REQUEST_MOVE_LOGIN_SERVER_ON_PMANG,	MCDT_MACHINE2MACHINE, MCF_C2S, "Request Move Login Server On Pmang")
													P(MPT_UID, "ConnectionKey")
													P(MPT_INT, "CommandVersion")
													P(MPT_WSTR, "PmangStatIndex")
													P(MPT_INT, "PmangPCCafeID")

	END_CMD_DESC();
}
