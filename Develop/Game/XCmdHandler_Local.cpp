#include "stdafx.h"
#include "XCmdHandler_Local.h"
#include "XGame.h"
#include "XGlueGameEvent.h"
#include "XBaseApplication.h"

XCmdHandler_Local::XCmdHandler_Local(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_RELOAD_UI,			OnReloadUI);
	SetCmdHandler(MC_LOCAL_CHANGE_GAMEFRAME,	OnChangeGameFrame);
}

MCommandResult XCmdHandler_Local::OnReloadUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.ui->Reload();
	gvar.MyInfo.EquipmentSlot.RefreshUI();
	global.ui->InventoryUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Local::OnChangeGameFrame(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nGameFrameName;
	if (pCommand->GetParameter(&nGameFrameName,	0, MPT_INT)==false) return CR_ERROR;
	
	global.app->ChangeFrame(nGameFrameName);

	return CR_TRUE;
}

