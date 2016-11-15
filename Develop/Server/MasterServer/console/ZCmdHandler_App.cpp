#include "stdafx.h"
#include "ZCmdHandler_App.h"
#include "MCommandCommunicator.h"
#include "SCommandTable_App.h"
#include "ZConsole.h"

ZCmdHandler_App::ZCmdHandler_App(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MAPP_LOG_OUT_PUT, OnOutPutLog);
}

MCommandResult ZCmdHandler_App::OnOutPutLog(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strMsg;
	int nLogLevel = 0;

	if (pCommand->GetParameter(strMsg,		0, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nLogLevel,	1, MPT_INT)==false) return CR_FALSE;

	ZConsole::GetInstance().OutputLog(strMsg.c_str(), (MLogLevel)nLogLevel);

	return CR_TRUE;
}