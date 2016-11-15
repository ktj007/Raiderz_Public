#include "StdAfx.h"
#include "GStorageSystem.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GPlayerObjectManager.h"
#include "GDBTaskStorageSerialize.h"
#include "CCommandResultTable.h"
#include "GStorage.h"

bool GStorageSystem::Show( GEntityPlayer* pPlayer )
{
	if (!pPlayer) return false;

	if (!pPlayer->GetItemHolder()->GetStorage().IsSerialized())
	{
		gsys.pDBManager->StorageSerialize(pPlayer->GetUID(), pPlayer->GetCID());		
	}
	else
	{
		RouteShow(pPlayer);
	}

	return true;
}

void GStorageSystem::RouteShow( GEntityPlayer* pPlayer )
{
	int nStorageMoney = 0;
	vector<TD_ITEM> vecTDItem;

	bool bSerialized = pPlayer->GetItemHolder()->GetStorage().IsSerialized();
	if (!bSerialized)
	{
		nStorageMoney = pPlayer->GetStorageMoney();
		pPlayer->GetItemHolder()->MakeTD(SLOTTYPE_STORAGE, vecTDItem);
	}	

	MCommand* pNewCmd = MakeNewCommand(MC_STORAGE_SHOW_UI,
		3,
		NEW_INT(nStorageMoney),
		NEW_BLOB(vecTDItem), 
		NEW_BOOL(bSerialized));

	pPlayer->RouteToMe(pNewCmd);
}

void GStorageSystem::ShowForDBTask(GDBT_STORAGE_SERIALIZE& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;

	data.m_itemInstanceQ.SetToServer(pPlayer);
	RouteShow(pPlayer);

	pPlayer->GetItemHolder()->GetStorage().SetSerialized();
}

bool GStorageSystem::DepositMoney( GEntityPlayer* pPlayer, int nMoney )
{
	if (!pPlayer) return false;
	if (pPlayer->GetMoney() < nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_ENOUGH_MONEY);
	if (MAX_MONEY < pPlayer->GetStorageMoney() + nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_OVER_DEPOSIT_MONEY);

	if (!gsys.pDBManager->StorageUpdateMoney(GDBT_STORAGE_UPDATE_MONEY(GDBT_STORAGE_UPDATE_MONEY::DEPOSIT, pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), pPlayer->GetMoney()-nMoney, pPlayer->GetStorageMoney()+nMoney)))
		return false;

	return true;
}

void GStorageSystem::DepositMoneyForDBTask( GDBT_STORAGE_UPDATE_MONEY& data )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;

	int nDepositMoney = data.m_nStorageMoney - pPlayer->GetStorageMoney();

	pPlayer->GetPlayerInfo()->SetMoney(data.m_nMoney);
	pPlayer->GetPlayerInfo()->SetStorageMoney(data.m_nStorageMoney);

	MCommand* pNewCmd = MakeNewCommand(MC_STORAGE_DEPOSIT_MONEY,
		1,
		NEW_INT(nDepositMoney));

	pPlayer->RouteToMe(pNewCmd);
}

bool GStorageSystem::WithdrawMoney( GEntityPlayer* pPlayer, int nMoney )
{
	if (!pPlayer) return false;
	if (pPlayer->GetStorageMoney() < nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_ENOUGH_STORAGEMONEY);
	if (MAX_MONEY < pPlayer->GetMoney() + nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_OVER_WITHDRAW_MONEY);

	if (!gsys.pDBManager->StorageUpdateMoney(GDBT_STORAGE_UPDATE_MONEY(GDBT_STORAGE_UPDATE_MONEY::WITHDRAW, pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), pPlayer->GetMoney()+nMoney, pPlayer->GetStorageMoney()-nMoney)))
		return false;

	return true;
}

void GStorageSystem::WithdrawForDBTask( GDBT_STORAGE_UPDATE_MONEY& data )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;

	int nWithdrawMoney = data.m_nMoney - pPlayer->GetMoney();

	pPlayer->GetPlayerInfo()->SetMoney(data.m_nMoney);
	pPlayer->GetPlayerInfo()->SetStorageMoney(data.m_nStorageMoney);

	MCommand* pNewCmd = MakeNewCommand(MC_STORAGE_WITHDRAW_MONEY,
		1,
		NEW_INT(nWithdrawMoney));

	pPlayer->RouteToMe(pNewCmd);
}