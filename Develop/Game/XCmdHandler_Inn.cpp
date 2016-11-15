#include "StdAfx.h"
#include "XCmdHandler_Inn.h"
#include "XMyPlayer.h"
#include "XInteractionInfo.h"

XCmdHandler_Inn::XCmdHandler_Inn(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_INN_BEGIN_SLEEP,	OnBeginSleep);
	SetCmdHandler(MC_INN_END_SLEEP,		OnEndSleep);
}

MCommandResult XCmdHandler_Inn::OnBeginSleep(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionMapObjInteraction(XGetInteractionInfo().GetInteractionSearchUID());

	gvar.Game.pMyPlayer->GetModuleMyControl()->DoInnBeginSleep();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Inn::OnEndSleep(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->DoInnEndSleep();

	return CR_TRUE;
}
