#include "stdafx.h"
#include "LCmdHandler_App.h"
#include "MCommandCommunicator.h"
#include "SCommandTable_App.h"
#include "LApplication.h"

LCmdHandler_App::LCmdHandler_App(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MAPP_LOG_OUT_PUT, OnOutPutLog);
}

MCommandResult LCmdHandler_App::OnOutPutLog(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strMsg;
	int nLogLevel = 0;

	if (pCommand->GetParameter(strMsg,		0, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nLogLevel,	1, MPT_INT)==false) return CR_FALSE;

	LApplication::GetInstance().OutputLog(strMsg.c_str(), (MLogLevel)nLogLevel);

	return CR_TRUE;
}