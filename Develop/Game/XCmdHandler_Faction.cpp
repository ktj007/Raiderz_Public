#include "StdAfx.h"
#include "XCmdHandler_Faction.h"
#include "CSFactionInfo.h"
#include "XMyPlayer.h"
#include "XPlayerFaction.h"

XCmdHandler_Faction::XCmdHandler_Faction(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_FACTION_INCREASE,	OnIncrease);
	SetCmdHandler(MC_FACTION_DECREASE,	OnDecrease);	
}

MCommandResult XCmdHandler_Faction::OnIncrease(MCommand* pCommand, MCommandHandler* pHandler)
{
	uint8 nFactionID;
	uint16 nFactionQuantity;

	if (pCommand->GetParameter(&nFactionID,				0,	MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFactionQuantity,		1,	MPT_USHORT)==false) return CR_ERROR;

	gvar.MyInfo.FactionInfo.SetQuantity(nFactionID, nFactionQuantity);

	if (global.ui)	global.ui->RefreshFactionUI();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Faction::OnDecrease(MCommand* pCommand, MCommandHandler* pHandler)
{
	uint8 nFactionID;
	uint16 nFactionQuantity;

	if (pCommand->GetParameter(&nFactionID,			0,	MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFactionQuantity,	1,	MPT_USHORT)==false) return CR_ERROR;

	gvar.MyInfo.FactionInfo.SetQuantity(nFactionID, nFactionQuantity);

	if (global.ui)	global.ui->RefreshFactionUI();

	return CR_TRUE;
}