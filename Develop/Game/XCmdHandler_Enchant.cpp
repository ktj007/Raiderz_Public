#include "StdAfx.h"
#include "XCmdHandler_Enchant.h"
#include "XMyPlayer.h"
#include "CCommandResultTable.h"
#include "XStrings.h"

XCmdHandler_Enchant::XCmdHandler_Enchant(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_ENCHANT_SUCCESS,			OnEnchantDone);
	SetCmdHandler(MC_ENCHANT_BROKEN,			OnEnchantBroken);
	SetCmdHandler(MC_ENCHANT_CHECK_RESULT,		OnEnchantCheckResult);
}

MCommandResult XCmdHandler_Enchant::OnEnchantDone(MCommand* pCommand, MCommandHandler* pHandler)
{
	bool bEquipSlot = false;
	int nItemSlot = 0;
	int nEnchantItemID = 0;
	int8 nEnchantSlot = 0;

	if (pCommand->GetParameter(&bEquipSlot,	0, MPT_BOOL)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nItemSlot,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nEnchantSlot,	2, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nEnchantItemID,	3, MPT_INT)==false) return CR_ERROR;

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
	pMyPlayer->EnchantDone(bEquipSlot, nItemSlot, nEnchantSlot, nEnchantItemID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Enchant::OnEnchantBroken(MCommand* pCommand, MCommandHandler* pHandler)
{
	bool bEquipSlot = false;
	int nItemSlot = 0;
	int nEnchantBuffID = 0;
	int8 nEnchantSlot = 0;

	if (pCommand->GetParameter(&bEquipSlot,	0, MPT_BOOL)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nItemSlot,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nEnchantSlot,	2, MPT_CHAR)==false) return CR_ERROR;

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
	pMyPlayer->EnchantDone(bEquipSlot, nItemSlot, nEnchantSlot, ENCHANT_BROKEN_SLOT);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Enchant::OnEnchantCheckResult(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nErrCode = 0;

	if (pCommand->GetParameter(&nErrCode,	0, MPT_INT)==false) return CR_ERROR;

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();

	if(nErrCode == CR_SUCCESS)
	{
		// ¼º°ø
		XEvent evt;
		evt.m_nID = XEVTL_ENCHANT_CHECK_SUCCESS;
		pMyPlayer->Event(evt);
	}
	else
	{
		const wchar_t* szMsg = XCmdResultStr(nErrCode);
		global.ui->OnPresentationLower(szMsg, PRESENTATION_ICON_EXCLAMATION, true);


		XEvent evt;
		evt.m_nID = XEVTL_ENCHANT_CANCEL;
		pMyPlayer->Event(evt);
	}

	return CR_TRUE;
}