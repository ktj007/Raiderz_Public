#include "StdAfx.h"
#include "GMoneySystem.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBTaskCharData.h"
#include "GDB_CODE.h"

GMoneySystem::GMoneySystem(void)
{
}

GMoneySystem::~GMoneySystem(void)
{
}

bool GMoneySystem::Increase(GEntityPlayer* pPlayer, int nMoney)
{
	if (NULL == pPlayer) return false;
	if (0 == nMoney) return true;
	if (0 > nMoney) return false;
	if (INT_MAX - nMoney < pPlayer->GetPlayerInfo()->GetMoney()) return false;

	if (false == gsys.pDBManager->CharAddMoney(
		GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), GDB_CODE::CD_L_MONEY_GAIN
		, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetMoney() + nMoney, nMoney, false, 0))) return false;

	return true;
}

bool GMoneySystem::Decrease(GEntityPlayer* pPlayer, int nMoney)
{
	/*
	단순히 돈만 감소시키는데 사용해야 한다.
	다른 것들과의 조합으로 사용하면 안된다.
	*/

	if (NULL == pPlayer) return false;
	if (0 == nMoney) return true;
	if (0 > nMoney) return false;	
	if (nMoney > pPlayer->GetPlayerInfo()->GetMoney()) return false;

	if (false == gsys.pDBManager->CharMinusMoney(GDBT_CHAR_MONEY_INC_DEC(pPlayer->GetUID(), pPlayer->GetAID(), pPlayer->GetCID(), GDB_CODE::CD_L_MONEY_USE
		, pPlayer->GetPlayerInfo()->nPlayTimeSec, pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetMoney() - nMoney, nMoney, false, 0))) return false;

	return true;
}

void GMoneySystem::IncreaseForDBTask(GEntityPlayer* pPlayer, int nMoney)
{
	if (NULL == pPlayer) return;
	if (0 == nMoney) return;

	pPlayer->GetPlayerInfo()->SetMoney(pPlayer->GetMoney() + nMoney);

	RouteUpdateMoney(pPlayer);
}

void GMoneySystem::DecreaseForDBTask(GEntityPlayer* pPlayer, int nMoney)
{
	if (NULL == pPlayer) return;
	if (0 == nMoney) return;

	pPlayer->GetPlayerInfo()->SetMoney(pPlayer->GetMoney() - nMoney);

	RouteUpdateMoney(pPlayer);
}

void GMoneySystem::RouteUpdateMoney( GEntityPlayer* pPlayer)
{
	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_UPDATE_MONEY, 
		1, 
		NEW_INT(pPlayer->GetPlayerInfo()->GetMoney()));

	pPlayer->RouteToMe(pNewCmd);
}