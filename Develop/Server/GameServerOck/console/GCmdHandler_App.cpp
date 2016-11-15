#include "stdafx.h"
#include "GCmdHandler_App.h"
#include "MCommandCommunicator.h"
#include "SCommandTable_App.h"
#include "GConsole.h"

GCmdHandler_App::GCmdHandler_App(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MAPP_LOG_OUT_PUT, OnOutPutLog);	
}

MCommandResult GCmdHandler_App::OnOutPutLog(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strMsg;
	int nLogLevel = 0;

	if (pCommand->GetParameter(strMsg,		0, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nLogLevel,	1, MPT_INT)==false) return CR_FALSE;
	
	GConsole::GetInstance().OutputLog(strMsg.c_str(), (MLogLevel)nLogLevel);

	return CR_TRUE;
}