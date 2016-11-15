#include "stdafx.h"
#include "SCommandTable_App.h "
#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

void AddAppCommandTable()
{
	BEGIN_CMD_DESC();

	C(MAPP_LOG_OUT_PUT,	MCDT_LOCAL, MCF_ANY, "Out Put Log String")
		P(MPT_WSTR, "LogString");
		P(MPT_INT, "LogLevel");

	END_CMD_DESC();
}