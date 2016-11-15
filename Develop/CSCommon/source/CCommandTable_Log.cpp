#include "stdafx.h"
#include "CCommandTable_Log.h"
#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

void AddLogCommandTable()
{
	BEGIN_CMD_DESC();

	C(MLC_COMM_LOGIN_REQ,			MCDT_SERVER2SERVER, MCF_C2S, "Login Req")
									P(MPT_INT, "CommandVersion");
	C(MLC_COMM_LOGIN,				MCDT_SERVER2SERVER, MCF_S2C, "Login")
									P(MPT_INT,	"nResult")
									P(MPT_UID,	"AllocUID")

	C(MLC_CHAR_LOG_REQ,				MCDT_SERVER2SERVER, MCF_C2S, "CharLog Req")
									P(MPT_BLOB, "Date");
									P(MPT_INT,	"CID");
									P(MPT_INT,	"Type");
									P(MPT_WSTR,	"IP");

	C(MLC_ITEM_ACQUIRE_LOG_REQ,		MCDT_SERVER2SERVER, MCF_C2S, "ItemAcquire Log Req")
									P(MPT_BLOB, "Date");
									P(MPT_INT,	"CID");
									P(MPT_INT,	"Type");
									P(MPT_BLOB,	"ItemData");
									P(MPT_INT64,"Money");

	C(MLC_ITEM_LOST_LOG_REQ,		MCDT_SERVER2SERVER, MCF_C2S, "ItemLost Log Req")
									P(MPT_BLOB, "Date");
									P(MPT_INT,	"CID");
									P(MPT_INT,	"Type");
									P(MPT_BLOB,	"ItemData");
									P(MPT_INT64,"Money");

	C(MLC_ITEM_TRADE_LOG_REQ,		MCDT_SERVER2SERVER, MCF_C2S, "ItemTrade Log Req")
									P(MPT_BLOB, "Date");

									P(MPT_INT,	"CID1");
									P(MPT_BLOB,	"ItemData1");
									P(MPT_INT,	"Money1");

									P(MPT_INT,	"CID2");
									P(MPT_BLOB,	"ItemData2");
									P(MPT_INT,	"Money2");

	C(MLC_QUEST_LOG_REQ,			MCDT_SERVER2SERVER, MCF_C2S, "Quest Log Req")
									P(MPT_BLOB, "Date");
									P(MPT_INT,	"CID");
									P(MPT_INT,	"Type");
									P(MPT_INT,	"QID");


	END_CMD_DESC();
}