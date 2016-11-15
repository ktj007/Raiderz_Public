#include "StdAfx.h"
#include "GCmdHandler_Palette.h"
#include "CCommandTable.h"
#include "GPaletteSystem.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"

GCmdHandler_Palette::GCmdHandler_Palette(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_PALETTE_SELECT_REQ,		OnRequestSelect);
	SetCmdHandler(MC_PALETTE_SET_PRIMARY_REQ,	OnRequestSetPrimary);
	SetCmdHandler(MC_PALETTE_SET_SECONDARY_REQ,	OnRequestSetSecondary);
	SetCmdHandler(MC_PALETTE_PUTUP_REQ,			OnRequestPutUp);
	SetCmdHandler(MC_PALETTE_PUTDOWN_REQ,		OnRequestPutDown);
	SetCmdHandler(MC_PALETTE_CHANGE_REQ,		OnRequestChange);
}

MCommandResult GCmdHandler_Palette::OnRequestSelect(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	PALETTE_NUM nNum;

	if (pCmd->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_FALSE;

	gsys.pPaletteSystem->Select(pPlayer, nNum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Palette::OnRequestSetPrimary(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	PALETTE_NUM nNum;
	if (pCmd->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_FALSE;

	gsys.pPaletteSystem->SetPrimary(pPlayer, nNum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Palette::OnRequestSetSecondary(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	PALETTE_NUM nNum;
	if (pCmd->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_FALSE;

	gsys.pPaletteSystem->SetSecondary(pPlayer, nNum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Palette::OnRequestPutUp(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	PALETTE_NUM nNum;
	PALETTE_SLOT nSlot;
	PALETTE_ITEM_TYPE nType;
	int nItemIDorTalentID;

	if (pCmd->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSlot,	1, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nType,	2, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nItemIDorTalentID,	3, MPT_INT)==false) return CR_FALSE;

	gsys.pPaletteSystem->PutUp(pPlayer, nNum, nSlot, nType, nItemIDorTalentID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Palette::OnRequestPutDown(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	PALETTE_NUM nNum;
	PALETTE_SLOT nSlot;
	if (pCmd->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSlot,	1, MPT_UCHAR)==false) return CR_FALSE;

	gsys.pPaletteSystem->PutDown(pPlayer, nNum, nSlot);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Palette::OnRequestChange(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	PALETTE_NUM nNum1;
	PALETTE_SLOT nSlot1;
	PALETTE_NUM nNum2;
	PALETTE_SLOT nSlot2;
	if (pCmd->GetParameter(&nNum1,	0, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSlot1,	1, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nNum2,	2, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSlot2,	3, MPT_UCHAR)==false) return CR_FALSE;

	gsys.pPaletteSystem->Change(pPlayer, nNum1, nSlot1, nNum2, nSlot2);

	return CR_TRUE;
}