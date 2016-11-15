#include "StdAfx.h"
#include "XCmdHandler_Storage.h"
#include "CCommandTable.h"
#include "XItemManager.h"

XCmdHandler_Storage::XCmdHandler_Storage(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{	
	SetCmdHandler(MC_STORAGE_SHOW_UI,			OnShowUI);
	SetCmdHandler(MC_STORAGE_DEPOSIT_MONEY,		OnDepositMoney);
	SetCmdHandler(MC_STORAGE_WITHDRAW_MONEY,	OnWithdrawMoney);
}

MCommandResult XCmdHandler_Storage::OnShowUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nStorageMoney;
	vector<TD_ITEM>	vecTDItem;
	bool bSerialized;
	if (pCommand->GetParameter(&nStorageMoney, 0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDItem, 1)==false) return CR_ERROR;
	if (pCommand->GetParameter(&bSerialized, 2, MPT_BOOL)==false) return CR_ERROR;

	if (!bSerialized)
	{
		gvar.MyInfo.ChrInfo.SetStorageMoney(nStorageMoney);

		for each (const TD_ITEM& tdItem in vecTDItem)
		{
			XItem* pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);
			if (pNewItem == NULL) 
			{
				mlog3("No Item ID = %d\n", tdItem.m_nItemID);
				_ASSERT(0);
				continue;
			}

			pNewItem->ApplyTDItem(tdItem);
			gvar.MyInfo.Storage.PushItem(pNewItem->m_nSlotID, pNewItem);
		}
	}

	global.script->GetGlueGameEvent().OnStorageShow();

	return CR_TRUE;
}

// 보관함으로 넣기
MCommandResult XCmdHandler_Storage::OnDepositMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nMoney;
	if (pCommand->GetParameter(&nMoney,		0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.ChrInfo.SetStorageMoney(gvar.MyInfo.ChrInfo.GetStorageMoney() + nMoney);
	gvar.MyInfo.ChrInfo.SetMoney(gvar.MyInfo.ChrInfo.GetMoney() - nMoney);

	global.script->GetGlueGameEvent().OnStorageRefresh();
	global.ui->InventoryMoneyRefresh();

	return CR_TRUE;
}

//보관함에서 빼기
MCommandResult XCmdHandler_Storage::OnWithdrawMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nMoney;
	if (pCommand->GetParameter(&nMoney,		0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.ChrInfo.SetStorageMoney(gvar.MyInfo.ChrInfo.GetStorageMoney() - nMoney);
	gvar.MyInfo.ChrInfo.SetMoney(gvar.MyInfo.ChrInfo.GetMoney() + nMoney);

	global.script->GetGlueGameEvent().OnStorageRefresh();
	global.ui->InventoryMoneyRefresh();

	return CR_TRUE;
}