#include "StdAfx.h"
#include "XCmdHandler_Palette.h"
#include "CSItemHelper.h"

XCmdHandler_Palette::XCmdHandler_Palette(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_PALETTE_SELECT,			OnSelect);
	SetCmdHandler(MC_PALETTE_SET_PRIMARY,		OnSetPrimary);
	SetCmdHandler(MC_PALETTE_SET_SECONDARY,		OnSetSecondary);

	SetCmdHandler(MC_PALETTE_PUTUP,				OnPutUp);
	SetCmdHandler(MC_PALETTE_PUTDOWN,			OnPutDown);
	SetCmdHandler(MC_PALETTE_CHANGE,			OnChange);
	SetCmdHandler(MC_PALETTE_PUTDOWN_ALLTALENT,	OnPutDownAllTalent);
}

MCommandResult XCmdHandler_Palette::OnSelect(MCommand* pCommand, MCommandHandler* pHandler)
{
	PALETTE_NUM nNum;
	if (pCommand->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_ERROR;

	global.ui->PaletteUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Palette::OnSetPrimary(MCommand* pCommand, MCommandHandler* pHandler)
{
	PALETTE_NUM nNum;
	if (pCommand->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_ERROR;

	return CR_TRUE;
}

MCommandResult XCmdHandler_Palette::OnSetSecondary(MCommand* pCommand, MCommandHandler* pHandler)
{
	PALETTE_NUM nNum;
	if (pCommand->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_ERROR;

	return CR_TRUE;
}

MCommandResult XCmdHandler_Palette::OnPutUp(MCommand* pCommand, MCommandHandler* pHandler)
{
	PALETTE_NUM nNum;
	PALETTE_SLOT nSlot;
	PALETTE_ITEM_TYPE nType;
	int nItemIDorTalentID;
	if (pCommand->GetParameter(&nNum,	0, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlot,	1, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nType,	2, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nItemIDorTalentID,	3, MPT_INT)==false) return CR_ERROR;
		
	XPaletteItem item;
	
	if (nType == PIT_ITEM)
	{
		item.itemID = nItemIDorTalentID;
	}
	else if (PIT_TALENT)
	{
		item.talentID = nItemIDorTalentID;
	}

	int nPalleteIndex = CSItemHelper::GetPalleteIndex(nNum, nSlot);
	gvar.MyInfo.PaletteList.SetItemAtIndex(	nPalleteIndex, item);
	global.ui->PaletteUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Palette::OnPutDown(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<PALETTE_NUM> vecNum;
	vector<PALETTE_SLOT> vecSlot;
	if (pCommand->GetBlob(vecNum,	0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecSlot,	1)==false) return CR_ERROR;

	size_t nSize = vecNum.size();
	
	for (size_t i = 0; i < nSize; ++i)
	{
		PALETTE_NUM nNum = vecNum[i];
		PALETTE_SLOT nSlot = vecSlot[i];

		XPaletteItem item;

		int nPalleteIndex = CSItemHelper::GetPalleteIndex(nNum, nSlot);
		gvar.MyInfo.PaletteList.SetItemAtIndex(nPalleteIndex, item);
	}

	global.ui->PaletteUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Palette::OnChange(MCommand* pCommand, MCommandHandler* pHandler)
{
	PALETTE_NUM nNum1;
	PALETTE_SLOT nSlot1;
	PALETTE_NUM nNum2;
	PALETTE_SLOT nSlot2;
	if (pCommand->GetParameter(&nNum1,	0, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlot1,	1, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nNum2,	2, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlot2,	3, MPT_UCHAR)==false) return CR_ERROR;

	int nPalleteIndex1 = CSItemHelper::GetPalleteIndex(nNum1, nSlot1);
	int nPalleteIndex2 = CSItemHelper::GetPalleteIndex(nNum2, nSlot2);

	gvar.MyInfo.PaletteList.ChangeItem(nPalleteIndex1, nPalleteIndex2);
	global.ui->PaletteUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Palette::OnPutDownAllTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.MyInfo.PaletteList.DeleteTalents();
	global.ui->PaletteUIRefresh();

	return CR_TRUE;
}
